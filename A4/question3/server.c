#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#define MAXREQ 25600
#define MAXQUEUE 5
#define POLYNOMIAL 10101

void encodeCRC(char *msg, int N, char *result)
{
	char polynomial[5];
	char zeropoly[5] = "00000";
	int reshead = 0;
	sprintf(polynomial, "%d", POLYNOMIAL);
	for (int i = 0; i < 8 * N; i += 8)
	{
		char msgblock[12] = "000000000000";
		strncpy(msgblock, msg + i, 8);
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
		strncpy(result + reshead, msg + i, 8);
		strncpy(result + reshead + 8, msgblock + 8, 4);
		reshead += 12;
	}
}

void genMsg(char *msg, int N)
{
	srand(time(0));
	for (int i = 0; i < 8 * N; i++)
	{
		msg[i] = (rand() % 2) + '0';
	}
	printf("Generated: %s\n", msg);
}

void server(int consockfd)
{
	int reqbuf;
	int n;
	while (1)
	{
		printf("Waiting for N..\n");
		n = read(consockfd, &reqbuf, sizeof(int));
		printf("Recvd N: %d\n", reqbuf);
		char sndbuf[MAXREQ] = {0};
		char resbuf[MAXREQ] = {0};
		genMsg(sndbuf, reqbuf);
		encodeCRC(sndbuf, reqbuf, resbuf);
		if (n <= 0)
			return;
		sleep(5);
		printf("Sending: %s\n", resbuf);
		fflush(stdout);
		n = write(consockfd, resbuf, strlen(resbuf));
	}
}

int main()
{
	int lstnsockfd, consockfd, clilen, portno = 5000;
	struct sockaddr_in serv_addr, cli_addr;

	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	lstnsockfd = socket(AF_INET, SOCK_STREAM, 0);

	bind(lstnsockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	printf("Bounded to port\n");
	while (1)
	{
		printf("Listening for incoming connections\n");
		listen(lstnsockfd, MAXQUEUE);

		consockfd = accept(lstnsockfd, (struct sockaddr *)&cli_addr, &clilen);
		printf("Accepted connection\n");

		server(consockfd);

		close(consockfd);
	}
	close(lstnsockfd);
}
