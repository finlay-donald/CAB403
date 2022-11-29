#ifndef Controller_H_ /* \Include guard */
#define Controller_H_

void Send_Array_Data(int socket_id, char *myArray, int count)
{
    // for (int i = 0; i < 1; i++)
    // {
    //     printf("%s", myArray[i]);
    // }
    int tempNum = count;
    char message;
    printf("Sending int = %d\n", tempNum);
    for (int i = 0; i < ntohl(tempNum); i++)
    {
        message = myArray[i];
        if (send(socket_id, &message, sizeof(char), 0) < 0)
        {
            perror("send");
            exit(1);
        }
    }
}

#endif //Controller_H_