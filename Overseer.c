
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <libgen.h>
#include <pthread.h>   /* POSIX Threads */

#include "Overseer.h"

#define MAXRECVSIZE 1024 /* max number of bytes that we will receive from the controller */
#define MAXSENDSIZE 100 /* max number of bytes that will be sent back to controller */
#define BACKLOG 10 /* how many pending connections queue will hold */

int main(int argc, char *argv[])
{
    int sockfd, new_fd;            /* listen on sock_fd, new connection on new_fd */
    struct sockaddr_in my_addr;    /* my address information */
    struct sockaddr_in their_addr; /* connector's address information */
    pthread_t thread1, thread2, thread3, thread4, thread5; /* thread variables */
    socklen_t sin_size;

    thdata data1, data2; 

    /* generate the socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    if (argc != 2) {
        fprintf(stderr, "usage: <port number>\n");
        exit(1);
    }

    /* Enable address/port reuse, useful for overseer development */
    int opt_enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt_enable, sizeof(opt_enable));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt_enable, sizeof(opt_enable));

    int myport = atoi(argv[1]);

    /* clear address struct */
    memset(&my_addr, 0, sizeof(my_addr));

    /* generate the end point */
    my_addr.sin_family = AF_INET;         /* host byte order */
    my_addr.sin_port = htons(myport);     /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */

    /* bind the socket to the end point */
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    /* start listnening */
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("Overseer starts listening ...\n");

    /* repeat: accept, send, close the connection */
    /* for every accepted connection, use a sepetate process or thread to serve it */
    
    char *message1 = "Thread 1";

    pthread_create(&thread1,NULL,IncommingConnection(sin_size, new_fd,  sockfd, their_addr),(void*)&message1);
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);
    pthread_join(thread5, NULL);
}
