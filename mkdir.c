#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]){
    if (argc==1){
        //handles the case when mkdir is provided no arguments
        //error handling
        fprintf(stderr, "Usage: mkdir DIRECTORY...\n");
        exit (EXIT_FAILURE);
    } 

    else{
        for (int i=1; i<argc; i++){
            if (mkdir(argv[i], 0777)==-1){ 
                //mkdir system call creates a new directory given the permission mode 
                //error handling
                fprintf (stderr, "mkdir: cannot create directory '%s': %s\n", argv[i], strerror(errno));
                continue;
            }
        }
    }

    return 0;
}
