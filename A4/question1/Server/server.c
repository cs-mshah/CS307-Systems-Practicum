#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#define MAXREQ 65536
#define MAXQUEUE 5
#define MAXFILE 100
#define MAXFILENAME 100

typedef struct Users{
	int id, totalsize, nfiles;
	char filelist[MAXFILE][MAXREQ];
} User;

User* users[MAXQUEUE];
int available[MAXQUEUE] = {0};

void handleClient(int consockfd, char* reqbuf, char* sndbuf, User* user){
	FILE *fd;
	char filename[MAXFILENAME]={0};
	if(strncmp(reqbuf, "details", 7)==0){
		char *filelst, *tmp;
		filelst = (char*)malloc(1000*sizeof(char));
		tmp=filelst;
		bzero(filelst, 1000);
		for(int i=0;i<user->nfiles;i++){
			strncpy(filelst, user->filelist[i], strlen(user->filelist[i]));
			filelst+=strlen(user->filelist[i]);
			strncpy(filelst,"\n",1);
			filelst+=1;
		}
		snprintf(sndbuf, MAXREQ, "User id: %d\nFile size transfered: %d\nList of Files: %s\n", user->id, user->totalsize, tmp);
	}
	else if(strncmp(reqbuf, "exit", 4)==0){
		snprintf(sndbuf, MAXREQ, "exiting");
	}
	else{
		strncpy(filename, reqbuf, strlen(reqbuf)-1);
		fd = fopen(filename,"r");
		printf("requested %s by user %d\n",filename,user->id);
		if(fd != NULL){
			int n;
			long int filesize;
			char tmpbuf[MAXREQ];
			memset(tmpbuf,0, MAXREQ);
			fseek(fd, 0L, SEEK_END);
			filesize = ftell(fd);
			fseek(fd, 0L, SEEK_SET);
			printf("File size: %ld\n",filesize);
			n = send(consockfd, (void*)(&filesize), sizeof(long int),0);
			n = read(consockfd,tmpbuf,MAXREQ-1);
			printf("confirm File size: %ld\n",*(long int*)tmpbuf);
			if(*(long int*)tmpbuf == filesize){
				printf("User %d has made preparations to accept file of size %ld\n", user->id, filesize);
			}
			user->totalsize += fread(sndbuf, sizeof(char),MAXREQ,fd);
			strncpy(user->filelist[user->nfiles], filename, strlen(filename));
			user->nfiles+=1;
		}
		else{
			strncpy(sndbuf, "File not found\n", 16);
		}
		close(fd);
	}
	fsync(consockfd);
}

void* server(void* sockfd) {
	char reqbuf[MAXREQ]={0};
	int n, id;
	int consockfd = *((int*)sockfd);
	User user;
	
	for(id=0;id<MAXQUEUE;id++){
		if(available[id]==0){
			available[id]=1;
			break;
		}
	}
	
	user.id=id;
	user.totalsize=0;
	user.nfiles=0;
	for(int i=0;i<MAXFILE;i++){
		for(int j=0;j<MAXFILENAME;j++){
			user.filelist[i][j]=0;
		}
	}

	users[id] = &user;

	while (1) {
		bzero(reqbuf, MAXREQ);
		n = read(consockfd,&reqbuf,MAXREQ-1);
		char sndbuf[MAXREQ]={0};
		if (n <= 0) return;
		sleep(5);
		handleClient(consockfd, reqbuf, sndbuf, users[id]);
		fflush(stdout);
		n = send(consockfd, (void*)(sndbuf), sizeof(sndbuf),0);
		if(strncmp(sndbuf, "exiting", 7)==0){
			break;
		}
	}
	printf("User: %d exit\n", users[id]->id);
	available[id]=0;
	close(consockfd);
	pthread_exit(NULL);
}

int main() 
{
	int lstnsockfd, consockfd, clilen, portno = 5000;
	struct sockaddr_in serv_addr, cli_addr;

	memset((char *) &serv_addr,0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	lstnsockfd = socket(AF_INET, SOCK_STREAM, 0);

	bind(lstnsockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr));
	printf("Bounded to port\n");
	listen(lstnsockfd, MAXQUEUE); 
	pthread_t tid[MAXQUEUE];
	int nthreads = 0;
	while (1) {
	printf("Listening for incoming connections\n");

	consockfd = accept(lstnsockfd, (struct sockaddr *) &cli_addr,&clilen);
	printf("Accepted connection\n");
	if(pthread_create(&tid[nthreads++], NULL, server, &consockfd) != 0)
		printf("Failed thread creation");

	}
	
	if(nthreads>=MAXQUEUE){
		int j=0;
		while(j<MAXQUEUE){
			pthread_join(tid[j++], NULL);
		}
	}

	close(lstnsockfd);
	return 0;
}

