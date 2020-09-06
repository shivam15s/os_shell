#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

//function to get the current working directory
char *get_cwd(){

    char *cur_dir = NULL; 
    cur_dir = (char *)malloc(PATH_MAX);

    if (cur_dir==NULL){
        //error handling
        perror("pwd: malloc error");
        exit(EXIT_FAILURE);
    }
    //getcwd system call
    if (getcwd(cur_dir, PATH_MAX)==NULL){
        //error handling
        perror("pwd: getcwd() error");
        exit (EXIT_FAILURE);
    }

    return cur_dir;
}

int main(int argc, char *argv[]){
    if (argc>1){
        //handles the case when pwd is provided arguments
        //error handling
        fprintf(stderr, "pwd: too many arguments\n");
        exit(EXIT_FAILURE);
    }
    else{
        char *cwd_u = get_cwd();
        printf("%s\n", cwd_u); //prints the cwd
    }

    return 0;
}
