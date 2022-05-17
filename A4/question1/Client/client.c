#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXIN 100
#define MAXOUT 65536
#define MAXFILENAME 100


char *getreq(char *inbuf, int len) {
	printf("Enter message: ");
	memset(inbuf,0,len);
	return fgets(inbuf,len,stdin);
}

void client(int sockfd) {
	int n;
	char sndbuf[MAXIN];
	char rcvbuf[MAXOUT];
	char tmpbuf[MAXOUT];
	while (1) {
		getreq(sndbuf, MAXIN);
		write(sockfd, &sndbuf, strlen(sndbuf));
		memset(rcvbuf,0,MAXOUT);
		memset(tmpbuf,0,MAXOUT);
		n=read(sockfd, &tmpbuf, MAXOUT-1);
		while(tmpbuf[0]==0){
			n=read(sockfd, &tmpbuf, MAXOUT-1);
		}
		printf("Received message: \n");
		if(strncmp(sndbuf, "exit", 4)==0){
			close(sockfd);
			exit(0);
		}
		else if(strncmp(sndbuf, "details", 7)==0){
			write(STDOUT_FILENO, tmpbuf, n);
		}
		else if(strcmp(tmpbuf, "File not found\n")==0){
			write(STDOUT_FILENO, tmpbuf, n);
		}
		else{
			long int filesize = *(long int*)tmpbuf;
			printf("File size: %ld\n",filesize);
			n = send(sockfd, (void*)(&filesize), sizeof(long int),0);
			n = recv(sockfd, (void*)(&rcvbuf),filesize, 0);
			printf("Got %d\n",n);
			char filename[MAXFILENAME]={0};
			FILE *fd;
			strncpy(filename, sndbuf, strlen(sndbuf)-1);
			fd = fopen(filename, "wb");
			fwrite(rcvbuf, sizeof(char), filesize, fd);
			//char* testbuf = "PLZZZZZ";
			//fwrite(testbuf, sizeof(char), sizeof(testbuf), fd);
			//printf("Filesize supposed to be %d, rcvbuf size is %d", filesize, sizeof(rcvbuf));
			printf("File %s written successfully\n", filename);
			fflush(stdout);
			fclose(fd);
			fsync(sockfd);
		}
	}
}

struct hostent *buildServerAddr(struct sockaddr_in *serv_addr, char *serverIP, int portno) {
	memset((char *) serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr->sin_family = AF_INET;
	inet_aton(serverIP, &(serv_addr->sin_addr));
	serv_addr->sin_port = htons(portno);
}

int main() 
{
	char *serverIP = "0.0.0.0";
	int sockfd, portno = 5000;
	struct sockaddr_in serv_addr;

	buildServerAddr(&serv_addr, serverIP, portno);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	printf("Connected to %s:%d\n",serverIP, portno);

	client(sockfd);

}

