#include<stdio.h>
#include <fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/wait.h>
#include<string.h>
#include<termios.h>
#include <sys/stat.h>
#include<sys/mman.h>
int main(int argc, char* argv[]){
    char* new_argv[argc+1];
    new_argv[0] = "cut";
    for(int i=1; i<argc; i++){
        new_argv[i] = argv[i];
    }
    new_argv[argc] = NULL;
    execvp("cut", new_argv);
    perror("exec failed");
    return 1;
}
