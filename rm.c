#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>

//utility function which concatenates two string with '/' as a separator
char *concat_parent_file(char *parent, char *file){

    char *util = NULL;
    util = (char *)malloc(PATH_MAX);
    if (util==NULL){
        //error handling
        perror("rm: malloc error");
        exit(EXIT_FAILURE);
    }

    strncpy(util, parent, strlen(parent));
    strcat(util, "/");
    strcat(util, file);

    return util;
}

//recursive function which given a directory removes all the files inside it.
//if there is a subdirectory inside the directory, it calls itself with the subdirectory as 
//the argument 
void del_dir(char *pathname){
    DIR *dp = opendir(pathname); //opens a directory stream
    if (dp==NULL){
        //error handling
        fprintf(stderr, "rm: opendir cannot open directory '%s':%s\n", pathname, strerror(errno));
        exit (EXIT_FAILURE);
    }

    struct dirent *d; struct stat sbf;
    d = readdir(dp); //returns pointer to the next entry in the directory stream
    while (d!=NULL){
        if (strcmp(d->d_name, ".")==0 || strcmp(d->d_name, "..")==0){
            /*handles the case for '.' and '..' as these directories are
             * just another reference for the current and parent directories respectively */
            d=readdir(dp);
            continue;
        }
        char *file_path = concat_parent_file(pathname, d->d_name); 
        if (stat(file_path, &sbf)==-1){
            //error handling
            fprintf(stderr, "rm: cannot stat '%s':%s\n", file_path, strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(sbf.st_mode)){ //handles the case when a subdirectory is found
            del_dir(file_path);
        }
        if (remove(file_path)==-1){
            //error handling
            fprintf(stderr, "rm: cannot remove '%s': %s\n", file_path, strerror(errno));
            exit(EXIT_FAILURE);
        }
        d=readdir(dp);
    }
}

int main(int argc, char *argv[]){
    
    struct stat sb;
    bool flag = 0; //turns on if -r flag is specified 

    if (argc<=1){
        //handles the case when no argument is specified to rm
        fprintf(stderr, "Usage: rm [OPTION] [FILE]...\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-r")==0){ //compares whether the arguments contains -r
        flag = 1;
    }

    if (argc<=1+flag){
        //error handling
        fprintf(stderr, "Usage: rm [OPTION] [FILE]...\n");
        exit(EXIT_FAILURE);
    }
    
    for (int i=1+flag; i<argc; i++){
        if (stat(argv[i], &sb)==-1){
            //error handling
            fprintf(stderr, "rm: cannot stat '%s': %s\n", argv[i], strerror(errno));
            continue;
        }
        if (S_ISDIR(sb.st_mode)){
            //handles the case when the file to be removed is a directory
            if (flag){
                del_dir(argv[i]);
            }
            else{
                //error handling
                fprintf(stderr, "rm: cannot remove '%s': Is a directory\n", argv[i]);
                continue;
            }
        }
        //removes the file or a directory which is empty
        if (remove(argv[i])==-1){
            //error handling
            fprintf(stderr, "rm: cannot remove '%s': %s\n", argv[i], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
