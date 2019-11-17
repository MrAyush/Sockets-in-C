#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>

#define MAX_THREAD 10
#define MAX_CLIENT 10

struct sockaddr_in servaddr, clientaddr;

pthread_t threads[MAX_THREAD];
int clients[MAX_CLIENT] = {-1};

struct client_conn
{
    int client;
    struct sockaddr_in clientaddr_s;
};

void make_thread(void *arg) {
    int n;
    char buff[1024];
    struct client_conn c = *((struct client_conn *)arg);
    printf("[+] Incoming connection form (%s, %d)\n", 
        inet_ntoa(c.clientaddr_s.sin_addr), 
        (int) ntohs(c.clientaddr_s.sin_port));
	while(1) {
        n = read(c.client, buff, 1024);
        printf("Client_thread %d: ", c.client);
        buff[n] = '\0';
        printf("%s\n", buff);
        if (!strcmp(buff, "exit") || n == 0) {
            printf("[-] Closing connection to client (%s)\n", inet_ntoa(c.clientaddr_s.sin_addr));
            close(c.client);
            return;
        } else {
            for (int j = 0; j < MAX_CLIENT; j++) {
                if (clients[j] != c.client && clients[j] != 0) {
                    printf(" %d\n", clients[j]);
                    write(clients[j], buff, n);
                }
            }
        }
    }
}
int main(int argc, char** argv) {
    int len = sizeof(struct sockaddr_in);
    int i = 0;

    // socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int client;

	servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(0);

    // bind
    bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));
    getsockname(sock, (struct sockaddr *) &servaddr, &len);
	printf("[ ] After bind ephemeral port: %d\n", (int)ntohs(servaddr.sin_port));
	
    // listen
    listen(sock, 5);

    while(1) {
        // accept
        client = accept(sock, (struct sockaddr *) &clientaddr, &len);
        if (client != -1) {
            pthread_t thread;
            struct client_conn c;
            printf("Client_main: %d", client);
            c.client = client;
            c.clientaddr_s = clientaddr;
            clients[i++] = client;
            pthread_create(&thread, NULL, (void *) &make_thread, (void *)&c);
            threads[i] = thread;
        }
    }
    for (int j = 0; j < i; j++) {
        pthread_join(threads[j], NULL);
        close(clients[j]);
    }
    return 0;
}
