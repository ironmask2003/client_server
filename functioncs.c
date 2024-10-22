#include "cliserv.h"



int check_directory(char* path)
{
    struct stat stat_path;

    if(stat(path,&stat_path)!=0) return 0;

    return 1;
}

int check_duplicate(char* path, char* file_name)
{
    struct dirent *entry;
    DIR *folder;

    int cont=0;

    if((folder=opendir(path)) == NULL)
    {
        perror("opendir error\n");
        return -1;
    }

    while ((entry= readdir(folder))!= NULL)
    {
        if(strcmp(entry->d_name, file_name)==0) cont++;
    }
    closedir(folder);
    return cont;
}

char* create_file(char* file_n, char* file_path)
{
    char path_copy[BUFFER_SIZE];
    int cont=0;
    char* name_copy= strdup(file_n);



    while(1)
    {
        char* old_file_n=strdup(file_n);
        if(check_duplicate(file_path,name_copy)==0) break;
        cont+=check_duplicate(file_path,name_copy);

        char* p =strtok(old_file_n,".");
        if(p!=NULL)
        {
            sprintf(name_copy,"%s(%d)",p,cont);
            p=strtok(NULL,"");
            if(p!=NULL)
            {
                sprintf(name_copy,"%s.%s",name_copy,p);
            }
        }
        free(old_file_n);
       
    }

    file_n=strdup(name_copy);
    sprintf(path_copy,"%s%s",file_path,file_n);
    FILE* file= fopen(path_copy,"w");
    if(file==NULL)
    {
        perror("fopen error\n");   
    }
    fclose(file);
    return file_n;
}