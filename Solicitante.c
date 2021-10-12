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

char continueValidation()
{
    char result;
    //printf("START\n");
    fflush(stdin);
    printf("¿Continue? (y/n) $");
    scanf(" %c", &result);
    return result;
}

char *getBookInfoFromMenu()
{
    char bookName[100];
    char ISBN[100];
    static char info[255] = "";
    if (strlen(info) > 0)
    {
        memset(info, 0, strlen(info));
    }
    fflush(stdin);
    printf("¿Book Name? $");
    scanf(" %[^\n]s", bookName);
    printf("\n¿ISBN? $");
    fflush(stdin);
    scanf(" %[^\n]s", ISBN);
    strcat(info, ",");
    strcat(info, bookName);
    strcat(info, ",");
    strcat(info, ISBN);
    return info;
}

void readConfirmationMessage(int fd, char *processId)
{
    //int fd;
    char *pipeName = processId;
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
//sendDataThroughPipe(processId, pipeReceptor, 0, i, 1);
void sendDataThroughPipe(char *info, char *pipeReceptor, int infoIndex, int numRequests, int isProcessId, char *processId)
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
        readConfirmationMessage(fdRead, processId);
    }
    sleep(3);
    if (infoIndex == numRequests - 1)
    {
        //printf("CLOSED!!\n");
        close(fd);
        close(fdRead);
    }
}

void workFromMenu(char *pipeReceptor)
{
    int option;
    char processId[25];
    char bookName[100];
    char ISBN[100];
    char conti;
    char request[100] = "";
    sprintf(processId, "%d", getpid());
    while (1)
    {
        printf("\n<<<<<<<Menu>>>>>>>\n");
        printf("1) Return Book\n");
        printf("2) Renew Book\n");
        printf("3) Request Book\n");
        scanf("%d", &option);
        if (option != 1 && option != 2 && option != 3)
        {
            printf("Invalid command\n");
        }
        else
        {
            //getBookInfoFromMenu();
            if (strlen(request) > 0)
            {
                memset(request, 0, strlen(request));
            }
            if (option == 1)
            {
                strcat(request, "D");
                strcat(request, getBookInfoFromMenu());
                //printf("Request: %s\n", request);
            }
            else if (option == 2)
            {
                strcat(request, "R");
                strcat(request, getBookInfoFromMenu());
            }
            else if (option == 3)
            {
                strcat(request, "P");
                strcat(request, getBookInfoFromMenu());
            }
            //printf("Request: %s\n", request);
            sendDataThroughPipe(processId, pipeReceptor, 0, 0, 1, processId);
            sendDataThroughPipe(request, pipeReceptor, 0, 1, 0, processId);
        }

        conti = continueValidation();
        if (conti == 'n' || conti == 'N')
        {
            break;
        }
    }
}

void workFromFile(char requests[100][100], int numRequests, char *pipeReceptor, char *processId)
{
    for (int k = 0; k < numRequests; k++)
    {
        if (requests[k][1] != ',')
        {
            //printf("Sending throught the pipe :%s\n", requests[k] + 1);
            sendDataThroughPipe(requests[k] + 1, pipeReceptor, k, numRequests, 0, processId);
        }
        else
        {
            //printf("Sending throught the pipe :%s\n", requests[k]);
            sendDataThroughPipe(requests[k], pipeReceptor, k, numRequests, 0, processId);
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
        sendDataThroughPipe(processId, pipeReceptor, 0, i, 1, processId);
        workFromFile(requests, i, pipeReceptor, processId);
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
        if (strcmp(argv[1], "-p") == 0 && argv[2] != NULL)
        {
            workFromMenu((char *)argv[2]);
        }
        else
        {
            printf("Invalid command\n");
        }
    }
    return 0;
}
