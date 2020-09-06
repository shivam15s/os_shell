#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]){

    if (argc==1){
        //handles the case when no argument is provided
        exit(0);
    }

    for (int i=1; i<argc; i++){
        //opens the file present at the path given by argv[i] and associates a stream with it
        FILE *ptr = fopen(argv[i], "r");
        if (ptr == NULL){
            //error handling
            fprintf (stderr, "cat: %s: cannot open file: %s\n", argv[i], strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (argc>2){
            //when multiple files are given, we specify the filename before printing the contents
            printf("%s:\n", argv[i]);
        }

        //we declare a line buffer and a variable which stores the memory
        //allocated to the line buffer
        char *line = NULL;
        size_t line_buf_size = 0;

        while ((getline(&line, &line_buf_size, ptr))!=-1){
        //prints the contents in the file
            printf("%s", line);
        }

    }
    return 0;
}
