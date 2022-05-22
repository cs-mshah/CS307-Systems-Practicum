#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define POLYNOMIAL 11011
#define MAXLEN 20000

int connect_socket(int port, struct sockaddr_in* address)
{
    int server_fd, new_socket, valread;
    // struct sockaddr_in address;
    int opt = 1;

    // create socket fd
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // attach socket to port
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    (*address).sin_family = AF_INET;
    (*address).sin_addr.s_addr = INADDR_ANY;
    (*address).sin_port = htons(port);

    // bind socket
    if (bind(server_fd, (struct sockaddr *) address, sizeof(*address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

   return server_fd;
}

int accept_connection(int server_fd, struct sockaddr* address)
{
    int new_socket, addrlen = sizeof((*address));;

    // accept connection
    printf("waiting for connection request\n");
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    return new_socket;
}

void generate_msg(char *msg, int N)
{
	srand(time(0));
	for (int i = 0; i < 8 * N; i++)
	{
		msg[i] = (rand() % 2) + '0';
	}
}

void encode_crc(char *msg, int N, char *result)
{
	char polynomial[5];
	char zeropoly[5] = "00000";
	int reshead = 0;
	sprintf(polynomial, "%d", POLYNOMIAL);
	// blocks of 8
	for (int i = 0; i < 8 * N; i += 8)
	{
		// 8 + 4 will be resultant block
		char msgblock[12] = "000000000000";
		strncpy(msgblock, msg + i, 8);
		printf("%s\n", msgblock);
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
        // copy 8-block initial string
		strncpy(result + reshead, msg + i, 8);
        // add 4 blocks crc
		strncpy(result + reshead + 8, msgblock + 8, 4);
		reshead += 12;
	}
}


void crc_client(int server_fd, int client_fd)
{
    while(1)
    {
        int n;
        char n_string[9];

        printf("\nEnter N: ");
        scanf("%d", &n);
        sprintf(n_string, "%d", n);

        // send n to client
        send(client_fd, n_string, strlen(n_string), 0);

        // generate msg
        char msg[MAXLEN] = {0}, encoded_msg[MAXLEN] = {0};
        generate_msg(msg, n);
        printf("generated: %s\n", msg);

        // encode msg
        encode_crc(msg, n, encoded_msg);
        printf("encoded msg: %s\n", encoded_msg);

        // send encoded message to client
        send(client_fd, encoded_msg, strlen(encoded_msg), 0);
    }
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    server_fd = connect_socket(PORT, &address);
    new_socket = accept_connection(server_fd, &address);

    printf("client connected\n");
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    crc_client(server_fd, new_socket);

    close(new_socket);
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}
