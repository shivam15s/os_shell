#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
    if (argc<=2){
        //handles the case when there chmod is given less than 2 arguments
        //error handling
        fprintf(stderr, "Usage: chmod MODE FILE...\n");
        exit(EXIT_FAILURE);
    }
    //converts given string into its octal representation
    int mode = strtol(argv[1], NULL, 8); 

    for (int i=2; i<argc; i++){
        if (chmod(argv[i], mode)==-1){
            //error handling
            fprintf(stderr, "chmod: can't change permission for '%s': %s\n", argv[i], strerror(errno));
            continue;
        }
    }

    return 0;
}
