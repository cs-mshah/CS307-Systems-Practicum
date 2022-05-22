#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

// Server address
struct hostent *buildServerAddr(struct sockaddr_in *serv_addr,
	char *serverIP, int portno) {
  /* Construct an address for remote server */
  memset((char *) serv_addr, 0, sizeof(struct sockaddr_in));
  serv_addr->sin_family = AF_INET;
  inet_aton(serverIP, &(serv_addr->sin_addr));
  serv_addr->sin_port = htons(portno);
}

int main() 
{
	//Client protocol
	char *serverIP = "192.168.1.2";
	int sockfd, portno = 3000;
	struct sockaddr_in serv_addr;
	
	buildServerAddr(&serv_addr, serverIP, portno);

	/* Create a TCP socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Connect to server on port */
	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connected failed\n");
        close(sockfd);
        return 0;
    }
    printf("Connected to %s:%d\n",serverIP, portno);
    char buff[1000];
    cout << "Commands supported: FILE, EXIT, STAT" << "\n";
    while (1)
    {
        cout << ">> ";
        string x;
        cin >> x;
        if (x!="FILE" && x!="EXIT" && x!="STAT")
        {
            cout << "Invalid command\n";
            continue;
        }
        if (x=="FILE")
        {
            cout << "Enter filename: \n";
            string fname; cin >> fname;
            strcpy(buff, "FILE");
            write(sockfd, buff, 5);
            strcpy(buff, fname.c_str());
	    write(sockfd, buff, 1000);
	    read(sockfd, buff, 9);
	    string response = string(buff);
	    if (response == "NOTFOUND")
	    {
		cout << "File not found on server\n";
		continue;
	    }
	    if (response == "FOUND")
	    {
		read(sockfd, buff, 10);
		int size = stoi(buff);
		cout << "Receiving file from server\n";
		string path = "RECV_" + fname;
		int filefd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		int n;
		while (size && (n=read(sockfd, buff, 1000)) > 0)
		{
			//cout << n << "\n";
			write(filefd, buff, n);
			size -= n;
			// if (n<1000) break;
		}
		cout << "Saved at " << path << "\n";
		close(filefd);
	    }
        }
        if (x=="STAT")
        {
            strcpy(buff, "STAT");
            write(sockfd, buff, 5);
	    string response = "";
	    int n;
	    read(sockfd, buff, 10);
            int size = stoi(buff);
	    while (size && (n=read(sockfd, buff, 1000)) > 0)
            {
		  buff[n]='\0';
                  response += string(buff);
		  size -= n;
            }
	    cout << response << "\n";
        }
        if (x=="EXIT")
        {
            cout << "Exiting\n";
            strcpy(buff, "EXIT");
            write(sockfd, buff, 5);
            close(sockfd);
            break;
        }
    }
}
