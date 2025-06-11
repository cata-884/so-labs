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
#define max 256
typedef struct{
    int nr;
    int contor;
} sh_t;
int s_cifre(int n){
    int s = 0;
    while(n){
        s+=n%10;
        n/=10;
    }
    return s;
}
void scriere(char* cale, int pipe_fd){
    FILE* fptr = fopen(cale, "r+");
    if(!fptr){
        perror("eroare la deschidere fisierului");
        exit(1);
    }
    int numar;
    while(fscanf(fptr, "%d", &numar) > 0){
        if(write(pipe_fd, &numar, sizeof(int))<=0) {
            perror("eroare la scriere");
            exit(1);
        }
    }
    fclose(fptr);
}
void citire(int shm_fd){

    sh_t* map = mmap(NULL, sizeof(sh_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(map == MAP_FAILED){
        perror("eroare la crearea maparii");
        exit(1);
    }
    int sum = map->nr + map->contor;
    munmap(map, sizeof(sh_t));
    printf("%d \n", s_cifre(sum));
}
int main(int argc, char* argv[]){
    if(argc==2 && access(argv[1], R_OK)) {
        printf("nu e primit un argument sau nu e fisier citibil\n");
        exit(1);
    }
    int shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    if(shm_fd == -1){
        perror("eroare shm_fd");
        exit(1);
    }
    if(ftruncate(shm_fd, sizeof(sh_t)) == -1){
        perror("eroare la ftruncate");
        exit(1);
    }
    int pipe_fd[2];
    if(pipe(pipe_fd) == -1){
        printf("eroare la pipe\n");
        exit(1);
    }
    char* cale = argv[1];
    scriere(cale, pipe_fd[1]);
    if(fork() == 0){
        close(pipe_fd[1]);
        dup2(pipe_fd[0], 0); //vom scrie in pipe_fd[1]
        execlp("./worker1", "worker1", NULL);
        perror("eroare la exceclp");
        exit(1);
    }
    wait(0);
    citire(shm_fd);
    close(shm_fd);
    shm_unlink(SHM_NAME);
    return 0;
}
