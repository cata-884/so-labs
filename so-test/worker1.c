#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#define SHM_NAME "/worker_shm"
#define FIFO_NAME "/tmp/service_worker_fifo"
#define max 256
typedef struct{
    int nr;
    int contor;
} sh_t;
void citire(int fifo_fd){
    int numar;
    int contor = 0;
    while(read(0, &numar, sizeof(int)) > 0){
        if(numar%3!=0) {
            numar*=3;
            contor++;
        }
        if(write(fifo_fd, &numar, sizeof(int)) < 0 ){
            perror("eroare la scriere");
            exit(1);
        }
    }
    if(write(fifo_fd, &contor, sizeof(int)) < 0){
        perror("eroare la scriere");
        exit(1);
    }
}
int main(int argc, char* argv[]){
    //va primi info de la supervisor prin stdin
    unlink(FIFO_NAME);
    if(mkfifo(FIFO_NAME, 0666) == -1){
        perror("eroare creare fifo");
        exit(1);
    }
    int fifo_fd = open(FIFO_NAME, O_RDWR | O_CREAT, 0666);
    if(fifo_fd == -1){
        perror("eroare creare fifo fd");
        exit(1);
    }
    citire(fifo_fd);
    close(fifo_fd);
    unlink(FIFO_NAME);
    return 0;
}
