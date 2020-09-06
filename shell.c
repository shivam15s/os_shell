#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <libgen.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>


int flag = 0; //turned on when a child process is running in the foreground 

//handles ctrl+c interrupt
//changes the default action of termination
void callback_handler(int sig){
    if (flag==1) printf("\n");
}

char *srcdirec = NULL;

//array for the userdefined binaries which need fork
char *userbin_f[] = {
    "ls", "cat", "grep", "pwd", "mkdir", "mv", "cp", "rm", "chmod"
}; int userbin_f_count = 9;

//array for the userdefined binaries which run in the parent terminal
char *userbin[] = {
    "exit", "cd"
}; int userbin_count = 2;


//handles the cd command
void cd(char **args){
    if (args[1]==NULL || args[2]!=NULL){
        //error handling
        fprintf(stderr, "Usage cd DIRECTORY\n"); 
        return;
    }
    //chdir system call
    if (chdir(args[1])!=0){ 
        //error handling
        fprintf(stderr, "cd: %s: %s\n", args[1], strerror(errno));
        return;
    }    
}

//utility function to concatenate a given string with the parent-terminal directory
char *concat_srcdirec(char *s1){
    char *final = NULL, *util = NULL;
    final = (char *)malloc(4000);
    util = (char *)malloc(100); 
    if (final == NULL){
        //error handling
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    final[0] = '\0';
    util[0] = '/'; util[1] = '\0'; 

    strcat(final, srcdirec);
    strcat(final, util);
    strcat(final, s1);

    return final;
}
    
//function to readline from the standard input
int MAXBUF_SIZE = 100; //stores the memory allocated to strings
char * read_line(){
    char *line = NULL;
    line = (char*)malloc(MAXBUF_SIZE);
    if (line==NULL){
        //error handling
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    int cur = 0;
    int buf_size = MAXBUF_SIZE;
    while (1){
        char c = getchar();
        if (c=='\n'){ 
            line[cur] = '\0';
            cur+=1;
            break;
        }
        line[cur] = c;
        cur+=1;
        if (cur>=buf_size){ //if the line buffer gets full we realloc
            buf_size+=MAXBUF_SIZE;
            line = (char *)realloc(line, buf_size);
            if (line==NULL){
                //error handling
                perror("malloc error");
                exit(EXIT_FAILURE);
            }
        }
    }

    return line;
}



int MAX_ARGS_COUNT = 10; //variable which stores the maximum number of arguments provided in stdin
char ** get_line_args(char * line){
    char DELIMETER = ' '; //whitespace is taken as the delimeter
    char **line_args;
    char *arg;
    line_args = (char**)malloc(MAX_ARGS_COUNT*sizeof(char*));
    if (line_args==NULL){
        //error handling
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    int cur = 0;
    int args_count = MAX_ARGS_COUNT;
    arg  = strtok(line, &DELIMETER); //strtok breaks the string using the given delimeter and returns 
    // the substrings in order from start to finish with each call of strtok
    while (arg!=NULL){
        line_args[cur] = arg;
        cur+=1;
        if (cur>=args_count){ //handles the case when the number of arguments exceed the MAX_ARGS_COUNT
            args_count += MAX_ARGS_COUNT;
            line_args = (char **)realloc(line_args, args_count);
            if (line_args==NULL){
                //error handling
                perror("malloc error");
                exit(EXIT_FAILURE);
            }
        }
        arg = strtok(NULL, &DELIMETER);
    } 
    line_args[cur]=NULL;
    return line_args; 
}

//function to return the current working directory
char *get_cwd(){
    char *cur_dir = NULL; 
    cur_dir = (char *)malloc(PATH_MAX);
    if (cur_dir==NULL){
        //error handling
        perror("getcwd: malloc error");
        exit(EXIT_FAILURE);
    }
    if (getcwd(cur_dir, PATH_MAX)==NULL){ //getcwd() returns the cwd
        //error handling
        perror("getcwd error");
        exit (EXIT_FAILURE);
    }
    return cur_dir;
}

//function to return the login name
char *get_login_name(){
    char *login_n = getlogin();
    if (login_n==NULL){
        //error handling
        perror("getlogin() error");
        exit(EXIT_FAILURE);
    }
    return login_n;
}

//function to return the host name
char *get_host_name(){
    char *host_n = NULL; 
    host_n = (char *)malloc(100*sizeof(char));
    if (host_n==NULL){
        //error handling
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    if (gethostname(host_n, 50)==-1){
        //error handling
        perror("gethostname() error");
        exit(EXIT_FAILURE);
    }
    return host_n;
}

//executes the command present in the args
int exec_command(char **args){

    //return 1 to continue and 0 to stop the shell
    
    if (args[0]==NULL){
        //handles the case when no input is given
        return 1; 
    }

    bool bg_flag = 0; //turned on when the & symbol is present
    int args_count = 0; //keeps track of the number of arguments in args

    while (args[args_count]!=NULL){
       args_count += 1;
    } 
    if (strcmp(args[args_count-1], "&")==0){ //check for the & symbol
        args[args_count-1]=NULL;
        bg_flag = 1;
    }

    int ub = 0; //set to 1 when the given command is one of the inbuilt binaries which 
    // need to run in the parent terminal
    for (int i=0; i<userbin_count; i++){
        if (strcmp(userbin[i], args[0])==0){
            ub = 1;
        }
    }

    if (ub==1){
        if (strcmp("exit", args[0])==0){
            return 0;
        }
        else if (strcmp("cd", args[0])==0){
            cd(args);
        } 
        return 1; 
    }



    flag = 1; //flag turned to 1 as child process going to come in foreground
    int rc = fork(); //fork
    if (rc<0){
        //error handling
        perror("fork error");
        exit (EXIT_FAILURE);
    }
    else if (rc==0){
        //child process
        int ubf = 0, ib = 1;
        //ubf: set to 1 when the given command is present in the user defined binaries which require fork
        //ib: set to 1 when the given command is not present in the user defined binaries 
       
        for (int i=0; i<userbin_f_count; i++){
            if (strcmp(userbin_f[i], args[0])==0){
                ubf = 1; ib = 0;
            }
        }
        if (ubf==1){
            /*concatenates the user defined binary after the 
            * path where it is stored */
            char *mod_bin = concat_srcdirec(args[0]); 
            args[0] = mod_bin; 
        }
        
        if (bg_flag){ //if the process needs to be run as the background process, we print its pid and close the stdin
            printf ("%d\n", getpid());
            fclose (stdin);
        }

        if (execvp(args[0], args)==-1){
            //error handling
            perror("exec error");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

    else{
        //parent process
        int wstatus;
        if (bg_flag == 0){ //if the child process was not a background process, we wait for it to get over
            int wc = waitpid(rc, &wstatus, WUNTRACED);

            if (wc==-1){
                //error handling
                perror("wait() error");
                exit(EXIT_FAILURE);
            }
        }

        flag = 0; //flag turned to 0 as control back to parent

    }
    return 1;
} 

//function which accepts user input and runs the corresponding command
//stops when the user inputs exit
void loop(){
    while (1){
        //gets the login, host and current directory to display in the terminal
        
        char *login_n = get_login_name();
        char *host_n = get_host_name();
        char *cur_dir = get_cwd();
        printf("%s@%s:%s$ ", login_n, host_n, cur_dir);
        //printf("%s:%s$ ", host_n, cur_dir);


        //gets the input from the stdin
        char *line = read_line();
        //breaks the input into array of strings using whitespace as the delimiter
        char **args = get_line_args(line);

        //executes the given command
        int status = exec_command(args);

        if (status == 0) break; //For current debugging 
    }
}

int main (int argc, char *argv[])
{   
    //sets the ctrl+c handler function
    signal(SIGINT, callback_handler);

    //gets the realpath where the file shell.c is present
    srcdirec = (char *)malloc(PATH_MAX);
    char *relpath = dirname(argv[0]);
    realpath(relpath, srcdirec);

    //shell function to accept the user input until the exit command is given
    loop(); 

    return 0;
} 


