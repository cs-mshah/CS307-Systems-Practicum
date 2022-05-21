#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <set>
using namespace std;
#define MAXQUEUE 10
//mutex streammutex;

void serve(int consockfd, int i)
{
    char buffer[1024];
    int numBytesSent = 0;
    set <string> filesDownloaded;
    while (1)
    {
        int x = read(consockfd, buffer, 5);
	if (x<5)
	{
		cout << "No input from client " << i << ". Closing connection\n";
		close(consockfd);
		break;
	}
        string input = string(buffer);
        // cout << input << " " << input.size() << "\n";
	if (input == "FILE")
	{
		int n=read(consockfd, buffer, 1000);
		if (n<=0)
		{
			continue;
		}
		string fname = string(buffer);
		cout << "Client " << i << " requested file " << fname << "\n";
		int size=0;
		FILE * f = fopen(fname.c_str(), "r");
		if (f!=NULL)
		{
			fseek(f, 0, SEEK_END);
                        size = ftell(f);
		}
		fclose(f);
		int filed = open(fname.c_str(), O_RDONLY);
		if (filed==-1)
		{
			cout << "File not found\n";
			strcpy(buffer, "NOTFOUND");
			write(consockfd, buffer, 9);
		}
		else
		{
			cout << "Sending file " << fname << "\n";
			strcpy(buffer, "FOUND");
			write(consockfd, buffer, 9);
			string sz = to_string(size);
			strcpy(buffer, sz.c_str());
			write(consockfd, buffer, 10);
			int n;
			while((n=read(filed, buffer, 1000))>0) {
				numBytesSent += n;
				if (write(consockfd, buffer, n) == -1)
				{
					cout << "Write failed\n";
					break;
				}
			}
			filesDownloaded.insert(fname);
			cout << "Finished sending file\n";
		}

	}
        if (input == "EXIT")
        {
            cout << "Client exited\n";
            return;
        }
	if (input == "STAT")
	{
	    string response = "";
	    response += ("Number of bytes sent = " + to_string(numBytesSent) + "\n");
	    response += "Files downloaded: \n";
	    for (auto &x: filesDownloaded)
	    {
		response += x;
		response += "\n";
	    }
	    int sendsize = response.size()+1;
	    char stat[1024];
	    int offset = 0;

	    strcpy(buffer, to_string(sendsize).c_str());
            write(consockfd, buffer, 10);


	    strcpy(stat, response.c_str());
            while (sendsize)
	    {
		if (sendsize >= 1000)
		{
			write(consockfd, stat+offset, 1000);
			offset += 1000;
			sendsize -= 1000;
		}
		else
		{
			write(consockfd, stat+offset, sendsize);
			sendsize=0;
		}
	    }
	    cout << "Sending usage stats to client " << i << "\n";
	}
    }
}

int main()
{
    int lstnsockfd, consockfd, portno = 3000;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    memset((char *) &serv_addr,0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port        = htons(portno);

    // Server protocol
    /* Create Socket to receive requests*/
    lstnsockfd = socket(AF_INET, SOCK_STREAM, 0);

    /* Bind socket to port */
    bind(lstnsockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr));
    printf("Bounded to port\n");
    vector <thread> threads;
    int i=0;
    while (1)
    {
        printf("Listening for incoming connections\n");

        /* Listen for incoming connections */
        listen(lstnsockfd, MAXQUEUE); 

        /* Accept incoming connection, obtaining a new socket for it */
        consockfd = accept(lstnsockfd, (struct sockaddr *) &cli_addr, &clilen);
        
        printf("Accepted connection from client no %d\n", i);
        threads.push_back(thread(serve, consockfd, i));
        ++i;
    }
    close(lstnsockfd);
}
