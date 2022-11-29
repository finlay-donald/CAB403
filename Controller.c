#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <ctype.h>
#include <string.h>

#define MAXDATASIZE 100 /* max number of bytes we can get at once */
#define MAXFILESENDSIZE 1024

int main(int argc, char *argv[])
{
    int sockfd;
    struct hostent *he;
    struct sockaddr_in their_addr; /* connector's address information */

    /* Checking to see if the correct number of arguements has been entered */
    if (argc < 1)
    {
        printf("Usage: controller <address> <port> {[-0 out_file] [-log log_file] \
[-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
        exit(1);
    }

    /* Testing to see if the first arguement passed to the controller is --help */
    if (!strcmp(argv[1], "--help"))
    {
        printf("Usage: controller <address> <port> {[-0 out_file] [-log log_file] \
[-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
        exit(1);
    }

    /* Testing to see if the IP is numerical, and number of arguement is correct */
    if ((strtol(argv[2], NULL, 10) == 0) || argc > 10)
    {
       fprintf(stderr, "Usage: controller <address> <port> {[-0 out_file] [-log log_file] \
[-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
        exit(1);
    }

    int port = atoi(argv[2]);

    if ((he = gethostbyname(argv[1])) == NULL)
    { /* get the host info */
        herror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    /* clear address struct */
    memset(&their_addr, 0, sizeof(their_addr));

    their_addr.sin_family = AF_INET;   /* host byte order */
    their_addr.sin_port = htons(port); /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);

    if (connect(sockfd, (struct sockaddr *)&their_addr,
                sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Could not connect to overseer at <%s> <%d>\n", 
                inet_ntoa(their_addr.sin_addr), port);
        exit(1);
    }
    
    /* Getting just the arguements that need to be passed to the file */
    char *send_args[] = {};
    for (int i=4; i<argc; i++)
    {
        send_args[i-4] = argv[i];
        printf("arguement %d is: %s\n", i-4, send_args[i-4]);
    }
    
    
    /* Sending the file path for file execution in the Controller */
    if (send(sockfd, argv[3], MAXDATASIZE, 0) == -1)
    {
        perror("send");
        exit(1);
    }

    size_t n = (&send_args)[1] - send_args;
    printf("%ld\n", n);
    for (int i=4; i<6; i++)
    {
        /* Sending the arguements that the file has to execute */
        if (send(sockfd, argv[i], strlen(argv[i]), 0) == -1)
        {
            perror("send");
            exit(1);
        }
        char *space = " ";
        if (send(sockfd, space, strlen(space), 0) == -1)
        {
            perror("send");
            exit(1);
        }
    }

    close(sockfd);

    return 0;
}
