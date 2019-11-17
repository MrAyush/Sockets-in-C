#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>

int main(int argc, char ** argv) {
	int listenfd, connfd, len, ib, n;
	unsigned char *ptr;
	struct sockaddr_in servaddr, clientaddr;
	char buff[1024];
	time_t ticks;
	len = sizeof(struct sockaddr_in);
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(0);
	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	getsockname(listenfd, (struct sockaddr *) &servaddr, &len);
	printf("After bind ephemeral port: %d\n", (int)ntohs(servaddr.sin_port));
	listen(listenfd, 5);
	
	connfd = accept(listenfd, (struct sockaddr *) &clientaddr, &len);
	printf("Client port:%d\n", (int) ntohs(clientaddr.sin_port));
	printf("Client ip:%s\n", inet_ntoa(clientaddr.sin_addr));
	 
	write(connfd, "UNP", 7);
	n = read(connfd, buff, 1024);
	printf("Number of bytes received from server: %d\n", n);
	buff[n] = 0;
	printf("%s\n", buff);
	close(connfd);
}
