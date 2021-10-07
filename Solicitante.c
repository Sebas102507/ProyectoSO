//
//  main.c
//  Solicitante
//
//  Created by Juan sebastian Vargas torres on 25/09/21.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void workFromMenu(char *pipeReceptor)
{
    /*//printf("START\n");
    char opcion[50];
    int cont = 1;

    while (cont)
    {
        printf("$");
        scanf("%s", opcion);
        if (strcmp(opcion, "s") == 0)
        {
            cont = 0;
            printf("Closed\n");
        }
    }*/
}

void readConfirmationMessage(int fd)
{
    //int fd;
    char *pipeName = "confirmationPipe";
    char message[100];
    unlink(pipeName);
    mkfifo(pipeName, 0);
    chmod(pipeName, 460);
    fd = open(pipeName, O_RDONLY);
    if (fd == -1)
    {
        printf("There was an error");
    }
    while (read(fd, message, 100) > 0)
    {
        printf("%s\n", message);
    }
    //close(fd);
}

void sendDataThroughPipe(char *info, char *pipeReceptor, int infoIndex, int numRequests, int isProcessId)
{
    int fd;
    int fdRead;
    do
    {
        fd = open(pipeReceptor, O_WRONLY | O_NONBLOCK);
        if (fd == -1)
        {
            printf("An error ocurrend with solicitante\n");
            sleep(1);
        }
    } while (fd == -1);
    printf("Sending throught the pipe :%s\n", info);
    write(fd, info, 100);
    if (!isProcessId)
    {
        readConfirmationMessage(fdRead);
    }
    sleep(3);
    if (infoIndex == numRequests - 1)
    {
        printf("CLOSED!!\n");
        close(fd);
        close(fdRead);
    }
}

void workFromFile(char requests[100][100], int numRequests, char *pipeReceptor)
{
    for (int k = 0; k < numRequests; k++)
    {
        if (requests[k][1] != ',')
        {
            //printf("Sending throught the pipe :%s\n", requests[k] + 1);
            sendDataThroughPipe(requests[k] + 1, pipeReceptor, k, numRequests, 0);
        }
        else
        {
            //printf("Sending throught the pipe :%s\n", requests[k]);
            sendDataThroughPipe(requests[k], pipeReceptor, k, numRequests, 0);
        }
    }
}

void getBooksFromFile(char *fileName, char *pipeReceptor)
{
    char fileRoute[100] = "./";
    strcat(fileRoute, fileName);
    char processId[25];
    sprintf(processId, "%d", getpid());
    char requests[100][100];
    char text[100];
    FILE *fp;
    fp = fopen(fileRoute, "r");
    char c;
    int i = 0;
    if (fp)
    {
        while ((c = getc(fp)) != EOF)
        {
            if (c != '\n')
            {
                strncat(text, &c, 1);
            }
            else
            {
                strcpy(requests[i], text);
                memset(text, 0, strlen(text));
                i += 1;
            }
        }
        ///Here the last line of the text file is being saved
        strcpy(requests[i], text);
        memset(text, 0, strlen(text));
        i += 1;
        /////////////////////////////////
        fclose(fp);
        //printf("Sending throught the pipe :%s\n", processId);
        sendDataThroughPipe(processId, pipeReceptor, 0, i, 1);
        workFromFile(requests, i, pipeReceptor);
    }
    else
    {
        printf("File Not Found\n");
    }
}

int main(int argc, const char *argv[])
{
    if (strcmp(argv[1], "-i") == 0)
    {
        if (argv[2] == NULL || argv[3] == NULL || argv[4] == NULL)
        {
            printf("Invalid command\n");
        }
        else
        {
            getBooksFromFile((char *)argv[2], (char *)argv[4]);
        }
    }
    else
    {
        printf("FROM MENU\n");
    }
    return 0;
}
