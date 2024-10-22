#include "functioncs.c"




char server_port[100];
char server_address[100];
char root_directory[100];
msg messaggio;
char file_name[BUFFER_SIZE];
char dir[BUFFER_SIZE];

void check_arg_s(int argc, char* argv[])
{
    if(argc<7)
    {
        printf("argomenti mancanti\n");
        exit(-1);    
    }
    else
    {
        if(strcmp(argv[1],"-a")!=0 || strcmp(argv[3],"-p")!=0 || strcmp(argv[5],"-d")!=0 )
        {
            printf("Utilizzare formato: MyFTserver -a server_address -p server_port -d ft_root_directory\n");
            exit(-1);
        }
        else
        {
            
            strcpy(server_address,argv[2]);
            strcpy(server_port,argv[4]);
            strcpy(root_directory,argv[6]);
            
        }
    }
}


void handle_client(void *sockfd) 
{


    printf("vamos\n");
    int* new_socket = sockfd;
    
    printf("dale papi\n"); 
    // socket del client
    
    // Variabile utilizzata per salvare i messaggi ricevuti dal client
    char buffer[BUFFER_SIZE];
    int bytes_read;
    
    FILE* file;
    FILE* fp;
    if(messaggio.mode==0)
    {
        /*
        In path siamo dei coglioni perche' aprivamo il path che arrivava dal client, quindi per esempio se ciccio.txt esisteva nel client creavamo client(1).txt e aprivamo quello.
        Mentre se vai su read_mod alla riga 136/137 dopo che contavamo il file e creavamo ciccio(1).txt non lo salvavamo nella variabile path (cosa che ho aggiunto), che poi copiavi 
        in p che usavi nel client per aprire il file quindi nel client apriva ciccio.txt nel server ciccio(1).txt; ciccio(1).txt appena creato vuoto, mentre ciccio.txt veniva 
        sovrascritto con il contenuto di ciccio(1).txt che era vuoto, per questo ciccio.txt si ripuliva sempre.
        PORCO DIO
        */ 
        char* path = "root/ciccio.txt";
        file = fopen(path, "r");
        if(file == NULL){
            printf("File non aperto\n");
        }
        strcpy(buffer, "inizio file");
        while(send(new_socket, buffer, BUFFER_SIZE, 0) > 0){
            printf("dentro while\n");
            recv(new_socket,buffer,BUFFER_SIZE,0);
            if(fgets(buffer, BUFFER_SIZE, file) == NULL){
                printf("dentro if\n");
                memset(&buffer, 0, BUFFER_SIZE);
                strcpy(buffer, "end file");
                printf("copia effettuata\n");
                fclose(file);
                printf("file chiuso\n");
                break;
            }
        }
    }

    printf("ciclo fine\n");
    close(new_socket);
}

int main(int argc,char* argv[])
{
    int sock,portnu;
    struct sockaddr_in server,client;
    int length;
    int current_sock;
    int opt=1;
    
    msg messaggio;

    check_arg_s(argc,argv);
    portnu=atoi(server_port);
    if(check_directory(root_directory)==0) mkdir(root_directory,0755);

    sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        perror("socket error");
        exit(-1); 
    }

    server.sin_family= AF_INET;
    server.sin_port=htons(portnu);
    if(inet_pton(AF_INET, server_address, &server.sin_addr) <= 0) perror("Errore nella conversione dell'indirizzo IP");
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) perror("setsockopt");
    if(bind(sock,(struct sockaddr *)&server,sizeof(server))<0)
    {
        perror("bind error");
        exit(-1);
    }

    if(listen(sock,5)<0)
    {
        perror("listen error");
        exit(-1);
    }
    printf("Server attivo alla porta %d, attendo connessioni...\n", portnu);
    length=sizeof(client);
    pthread_t client_thread;
    while(1)
    {
        printf("DAJe lachi\n");
        current_sock=accept(sock,(struct sockaddr *)&client, (socklen_t *)&length);

        if(current_sock<0)
        {
            perror("accept error");
            exit(-1);
        }
        if(recv(current_sock,&messaggio,sizeof(messaggio),0)<0)
        {
            perror("read error");
            exit(-1);
        }
        printf("messaggio: %d,%s,%s",messaggio.mode,messaggio.path,messaggio.file_name);
        
        if (pthread_create(&client_thread, NULL, handle_client, (void*) current_sock) < 0) {
            perror("Thread creation error");
            close(current_sock);
            //free(client);
            
        }

        
        //pthread_detach(client_thread);
        /*int pid=fork();

        if(pid<0)
        {
            perror("fork error");
            exit(-1);        
        }
        if(pid==0)
        {
            if(read(current_sock,&messaggio,sizeof(messaggio))<0)
            {
                perror("read error");
                exit(-1);
            }
            printf("messaggio: %d,%s,%s",messaggio.mode,messaggio.path,messaggio.file_name);
            
            send(current_sock,"daje forte",BUFFER_SIZE,0);

            handle_client(current_sock,messaggio);

            close(current_sock);
            exit(0);

        }
        else if(pid>0)
        {
            printf("sono il pid porco de dio\n");
            close(current_sock);
        }*/
    }

    return 0;
}