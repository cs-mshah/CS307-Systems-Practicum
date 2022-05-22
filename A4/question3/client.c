#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define POLYNOMIAL 11011
#define MAXLEN 20000

int connect_socket(int port, struct sockaddr_in *address)
{
    int server_fd = 0;
    struct sockaddr_in serv_addr;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf("\nInvalid address\n");
        return -1;
    }
 
    *address = serv_addr;
    return server_fd;
}

int connect_client(int server_fd, struct sockaddr_in *server_addr)
{
    int client_fd;

    if ((client_fd = connect(server_fd, (struct sockaddr *)server_addr, sizeof(*server_addr))) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    return client_fd;
}

void decode_crc(char *msg, int N, char *result)
{
	char polynomial[5];
	char zeropoly[5] = "00000";
	int reshead = 0;
	int errorflag[N], has_error = 0;

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
            has_error = 1;
			printf("Error in block %d", i);
		}
	}

    if(has_error == 0)
    {
        printf("no errors\n");
    }
}


void crc(int server_fd, int client_fd)
{
    while(1)
    {
        int n, read_length;
        char n_str[MAXLEN] = {0};

        // receive n from server
        read_length = read(server_fd, n_str, MAXLEN - 1);
        n = atoi(n_str);

        printf("n received: %d\n", n);

        // receive msg
        char recvd_msg[MAXLEN] = {0}, res[MAXLEN] = {0};
        read_length = read(server_fd, recvd_msg, MAXLEN - 1);
        printf("recvd: %s\n", recvd_msg);

        // decode crc
        decode_crc(recvd_msg, n, res);
        printf("decoded: %s\n", res);
    }
}

int main(int argc, char const *argv[])
{
    int server_fd = 0, valread, client_fd;
    struct sockaddr_in server_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};

    server_fd = connect_socket(PORT, &server_addr);
    connect_client(server_fd, &server_addr);

    send(server_fd, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    valread = read(server_fd, buffer, 1024);
    printf("%s\n", buffer);

    crc(server_fd, client_fd);

    // close socket
    close(client_fd);
    return 0;
}
