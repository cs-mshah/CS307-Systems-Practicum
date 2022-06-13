#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/wait.h>
#include <sched.h>
#include "queue.hpp"
#include <bits/stdc++.h>
#include "test.hpp"

#define MAX_MSG_SIZE 1024

using namespace std;

/*Declare global variables*/
pthread_mutex_t mut;
pthread_cond_t cond;
queue1* q;


/*
 * This function receives receives one number from the client and multiplies 
 * it by 10 and sends it back
 */
void doProcessing (int connfd)
{
        char recvBuff[MAX_MSG_SIZE], sendBuff[MAX_MSG_SIZE];
        // string a;
        // char* a;
        int result;

        memset(recvBuff, '0', sizeof(recvBuff));
        memset(sendBuff, '0', sizeof(sendBuff)); 

        /*receive data from the client*/
        int numbytes = recv(connfd,recvBuff, sizeof(recvBuff)-1,0);
        if (numbytes == -1){
                perror("recv");
                exit(1);
        }
        recvBuff[numbytes] = '\0';
        cout<<"Numbytes is:"<<numbytes<<endl;
        // printf("Printing rcvbuf:%s", recvBuff);
        cout<<"size of recv buffer is:"<<sizeof(recvBuff)<<'\n';
        /*Extract the two numbers */
        // sscanf(recvBuff, "%s", a);
        string a = recvBuff;

        cout<<"Printing magnet link:\n";
        // cout << a << endl;
        cout<<"Magnet link is:"<<a<<'\n';
        // printf("Magnet link is:%s",a);
        /*Multiply the answer by 10*/
        result = 10 * 10;
        torrent(a);

        snprintf(sendBuff, sizeof(sendBuff), "%d", result);

        send(connfd, sendBuff, strlen(sendBuff),0); 

        close(connfd);
}

/*
 * This method locks down the connection queue1 then utilizes the queue1.h push function
 * to add a connection to the queue1. Then the mutex is unlocked and cond_signal is set
 * to alarm threads in cond_wait that a connection as arrived for reading
 */
void queue1_add(int value)
{
        /*Locks the mutex*/
        pthread_mutex_lock(&mut);

        push(q, value);

        /*Unlocks the mutex*/
        pthread_mutex_unlock(&mut);

        /* Signal waiting threads */
        pthread_cond_signal(&cond);
}

/*
 * This method locks down the connection queue1 then utilizes pthread_cond_wait() and waits
 * for a signal to indicate that there is an element in the queue1. Then it proceeds to pop the 
 * connection off the queue1 and return it
 */
int queue1_get()
{
       /*Locks the mutex*/
        pthread_mutex_lock(&mut);

        /*Wait for element to become available*/
        while(empty(q) == 1)
        {
                printf("Thread %lu: \tWaiting for Connection\n", pthread_self());
                if(pthread_cond_wait(&cond, &mut) != 0)
                {
                    perror("Cond Wait Error");
                }
        }

        /*We got an element, pass it back and unblock*/
        int val = peek(q);
        pop(q);

        /*Unlocks the mutex*/
        pthread_mutex_unlock(&mut);

        return val;
}

static void* connectionHandler(void* dummy)
{
        int connfd = 0;

        /*Wait until tasks is available*/
        while(1)
        {
                connfd = queue1_get();
                printf("Handler %lu: \tProcessing\n", pthread_self());
                /*Execute*/
                doProcessing(connfd);
        }
}

int main(int argc, char *argv[])
{
        /*Command line argument: port number*/
        if(argc != 4)
        {
                printf("\n Usage: %s port number_of_threads size_of_connections_array\n",argv[0]);
                return 1;
        }

        q = createqueue1(atoi(argv[3]));

        int listenfd = 0, connfd = 0;
        struct sockaddr_in serv_addr; 
        int rv;

        /*Initialize the mutex global variable*/
        pthread_mutex_init(&mut,NULL);

        /*Declare the thread pool array*/
        pthread_t threadPool[atoi(argv[2])];

        /*Socket creation and binding*/
        listenfd = socket(AF_INET, SOCK_STREAM, 0);

        if (listenfd <  0) 
        {
                perror("Error in socket creation");
                exit(1);
        }

        memset(&serv_addr, '0', sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(atoi(argv[1])); 

        rv = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

        if (rv <  0) 
        {
                perror("Error in binding");
                exit(1);
        }

        /*Make Thread Pool*/
        for(int i = 0; i < atoi(argv[2]); i++)
        {
                pthread_create(&threadPool[i], NULL, connectionHandler, (void *) NULL);
        }

        listen(listenfd, 10); 

        /*Accept connection and push them onto the stack*/
        while(1)
        {
                printf("\nMain: \t\t\t\tAccepting Connections\n");

                /*The accept call blocks until a connection is found
                 * then the connection is pushed onto the queue1 by queue1_add*/
                queue1_add(accept(listenfd, (struct sockaddr*)NULL, NULL)); 

                printf("Main: \t\t\t\tConnection Completed\n\n");
        }
}
