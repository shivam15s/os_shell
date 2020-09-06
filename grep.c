#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>


int main(int argc, char *argv[]){
    
    if (argc==1){
        //to handle the case when no argument is given
        fprintf(stderr, "Usage: grep searchterm [file ...]\n");
        exit (EXIT_FAILURE);
    }

    if (strcmp(argv[1], "\"\"")==0 || strcmp(argv[1],"\'\'")==0){
        /* when the search string is """" or "''", we explicitly need to define the 
         * search string as "" */
        char *str_m = "";
        argv[1] = str_m;      
    }    

    if (argc==2){
        //to handle the case when File is not given
        //take input from stdin

        /*variables for storing the input line and line_buf_size which
         * stores the memory allocated to line */ 
        char *line = NULL; size_t line_buf_size = 0;
        while (1){
            if (getline(&line, &line_buf_size, stdin)!=-1){
                //we find if the line contains the searchterm
                if (strstr(line, argv[1])!=NULL){
                    //if match if found ie strstr return a ptr, we print the line
                    printf("%s", line);
                } 
            }
        }
    }

    else{
        //to handle the case when FILE is given
        for (int i=2; i<argc; i++){
            //opens a file at the location given by argv[i] in read only mode
            FILE *ptr = fopen(argv[i], "r");   
            char *line = NULL; size_t line_buf_size = 0;

            if (ptr==NULL){
                //error handling
                fprintf(stderr, "grep: %s: cannot open file: %s\n", argv[i], strerror(errno)); 
                exit(EXIT_FAILURE);
            }

            //reads each line in the file and prints the line if a match is found
            while (getline(&line, &line_buf_size, ptr)!=-1){
                if (strstr(line, argv[1])!=NULL){
                    if (argc>3)
                        printf("%s:%s", argv[i], line);
                    else
                        printf("%s", line);
                }
            }

            free (line);
            line_buf_size = 0;
            fclose(ptr);
        }
    }

    return 0;
}
