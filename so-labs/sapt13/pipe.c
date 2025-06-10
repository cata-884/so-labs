#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int pipe1[2], pipe2[2], pipe3[2];
    pid_t pid1, pid2, pid3, pid4;
    
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if ((pid1 = fork()) == 0) {
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        
        execlp("ps", "ps", "-eo", "user,comm,pid", "--no-headers", NULL);
        perror("execlp ps");
        exit(EXIT_FAILURE);
    }
    
    if ((pid2 = fork()) == 0) {
        dup2(pipe1[0], STDIN_FILENO);  
        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        
        execlp("tr", "tr", "-s", " ", NULL);
        perror("execlp tr");
        exit(EXIT_FAILURE);
    }
    
    close(pipe1[0]);
    close(pipe1[1]);
    
    if ((pid3 = fork()) == 0) {
        dup2(pipe2[0], STDIN_FILENO);  
        dup2(pipe3[1], STDOUT_FILENO); 
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe3[0]);
        close(pipe3[1]);
        
        execlp("cut", "cut", "-d", " ", "-f", "1,2,3", "--output-delimiter=:", NULL);
        perror("execlp cut");
        exit(EXIT_FAILURE);
    }
    
    close(pipe2[0]);
    close(pipe2[1]);
    
    if ((pid4 = fork()) == 0) {
        dup2(pipe3[0], STDIN_FILENO); 
        close(pipe3[0]);
        close(pipe3[1]);
        
        execlp("sort", "sort", "-k2", "-t:", NULL);
        perror("execlp sort");
        exit(EXIT_FAILURE);
    }
    
    close(pipe3[0]);
    close(pipe3[1]);
    
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);
    waitpid(pid4, NULL, 0);
    
    return 0;
}
