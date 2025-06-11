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
typedef struct{
    int patrat;
    int ready;
    int contor_impare;
} w1w2_t;
int main(int argc, char* argv[]){
    int shm_fd = open(SHM_NAME, O_RDWR);
    if(shm_fd == -1){
        perror("eroare shm_fd");
        exit(1);
    }
    sh_t* map = mmap(NULL, sizeof(w1w2_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);
    if(map == MAP_FAILED){
        perror("eroare la mapare");
        exit(1);
    }
    int pid = fork();
    if(pid == -1){
        perror("eroare la fork");
    }
    else if(pid == 0){
        execlp("./worker3", "worker3", NULL);
        perror("eroare la execlp");
        close(shm_fd);
        munmap(map, sizeof(sh_t));
        exit(1);
    }
    execlp("./worker2", "worker2", NULL);
    perror("eroare la execlp");
    close(shm_fd);
    munmap(map, sizeof(sh_t));
    exit(1);
}
