#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define MAX 50
#define MAX_S 100

struct arg_struct
{
    struct Libro *argDB;
    char *argISBN;
    char *fileRoute;
};

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

char *getCurrentDate()
{
    static char date[MAX_S];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    //strfcat(date, "%d-%d-%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    sprintf(date, "%d-%d-%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    return date;
}

void updateDB(struct Libro *db, char *fileRoute)
{
    printf("Updating DB... ");
    int i = 0;
    FILE *file;
    file = fopen(fileRoute, "w+");
    if (file != NULL)
    {
        //fputs("This is testing for fputs...\n", file);
        while (1)
        {
            if (strlen(db[i].name) == 0)
            {
                //printf("END PRINTakldfakl\n");
                break;
            }
            else
            {
                //printf("HELLO\n");
                fprintf(file, "%s,%s,%d\n", db[i].name, db[i].ISBN, db[i].numEjemp);
                for (int j = 0; j < db[i].numEjemp; j++)
                {
                    if (j == db[i].numEjemp - 1 && strlen(db[i + 1].ISBN) == 0)
                    {
                        fprintf(file, "%d,%s,%s", db[i].ejempl[j].id, db[i].ejempl[j].status, db[i].ejempl[j].fecha);
                    }
                    else
                    {
                        fprintf(file, "%d,%s,%s\n", db[i].ejempl[j].id, db[i].ejempl[j].status, db[i].ejempl[j].fecha);
                    }
                    //printf("      Ejemplar ID: %d, STATUS: %s, Date: %s\n", db[i].ejempl[j].id, db[i].ejempl[j].status, db[i].ejempl[j].fecha);
                }
            }
            i++;
        }
    }
    printf("OK\n");
    fclose(file);
}

//void *returnBook(struct Libro *db, char *bookName, char *ISBN)
void returnBook(void *context)
{
    struct arg_struct *arguments = context;
    int newNumEjem;
    struct Ejemplar newEjemplar;
    //printf("HEllo!!!!!!! %s\n", arguments->argISBN);
    int i = 0;
    while (1)
    {
        if (strlen(arguments->argDB[i].name) == 0)
        {
            //printf("END PRINT\n");
            break;
        }
        if (strcmp(arguments->argDB[i].ISBN, arguments->argISBN) == 0)
        {
            //newEjemplar.fecha
            //strcpy(newEjemplar.fecha, getCurrentDate());
            newNumEjem = arguments->argDB[i].numEjemp + 1;
            newEjemplar.id = newNumEjem;
            strcpy(newEjemplar.fecha, getCurrentDate());
            strcpy(newEjemplar.status, "D");
            arguments->argDB[i].ejempl[arguments->argDB[i].numEjemp] = newEjemplar;
            arguments->argDB[i].numEjemp = newNumEjem;
        }
        i++;
    }
    updateDB(arguments->argDB, arguments->fileRoute);
}

void printDB(struct Libro *db)
{
    int i = 0;
    while (1)
    {
        if (strlen(db[i].name) == 0)
        {
            //printf("END PRINT\n");
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
}

struct Libro *getDB(char *dataBaseFile)
{
    printf("Loading Database...");
    char fileRoute[100] = "./";
    strcat(fileRoute, dataBaseFile);
    //printf("DB FILE: %s\n", fileRoute);
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
    printf("  OK\n");
    return libros;
}

void *continueValidation()
{
    char *result = malloc(sizeof(char));
    //printf("START\n");
    printf("$");
    scanf("%s", result);
    return (void *)result;
}

void sendRequestRecivedConfirmationMessage(char *messange, char *processId)
{
    int fd;
    do
    {
        fd = open(processId, O_WRONLY | O_NONBLOCK);
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

void confirmationResponsePipe(struct Libro *db, char *buffer[MAX_S], char *processId, char *fileRoute)
{
    char *tok;
    tok = strtok(buffer[0], ",");
    char tokens[3][255];
    int i = 0;
    pthread_t th1;
    struct arg_struct arguments;
    while (tok != NULL)
    {
        strcpy(tokens[i], tok);
        tok = strtok(NULL, ",");
        i += 1;
    }

    if (strcmp(tokens[0], "D") == 0)
    {
        sendRequestRecivedConfirmationMessage("¡¡Return book request recived!!\n", processId);
        arguments.argDB = db;
        arguments.argISBN = tokens[2];
        arguments.fileRoute = fileRoute;
        pthread_create(&th1, NULL, (void *)returnBook, &arguments);
    }
    else if (strcmp(tokens[0], "R") == 0)
    {
        sendRequestRecivedConfirmationMessage("¡¡Renew book request recived!!\n", processId);
        pthread_create(&th1, NULL, (void *)returnBook, &arguments);
    }
    if (strcmp(tokens[0], "P") == 0)
    {
        sendRequestRecivedConfirmationMessage("¡¡Request book request recived!!\n", processId);
        pthread_create(&th1, NULL, (void *)returnBook, &arguments);
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
        //printDB(db);
        int fd;
        char *comm;
        int i = 0;
        char confirmationPipe[MAX_S];
        char *buffer[MAX_S];
        pthread_t th2;
        char message[100];
        int proId;
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
                printf("Reading...\n");
                if (i == 0)
                {
                    proId = atoi(message);
                    printf("Process ID: %d\n", proId);
                    i += 1;
                }
                else
                {
                    if (atoi(message) != proId)
                    {
                        printf("Request: %s\n", message);
                        sprintf(confirmationPipe, "%d", proId);
                        db = getDB(argv[4]);
                        buffer[0] = message;
                        printf("Buffer : %s\n", buffer[0]);
                        confirmationResponsePipe(db, buffer, confirmationPipe, argv[4]);
                    }
                }
                if (strlen(message) > 0)
                {
                    memset(message, 0, strlen(message));
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