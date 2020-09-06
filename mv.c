#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>

//utility function which concatenates given string with '/' as a separator
char *concat_src_target(char *src, char *dir){
    char *util = (char *)malloc(PATH_MAX);

    if (util==NULL){
        //error handling
        perror("mv: malloc error");
        exit (EXIT_FAILURE);
    }

    strncpy(util, dir, strlen(dir));
    strcat(util, "/");
    strcat(util, src);

    return util;
}

int main (int argc, char *argv[]){

    struct stat sb1, sb2;
    if (argc<=2){
        //handles the case when mv has less than 2 arguments
        fprintf(stderr, "Usage: mv SRC... TARGET\n");
        exit(EXIT_FAILURE);
    }

    if (stat(argv[argc-1], &sb1)!=-1 && S_ISDIR(sb1.st_mode)){ 
        /*handles the case when the target is an existing directory */

        for (int i=1; i<argc-1; i++){
            if (stat(argv[i], &sb2)==-1){
                //error handling
                fprintf (stderr, "mv: %s: fopen error: %s\n", argv[i], strerror(errno));
                continue;
            }

            if (S_ISREG(sb2.st_mode)){ //handles the case when the source is regular file
                char *newsrc = basename(argv[i]);
                char *newtarget = concat_src_target(newsrc, argv[argc-1]);
                //rename system call allows to link the file to a new location and then unlinks it
                //at the original location
                if (rename(argv[i], newtarget)!=0){
                    //error handling
                    perror("mv: rename error");
                }
            }
            else {
                //error handling
                fprintf(stderr, "mv: source '%s' not a regular file\n", argv[i]);
                continue;
            }

        }
    }
    else{
        if (argc>3){
            //error handling
            fprintf(stderr, "mv: target '%s' is not a directory\n", argv[argc-1]); 
            exit (EXIT_FAILURE);
        }
        //handles the case when the target is a file or a non existing directory
        if (rename(argv[1], argv[2])!=0){
            perror("mv: rename error");
        }
    }
    
    return 0;
}
