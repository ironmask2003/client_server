#include "functioncs.c"


cl_req cl;
char p[BUFFER_SIZE];


void check_arg_c(int argc,char* argv[])
{
    if(argc<8)
    {
        printf("few arguments, retry\n");
        exit(-1 );
    }
    else
    {
        if(strcmp(argv[1],"-w")==0)
        {
           cl.write=1;
           cl.read=0;
           cl.list=0;
        }
        else if(strcmp(argv[1],"-r")==0)
        {
            cl.write=0;
            cl.read=1;
            cl.list=0;
        }
        else if(strcmp(argv[1],"-l")==0)
        {
            cl.write=0;
            cl.read=0;
            cl.list=1;
        }
        else
        {
            printf("format not supported\n");
            exit(-1);
        }

        if(strcmp(argv[2],"-a")!=0)
        {
            printf("daje roma\n");
            exit(-1);
        }
        else
        {
            cl.server_addr=argv[3];
        }

        if(strcmp(argv[4],"-p")!=0)
        {
            printf("daje roma 2\n");
            exit(-1);
        }
        else
        {
            
            cl.server_po=atoi(argv[5]);
        }
        //PROBLEMI
        if(strcmp(argv[6],"-f")!=0)
        {
            printf("daje roma 3\n");
            exit(-1);
        }
        else
        {
            printf("%s\n",argv[7]);
            if(cl.list!=1 && cl.read==1)
            {
                split_p(argv[7],0);
            }
            else if(cl.list!=1 && cl.write==1)
            {
                split_p(argv[7],1);
            }
            else
            {
                cl.remote_path=strdup(argv[7]);
            }         
        }

        if(argc>8)
        {
            if(strcmp(argv[8],"-o")!=0)
            {
                printf("formato error\n");
                exit(-1);
            }
            else
            {
                if(cl.read==1)
                {
                    split_p(argv[7],1);
                }
                else
                {
                    split_p(argv[7],0);
                }
            }
        }
        /*else
        {
            if(cl.read==1)
            {
                cl.remote_path=&cl.local_path;
                cl.remote_file_n=&cl.local_file_n;
            }
            else
            {
                cl.local_path=&cl.remote_path;
                cl.local_file_n=&cl.remote_file_n;
            }
        }*/
       if(cl.read==1)
       {
        read_mode();
        printf("%s\n",p);
       }
       
        
    }
}


void read_mode()
{
    char path[BUFFER_SIZE];
    sprintf(path, "%s%s", cl.remote_path, cl.remote_file_n);
    FILE* file;
    // Controlla se il percorso e la directory e esistono, se non esistono vengono create
    if(check_directory(cl.remote_path) == 1 && check_directory(path) == 1){
        printf("file duplicato\n");
        cl.remote_file_n = strdup(create_file(cl.remote_file_n, cl.remote_path));      // Nel caso in cui il percorso esiste controlla se esiste il file e nel caso ne crea un altro con lo stesso nome
        // Dato che il nome del file e' stato cambiato, risalvo in path il percorso corretto in cui salvare il contenuto che prendo dal file nel server
        sprintf(path, "%s%s", cl.remote_path, cl.remote_file_n);
    }
    else{
        char* path_temp = strdup(cl.remote_path);        // Copia il percorso
        char* temp = strtok(path_temp, "/");            // Eseuge lo split del '/'
        char* cp_path_temp = strdup(temp);              // Copia la prime parte del percorso
        // Controlla se e' arrivato alla fine del percorso
        while(temp != NULL){
            if(check_directory(cp_path_temp) == 0) {mkdir(cp_path_temp, 0755);}     // Controlla se la directory esiste, nel caso la crea
            temp = strtok(NULL, "/");       // Effettua di nuovo lo split per passare al pezzo dopo
            sprintf(cp_path_temp, "%s/%s", cp_path_temp, temp);     // Viene aggiunto alla variabile in cui alla fine sara' salvato il percorso completo
        }
        // Apre il file e lo chiude per effettuare la creazione del file fisico
        file = fopen(path, "w");
        if(file == NULL) error("File not found");
        fclose(file);
    }
    printf("read_mode: %s\n", path);
    strcpy(p,path);
}








void split_p(char* path_input, int type_path)
{
    char* f_n = &strrchr(path_input, '/')[1];
    printf(f_n);
    size_t path_l = f_n - path_input;
    printf("%d\n",path_l);
    char path_out[path_l];
    if((f_n != NULL) && strcmp(f_n, "") != 0){
        strncpy(path_out,path_input,path_l);        // Copia il percorso del file in results
        path_out[path_l] = '\0';                         // Aggiunge il terminatore, per indicare la fine della stringa
    } else {perror("Error, missing slash in the path or file name not specified (try to run again the command and after -o or -f specify a correct path)");}
    
    if (type_path == 0) 
    {
        cl.remote_path = strdup(path_out);
        cl.remote_file_n = strdup(f_n);
    }
    else 
    {
        cl.local_path = strdup(path_out);
        cl.local_file_n = strdup(f_n);
    }
} 






int main(int argc,char* argv[])
{
    int client_fd;
    struct sockaddr_in server_address;
    msg messaggio;
    

    check_arg_c(argc,argv);


    client_fd=socket(AF_INET,SOCK_STREAM,0);
    if(client_fd<0)
    {
        perror("socket error");
        exit(-1);
    }

    server_address.sin_family= AF_INET;
    server_address.sin_port=htons(cl.server_po);

    if(inet_pton(AF_INET, cl.server_addr, &server_address.sin_addr) <= 0){error("Error converting IP address");}


    connect(client_fd,(struct sockaddr *)&server_address,sizeof(server_address));
    if(connect<0)
    {
        perror("connection error");
        exit(-1);
    }
    printf("Connessione riuscita\n");
    if(cl.read==1)
    {
        messaggio.mode=0;
    }
    else if(cl.write==1)
    {
        messaggio.mode=1;
    }
    else
    {
        messaggio.mode=2;
    }
        
    strcpy(messaggio.path,cl.remote_path);
    strcpy(messaggio.file_name,cl.remote_file_n);
        

    printf("%d\n",messaggio.mode);
    

    if(send(client_fd,&messaggio,sizeof(messaggio),0)<0)
    {
        perror("send error");
        exit(-1);
    }
    printf("messaggio inviato");
    printf("%s\n", p);

    if(cl.read==1)
    {
        char buffer[BUFFER_SIZE];
        int n;
        FILE *file = fopen(p, "w");
        while ((n = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0) 
        {
            if(strcmp(buffer, "inizio file") == 0){ 
                send(client_fd, buffer, BUFFER_SIZE, 0);
                continue; 
            }
            if(strcmp(buffer, "end file") != 0){
                printf("contenuto file %s\n", buffer);
                fprintf(file, "%s", buffer);
            } else break;
            memset(buffer, 0, BUFFER_SIZE);
            strcpy(buffer, "prova");
            send(client_fd, buffer, BUFFER_SIZE, 0);
        }
    fclose(file);
    printf("finito\n");
    
    }

    close(client_fd);


    return 0;
}