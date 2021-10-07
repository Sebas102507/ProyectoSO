#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#define MAX 50
#define MAX_S 100

struct Ejemplar
{
    int id;
    char status[MAX_S];
    char fecha[MAX_S];
};

struct Libro
{
    char name[MAX_S];
    char ISBN[MAX_S];
    int numEjemp;
    struct Ejemplar ejempl[MAX];
};

void printDB(struct Libro *db)
{
    int i = 0;
    while (1)
    {
        if (strlen(db[i].name) == 0)
        {
            printf("END PRINT\n");
            break;
        }
        else
        {
            printf("Book Name: %s, ISBN: %s, NumEjemplares: %d\n", db[i].name, db[i].ISBN, db[i].numEjemp);
            for (int j = 0; j < db[i].numEjemp; j++)
            {
                printf("      Ejemplar ID: %d, STATUS: %s, Date: %s\n", db[i].ejempl[j].id, db[i].ejempl[j].status, db[i].ejempl[j].fecha);
            }
        }
        i++;
    }
}

struct Libro getLibro(char *libroInfo)
{
    char *tok;
    tok = strtok(libroInfo, ",");
    char *name;
    char *ISBN;
    int numEjempl;
    int i = 0;
    while (tok != NULL)
    {
        //strcpy(tokens[i], tok);
        if (i == 0)
        {
            name = tok;
        }
        else if (i == 1)
        {
            ISBN = tok;
        }
        else
        {
            numEjempl = atoi(tok);
        }
        tok = strtok(NULL, ",");
        i += 1;
    }
    struct Libro libro = {.numEjemp = numEjempl};
    strcpy(libro.name, name);
    strcpy(libro.ISBN, ISBN);
    //struct Libro libro = {.name = name, .ISBN = ISBN, .numEjemp = numEjempl};
    return libro;
}

struct Ejemplar getEjemplar(char *ejemplarInfo)
{
    char *tok;
    tok = strtok(ejemplarInfo, ",");
    int index;
    char *status;
    char *date;
    int i = 0;
    while (tok != NULL)
    {
        //printf("TOK: %s, i: %d\n", tok, i);
        //strcpy(tokens[i], tok);
        if (i == 0)
        {
            index = atoi(tok);
        }
        else if (i == 1)
        {
            status = tok;
        }
        else
        {
            date = tok;
        }
        tok = strtok(NULL, ",");
        i += 1;
    }
    struct Ejemplar ejemplar = {.id = index};
    strcpy(ejemplar.status, status);
    strcpy(ejemplar.fecha, date);
    return ejemplar;
    //struct Libro libro = {.name = name, .ISBN = ISBN, .numEjemp = numEjempl};
}

struct Libro *getDB(char *dataBaseFile)
{
    char fileRoute[100] = "./";
    strcat(fileRoute, dataBaseFile);
    printf("DB FILE: %s\n", fileRoute);
    static struct Libro libros[MAX_S];
    char c;
    char text[100] = "";
    FILE *fp;
    int isBook = 1;
    fp = fopen(fileRoute, "r");
    struct Ejemplar ejemplar;
    struct Libro libro;
    int ejemplarIndex = 0;
    int i = 0;
    int first = 1;
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
                if (first)
                {
                    //printf("--Libro: %s", text);
                    libro = getLibro(text);
                    //printf(", NumEjemplares: %d\n", libro.numEjemp);
                    ejemplarIndex = 0;
                    first = 0;
                }
                else
                {
                    if (ejemplarIndex < libro.numEjemp)
                    {
                        // printf("Ejemplar: %s", text);
                        // printf(", Ejemplar index: %d\n", ejemplarIndex);
                        libro.ejempl[ejemplarIndex] = getEjemplar(text);
                        ejemplarIndex += 1;
                    }
                    else
                    {
                        strcpy(libros[i].name, libro.name);
                        strcpy(libros[i].ISBN, libro.ISBN);
                        libros[i].numEjemp = libro.numEjemp;
                        //printf("I VALUE: %d\n", i);
                        for (int k = 0; k < libros[i].numEjemp; k++)
                        {
                            //printf("Ejemplar status: %s  \n", libro.ejempl[k].status);
                            libros[i].ejempl[k] = libro.ejempl[k];
                        }
                        //printf("--Libro: %s", text);
                        libro = getLibro(text);
                        isBook = 0;
                        //printf(", NumEjemplares: %d\n", libro.numEjemp);
                        ejemplarIndex = 0;
                        i += 1;
                    }
                }

                memset(text, 0, strlen(text));
            }
        }
        //Last book info////////////////////////
        libro.ejempl[ejemplarIndex] = getEjemplar(text);
        strcpy(libros[i].name, libro.name);
        strcpy(libros[i].ISBN, libro.ISBN);
        libros[i].numEjemp = libro.numEjemp;
        for (int k = 0; k < libros[i].numEjemp; k++)
        {
            libros[i].ejempl[k] = libro.ejempl[k];
        }
        ////////////////////////////////////////////////////////
        fclose(fp);
    }
    return libros;
}

void *continueValidation()
{
    char *result = malloc(sizeof(char));
    printf("START\n");
    printf("$");
    scanf("%s", result);
    return (void *)result;
}

void sendRequestRecivedConfirmationMessage(char *messange, char *processId)
{
    int fd;
    char *pipeName = processId;
    do
    {
        fd = open("confirmationPipe", O_WRONLY | O_NONBLOCK);
        if (fd == -1)
        {
            sleep(1);
        }
    } while (fd == -1);
    //printf("Sending throught the pipe :%s\n", messange);
    write(fd, messange, 100);
    //sleep(1);
    close(fd);
}

void confirmationResponsePipe(char *request, char *processId)
{
    char *tok;
    tok = strtok(request, ",");
    char tokens[3][255];
    int i = 0;

    while (tok != NULL)
    {
        strcpy(tokens[i], tok);
        tok = strtok(NULL, ",");
        i += 1;
    }

    if (strcmp(tokens[0], "D") == 0)
    {
        sendRequestRecivedConfirmationMessage("¡¡Return book request recived!!\n", processId);
    }
    else if (strcmp(tokens[0], "R") == 0)
    {
        sendRequestRecivedConfirmationMessage("¡¡Renew book request recived!!\n", processId);
    }
    if (strcmp(tokens[0], "P") == 0)
    {
        sendRequestRecivedConfirmationMessage("¡¡Request book request recived!!\n", processId);
    }
}

int main(int argc, char *argv[])
{

    if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL || argv[4] == NULL)
    {
        printf("Invalid command\n");
    }
    else
    {
        struct Libro *db = getDB(argv[4]);
        printf("<<<<<<<Data base: \n");
        printDB(db);
        int fd;
        char *comm;
        int i = 0;
        char *confirmationPipe;
        pthread_t th2;
        char message[100];
        unlink(argv[2]);
        mkfifo(argv[2], 0);
        chmod(argv[2], 460);

        while (1)
        {
            fd = open(argv[2], O_RDONLY);
            if (fd == -1)
            {
                printf("There was an error reading in Receptor\n");
            }
            while (read(fd, message, 100) > 0)
            {
                if (i == 0)
                {
                    confirmationPipe = message;
                    printf("Process ID: %s\n", message);
                    i += 1;
                }
                else
                {
                    printf("Request: %s\n", message);
                    confirmationResponsePipe(message, confirmationPipe);
                }
            }
            i = 0;
            pthread_create(&th2, NULL, &continueValidation, NULL);
            pthread_join(th2, (void **)&comm);
            if (*comm == 'r')
            {
                printf("...Report: \n");
                // Generate report function
            }
            else if (*comm == 's')
            {
                printf("Finish\n");
                break;
            }
        }
        close(fd);
    }
}