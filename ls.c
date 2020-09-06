#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

//to list out the contents of the given directory
void print_dir(char *pathname){

    struct stat sb; 
    if (stat(pathname, &sb)==-1){
        //error handling
        perror("ls: stat error"); 
        return;
    }
    
    DIR *dp=NULL;

    //check if the given file is a directory
    if (S_ISDIR(sb.st_mode)){ 

        dp = opendir(pathname); //opens a directory stream
        if (dp==NULL){
            //error handling
            fprintf(stderr, "ls: opendir cannot open dir: %s: %s\n", pathname, strerror(errno));
            return;
        }

        struct dirent *d = readdir(dp); //returns pointer to the next entry in the directory stream
        errno = 0; //stores the number of the last error

        while (d!=NULL){
            printf("%s\t", d->d_name);
            d = readdir(dp);
        }printf ("\n");

        if (errno!=0){
            //error handling
            fprintf(stderr, "ls: readdir error reading dir: %s: %s\n", pathname, strerror(errno));
            return;
        }
        closedir(dp);

    }
    else{ //handles the case when the given file is not a directory
        printf("%s\n", pathname); 
    }
}

int main (int argc, char *argv[])
{
    char *pathname = "."; //stores the path of the directory which needs to be listed

    if (argc==1){
        //to handle ls with no arguments: ls
        print_dir(pathname);
    }
    else if (argc==2){
        //to handle the case with one argument: ls DIRECTORY
        pathname = argv[1];
        print_dir(pathname);
    }
    else{
        //to handle the case with multiple arguments: ls DIRECTORY ...
        for (int i=1; i<argc; i++){
            if (i!=1) printf("\n");
            printf("%s:\n", argv[i]);
            pathname = argv[i];
            print_dir(pathname);
        }
    }

    return 0;
}
