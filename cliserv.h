// Dichiarazione delle librerie utilizzate
#include <stdio.h>
#include <sys/socket.h>         // Usato per la comunicazione con il server
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>         
#include <string.h>            // Usato per lavorare con le stringhe
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdbool.h>            // Usato per usare i booleani
#include <unistd.h>
#include <pthread.h>            // Usato per istanziare Thread
#include <dirent.h> 


#define BUFFER_SIZE 1024
#define MAX_LEN 100

typedef struct client_req
{
    char* server_addr;
    int server_po;
    
    char* local_file_n;
    char* local_path;
    
    char* remote_path;
    char* remote_file_n;

    int read;
    int write;
    int list;
}cl_req;

typedef struct MSG
{
    int mode;
    char path[MAX_LEN];
    char file_name[MAX_LEN];
}msg;
