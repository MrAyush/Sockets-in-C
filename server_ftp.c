#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
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
    char f_name[128];
};

void make_thread(void *arg) {
    int n;
    char buff[1024];
    struct client_conn c = *((struct client_conn *)arg);
    printf("[+] Incoming connection form (%s, %d)\n", 
        inet_ntoa(c.clientaddr_s.sin_addr), 
        (int) ntohs(c.clientaddr_s.sin_port));
	while(1) {
        struct stat info;
        stat(c.f_name, &info);
        sprintf(buff, "%d", (int)info.st_size);
        n = strlen(buff);
        write(c.client, buff, n);
        printf("Size - %s bytes\n", buff);
        int fd = open(c.f_name, O_RDONLY);
        char *ch = malloc(sizeof(char));
        printf("Sending %s\n", c.f_name);
        while (read(fd, ch, 1) != 0) {
            printf("%s", ch);
            write(c.client, ch, 1);
        }
        close(c.client);
        printf("\nSent\n");
        printf("[-] Closing connection");
        break;
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
            printf("Client_main: %d\n", client);
            c.client = client;
            c.clientaddr_s = clientaddr;
            strcpy(c.f_name, "xyz.txt");
            clients[i++] = client;
            pthread_create(&thread, NULL, (void *) &make_thread, (void *)&c);
            threads[i] = thread;
        }
    }
    for (int j = 0; j < i; j++) {
        pthread_join(threads[j], NULL);
    }
    return 0;
}
