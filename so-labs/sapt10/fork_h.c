#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include<stdlib.h>
int main() {
    //constructing the hierarchy of 1 parent and 4 children 
    int depth1 = 4, depth2 = 3;
    pid_t pid, pid2;
    for(int i=1; i<=depth1; i++){
        pid = fork();
        if(pid == -1){
            printf("Erorare la fork");
            exit(1);
        }
        if(pid == 0){
            for(int j = 1; j<=depth2; j++){
                pid_t grandchild = fork();
                if(grandchild == -1){
                    printf("eroare la grandkid\n");
                    exit(1);
                }
                if(grandchild == 0){
                    exit(0);
                }
                printf("gpid - %d, pid - %d\n", grandchild, getpid());
            }
            for(int k=0; k<depth2*4; k++){
                int sig = wait(0);
            }
            exit(0);
        }
        else{
            printf("\t\tpid - %d, ppid - %d\n", pid, getppid());
        }
        for(int k=0; k<depth1; k++){
            int sig = wait(0);
        }
    }

    return 0;
}
