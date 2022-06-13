


#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <err.h>
#include <bits/stdc++.h>
#define MAX_MSG_SIZE 1024
using namespace std;

char* magnet_link = "magnet:?xt=urn:btih:36DBE2D21DC450174BA8040372AB9CB86D699CB1&dn=The%20First%20Modern%20Jew%3A%20Spinoza%20and%20the%20History%20of%20an%20Image%20(2012)&tr=udp%3A%2F%2Ftracker.coppersurfer.tk%3A6969%2Fannounce&tr=udp%3A%2F%2Ftracker.openbittorrent.com%3A6969%2Fannounce&tr=udp%3A%2F%2F9.rarbg.to%3A2710%2Fannounce&tr=udp%3A%2F%2F9.rarbg.me%3A2780%2Fannounce&tr=udp%3A%2F%2F9.rarbg.to%3A2730%2Fannounce&tr=udp%3A%2F%2Ftracker.opentrackr.org%3A1337&tr=http%3A%2F%2Fp4p.arenabg.com%3A1337%2Fannounce&tr=udp%3A%2F%2Ftracker.torrent.eu.org%3A451%2Fannounce&tr=udp%3A%2F%2Ftracker.tiny-vps.com%3A6969%2Fannounce&tr=udp%3A%2F%2Fopen.stealth.si%3A80%2Fannounce";
/*
 * The function get_sockaddr converts the server's address and port into a form usable to create a 
 * scoket
*/
struct addrinfo* get_sockaddr(const char* hostname, const char *port)
{

  struct addrinfo hints;
  struct addrinfo* results;
  int rv;

  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_family = AF_INET;          //Return socket address for the server's IPv4 addresses
  hints.ai_socktype = SOCK_STREAM;    //Return TCP socket addresses

  /* Use getaddrinfo will get address information for the host specified by hostnae */

  rv = getaddrinfo(hostname, port, &hints, &results);
  if (rv != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    exit(1);
  }

  return results;
}

/*
 * The function open_connection establishes a connection to the server
*/
int open_connection(struct addrinfo* addr_list)
{

  struct addrinfo* p;
  int sockfd;
  //Iterate through each addr info in the list; Stop when we successully connect to one

  for (p = addr_list; p != NULL; p = p->ai_next)
  {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        // Try the next address since the socket operation failed
       if (sockfd == -1) continue;

        //Stop iterating of we are able to connect to the server
 
       if (connect(sockfd,p->ai_addr, p->ai_addrlen) != -1) break;
  }

  freeaddrinfo(addr_list);
  
  if (p == NULL)
   err(EXIT_FAILURE, "%s", "Unable to connect");
   else
     return sockfd;

}

void socketConnection(char* IP, char* PORT) {
    int numbytes;
    char sendBuff[MAX_MSG_SIZE];
    char recvBuff[MAX_MSG_SIZE];
    struct sockaddr_in serv_addr; 
    struct hostent *server;
    int rv;
    int sum;

    struct addrinfo* results = get_sockaddr(IP, PORT);
    int sockfd = open_connection(results);

    memset(sendBuff, '0',sizeof(sendBuff));
    memset(recvBuff, '0',sizeof(recvBuff));

    snprintf(sendBuff, sizeof(sendBuff),"%s", magnet_link);
    printf("Clientside sendbuff:%d\n",strlen(sendBuff));
    write(sockfd, sendBuff, strlen(sendBuff)); 

    numbytes = recv(sockfd,recvBuff,sizeof(recvBuff)-1,0);
    if (numbytes == -1){
      perror("recv");
      exit(1);
    }
  
    recvBuff[numbytes] = '\0';
    sscanf(recvBuff, "%d", &sum);
    printf("Result is %d\n", sum);
}

int main(int argc, char *argv[])
{

    // int numbytes;
    // char sendBuff[MAX_MSG_SIZE];
    // char recvBuff[MAX_MSG_SIZE];
    // struct sockaddr_in serv_addr; 
    // struct hostent *server;
    // int rv;
    // int sum;

    // if(argc != 4)
    // {
    //     printf("\n Usage: %s hostname port number_to_be_multiplied\n",argv[0]);
    //     return 1;
    // } 

    vector<pair<char*,char*>> machines = {{"192.168.35.166","5016"}};

    int NUM_MACHINES = machines.size();
    thread threads[NUM_MACHINES];
    
    for(pair<char*,char*> machine: machines) {
      char *IP = machine.first, *PORT = machine.second;

      for(int i=0;i<NUM_MACHINES;i++) {
          threads[i] = thread(socketConnection, IP, PORT);
      }
      for(int i=0;i<NUM_MACHINES;i++) {
          threads[i].join();
      }
    }

    // vector<thread> threads;
    // for(pair<char*,char*> machine: machines) {
    //   char *IP = machine.first, *PORT = machine.second;

    //   thread new_machine(socketConnection,IP,PORT);
    //   threads.push_back(new_machine);

      // struct addrinfo* results = get_sockaddr(IP, PORT);
      // int sockfd = open_connection(results);

      // memset(sendBuff, '0',sizeof(sendBuff));
      // memset(recvBuff, '0',sizeof(recvBuff));

      // snprintf(sendBuff, sizeof(sendBuff),"%s", "magnet:?xt=urn:btih:36DBE2D21DC450174BA8040372AB9CB86D699CB1&dn=The%20First%20Modern%20Jew%3A%20Spinoza%20and%20the%20History%20of%20an%20Image%20(2012)&tr=udp%3A%2F%2Ftracker.coppersurfer.tk%3A6969%2Fannounce&tr=udp%3A%2F%2Ftracker.openbittorrent.com%3A6969%2Fannounce&tr=udp%3A%2F%2F9.rarbg.to%3A2710%2Fannounce&tr=udp%3A%2F%2F9.rarbg.me%3A2780%2Fannounce&tr=udp%3A%2F%2F9.rarbg.to%3A2730%2Fannounce&tr=udp%3A%2F%2Ftracker.opentrackr.org%3A1337&tr=http%3A%2F%2Fp4p.arenabg.com%3A1337%2Fannounce&tr=udp%3A%2F%2Ftracker.torrent.eu.org%3A451%2Fannounce&tr=udp%3A%2F%2Ftracker.tiny-vps.com%3A6969%2Fannounce&tr=udp%3A%2F%2Fopen.stealth.si%3A80%2Fannounce");
      // printf("Clientside sendbuff:%d\n",strlen(sendBuff));
      // write(sockfd, sendBuff, strlen(sendBuff)); 

      // numbytes = recv(sockfd,recvBuff,sizeof(recvBuff)-1,0);
      // if (numbytes == -1){
      //   perror("recv");
      //   exit(1);
      // }
    
      // recvBuff[numbytes] = '\0';
      // sscanf(recvBuff, "%d", &sum);
      // printf("Result is %d\n", sum);
    // }

    // for(auto& t: threads) {
    //   t.join();
    // }

    
    return 0;
}
