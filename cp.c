#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h> 
#include <stdbool.h>
#include <dirent.h>
#include <libgen.h>

//utility function to concatenate the two given strings with a '/' as a separator
char *concat_src_target(char *src, char *dir){
    char *util = (char *)malloc(PATH_MAX);
    if (util==NULL){
        //error handling
        perror("cp: malloc error");
        exit (EXIT_FAILURE);
    }

    strncpy(util, dir, strlen(dir));
    strcat(util, "/");
    strcat(util, src); 
    return util;
}

//utility function which copies the contents in the source file to the destination file
void copy_files(char *src, char *dst){
    FILE *fsrc, *fdst;

    //opens the files present at the path given by the strings src and dst
    fsrc = fopen(src, "r");
    fdst = fopen(dst, "w");

    if (fsrc==NULL){
        //error handling
        fprintf(stderr, "fopen can't open %s: %s\n", src, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (fdst==NULL){
        //error handling
        fprintf(stderr, "fopen can't open %s: %s\n", dst, strerror(errno));
        exit(EXIT_FAILURE);
    }

    //declares variables for line buffer and storing the line buffer size
    char *line = NULL; size_t line_buf_size = 0;
    while (getline(&line, &line_buf_size, fsrc)!=-1){
        fprintf(fdst, "%s", line);
    }
}

int main(int argc, char *argv[]){
   
     
    struct stat sb1, sb2, sb3; 

    if (argc<=2){
        //handles the case when less than cp has less than 2 arguments
        fprintf(stderr, "Usage: cp [OPTION] SOURCE... DIRECTORY\n       cp [OPTION] SOURCE DESTINATION\n");
        exit (EXIT_FAILURE);
    }

    bool flag = 0;
    if (strcmp(argv[1], "-r")==0){ //sets flag 1 if -r flag is given
        flag = 1;
    }

    if (stat(argv[argc-1], &sb1)!=-1 && S_ISDIR(sb1.st_mode)){ 
        /*handles the case when the target is an existing directory */

        for (int i=1+flag; i<argc-1; i++){
            if (stat(argv[i], &sb2)==-1){
                //error handling
                fprintf(stderr, "cp: cannot stat '%s': %s", argv[i], strerror(errno));
                exit (EXIT_FAILURE);
            }
        }

        for (int i=1+flag; i<argc-1; i++){

            if (stat(argv[i], &sb2)==-1){
                //error handling
                fprintf(stderr, "cp: cannot stat '%s': %s\n", argv[i], strerror(errno));
                exit(EXIT_FAILURE);
            }

            if (S_ISREG(sb2.st_mode)){
                //handles the case when the source file is a regular file
                char *newsrc = basename(argv[i]); 
                char *newtarget = concat_src_target(newsrc, argv[argc-1]);
                copy_files(argv[i], newtarget);
            }
            else if (S_ISDIR(sb2.st_mode)){
                //handles the case when the source file is a directory
                if (flag){
                    DIR *dp = opendir(argv[i]); //opens a  directory stream
                    if (dp==NULL){
                        fprintf(stderr, "cp: opendir cannot open directory '%s'; omitting directory\n", argv[i]);
                        continue;
                    }
                    char *newdir = (char *)malloc(PATH_MAX);
                    char *base_src = basename(argv[i]);

                    newdir = concat_src_target(base_src, argv[argc-1]);

                    if (mkdir (newdir, 07777)==-1){ //creates a directory at the path given by the newdir
                        //error handling
                        perror("mkdir error");
                        continue;
                    }
                    struct dirent *d; 
                    d = readdir(dp); //returns pointer to the next entry in the directory stream

                    while (d!=NULL){

                        char *pathname = (char *)malloc(sizeof(PATH_MAX));
                        pathname = concat_src_target(d->d_name, base_src);

                        if (stat(pathname, &sb3)==-1){
                            //error handling
                            perror("cp: stat error");
                            exit(EXIT_FAILURE);
                        }
                        if (S_ISREG(sb3.st_mode)){
                            /*as it's only required to copy the regular files in the directory,
                             * we check if the file in the directory is a regular file before copying it
                             * to the target directory */

                            char *newtarget = concat_src_target(pathname, argv[argc-1]);
                            copy_files(pathname, newtarget);
                        }     
                        d = readdir(dp);
                    }
                }
                else{
                    //error hanling, trying to copy directories without -r flag
                    fprintf(stderr, "cp: -r not specified; omitting directory '%s'\n", argv[i]);
                }
            }
            else{
                //error handling for when the source file is not a regular file nor a directory
                fprintf(stderr, "cp: %s neither a regular file nor a directory\n", argv[i]);
            }
        }
    }
    else{
        //handles the case when the target is a regular file or a non existing directory
        if (argc>3+flag){
            //error handling
            fprintf(stderr, "cp: target '%s' is not a directory\n", argv[argc-1]);
            exit(EXIT_FAILURE);
        }
        struct  stat sbfile;
        if (stat(argv[1+flag], &sbfile)==-1){
            //error handling
            perror("cp: stat error");
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(sbfile.st_mode)){
            //error handling
            fprintf(stderr, "cp: target '%s' is not a directory\n", argv[argc-1]);
            exit(EXIT_FAILURE);
        }                
        //copies given regular file to the destination file
        copy_files(argv[1+flag], argv[2+flag]);
    }

    return 0;
}

