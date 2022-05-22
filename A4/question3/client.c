#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

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

void crc(int server_fd, int client_fd)
{
    int divisor_n, read_length;
    char divisor[9] = {0};

    // receive n from server
    read_length = read(server_fd, divisor, 6);
    divisor_n = atoi(divisor);

    printf("n received: %d\n", divisor_n);
    
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
