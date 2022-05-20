#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXIN 20
#define MAXOUT 25600
#define POLYNOMIAL 10101

void decodeCRC(char *msg, int N, char *result)
{
	char polynomial[5];
	char zeropoly[5] = "00000";
	int reshead = 0;
	int errorflag[N];
	sprintf(polynomial, "%d", POLYNOMIAL);
	for (int i = 0; i < 12 * N; i += 12)
	{
		char msgblock[12] = {0};
		strncpy(msgblock, msg + i, 12);
		for (int j = 0; j < 7; j++)
		{
			int flag;
			if (msgblock[j] == '1')
				flag = 1;
			else
				flag = 0;
			msgblock[j] = '0';
			for (int k = j + 1; k < j + 5; k++)
			{
				if (flag == 1)
				{
					if (msgblock[k] == polynomial[k - j])
						msgblock[k] = '0';
					else
						msgblock[k] = '1';
				}
				else
				{
					if (msgblock[k] == zeropoly[k - j])
						msgblock[k] = '0';
					else
						msgblock[k] = '1';
				}
			}
		}
		if (strcmp(msgblock + 8, "0000") == 0)
			errorflag[i / 12] = 0;
		else
			errorflag[i / 12] = 1;
		strncpy(result + reshead, msg + i, 8);
		reshead += 8;
	}
	for (int i = 0; i < N; i++)
	{
		if (errorflag[i] == 1)
		{
			printf("Error in block %d", i);
		}
	}
	fflush(stdout);
}

char *getreq(char *inbuf, int len)
{
	printf("Enter N in (8*N)");
	memset(inbuf, 0, len);
	return fgets(inbuf, len, stdin);
}

void client(int sockfd)
{
	int n;
	int sndbuf;
	char rcvbuf[MAXOUT];
	char resbuf[MAXOUT] = {0};
	printf("Enter N in (8*N): ");
	scanf("%d", &sndbuf);
	while (sndbuf != 0)
	{
		write(sockfd, &sndbuf, sizeof(int));

		memset(rcvbuf, 0, MAXOUT);
		n = read(sockfd, rcvbuf, MAXOUT - 1);

		decodeCRC(rcvbuf, sndbuf, resbuf);
		printf("Recieved message: \n");
		write(STDOUT_FILENO, resbuf, n);
		printf("\nEnter N in (8*N): ");
		scanf("%d", &sndbuf);
	}
}

struct hostent *buildServerAddr(struct sockaddr_in *serv_addr, char *serverIP, int portno)
{
	memset((char *)serv_addr, 0, sizeof(struct sockaddr_in));
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

	connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	printf("Connected to %s:%d\n", serverIP, portno);

	client(sockfd);

	close(sockfd);
}
