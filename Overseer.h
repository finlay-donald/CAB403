#ifndef Overseer_H_ /* \Include guard */
#define Overseer_H_

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

#define MAXRECVSIZE 1024 /* max number of bytes that we will receive from the controller */
#define MAXSENDSIZE 100 /* max number of bytes that will be sent back to controller */
#define BACKLOG 10 /* how many pending connections queue will hold */


/* Struct holding Thread Data */
typedef struct str_thdata
{
    int thread_no;
    char message[100];
} thdata;

typedef struct num_thdata
{
    int thread_no;
    int sum_to;
} thsum;


//* for every accepted connection, use a sepetate process or thread to serve it */
void *IncommingConnection (socklen_t sin_size, int new_fd, int sockfd, struct sockaddr_in their_addr) {
     while (1)
    { /* main accept() loop */
        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
                             &sin_size)) == -1)
        {
            perror("accept");
            continue;
        }

        /* Getting the current date and time */
        time_t timer;
        char buffer[26];
        struct tm* tm_info;
        timer = time(NULL);
        tm_info = localtime(&timer);
        strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        printf("%s - ", buffer);
        printf("connection received from %s\n",
               inet_ntoa(their_addr.sin_addr));
        
        /* Recieving from the Overseer */
        /* Receiving the path for the file*/
        int numbytes;
        char buf[MAXRECVSIZE];
        if ((numbytes = recv(new_fd, buf, MAXRECVSIZE, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';

        /* Receiving the arguements for the file */
        int numbytes2;
        char buf2[MAXRECVSIZE];
        char res[MAXRECVSIZE];
        memset(res, 0, sizeof(res));
        for (int i=0; i<4; i++)
        {
            if ((numbytes2 = recv(new_fd, buf2, MAXRECVSIZE, 0)) == -1)
            {
                perror("recv");
                exit(1);
            }
            buf2[numbytes2] = '\0';
            strcat(res, buf2);
            printf("%s\n", buf2);
        }

        printf("%s\n", res);
        

        /* Breaking the received arguements up based on the spaces between them */
        int i = 0;
        char *p = strtok (res, " ");
        char *array[MAXRECVSIZE];
        while (p != NULL)
        {
            array[i++] = p;
            p = strtok(NULL, " ");
        }
        for (i = 0; i < 2; ++i)
        {
            printf("array arg %d is:||%s||\n", i, array[i]);
        }
        

        /*Printing message for attempting to execute */
        timer = time(NULL);
        tm_info = localtime(&timer);
        strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        printf("%s - ", buffer);
        printf("attempting to execute %s\n", buf);

        /* Forking to execute the built file sent over */
        pid_t pid_exec = fork();
        if (pid_exec == 0)
        {
            /*Executing the file received from Controller */
            if (execl(buf, basename(buf), array[0], array[1], NULL) == -1)
            {
                timer = time(NULL);
                tm_info = localtime(&timer);
                strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
                printf("%s - ", buffer);
                printf("could not execute %s", buf);
                for (int i=0; i<2; i++)
                {
                    printf(" %s", array[i]);
                }
                printf("\n");
                exit(1);
            }
            exit(0);
        }

        timer = time(NULL);
        tm_info = localtime(&timer);
        strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        printf("%s - ", buffer);
        printf("%s", buf);
        for (int i=0; i<2; i++)
        {
            printf(" %s", array[i]);
        }
        printf(" has been executed with pid %d\n", pid_exec);

        /* Determining the exit status code of the pid */
        int status;
        waitpid(pid_exec, &status, 0);
        if (WIFEXITED(status))
        {
            int exit_status = WEXITSTATUS(status);
            printf("%d has been terminated with status code %d\n", pid_exec, exit_status);
        }

        close(new_fd); /* parent doesn't need this */

        while (waitpid(-1, NULL, WNOHANG) > 0)
            ; /* clean up child processes */
    }

}

#endif //Overseer_H_