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

w1w2_t* map;

int main(int argc, char* argv[]){
    //ai deja un shm_fd si o mapa anonima
    int fifo_fd = open(FIFO_NAME, O_RDWR);
    struct stat st;
    if(fstat(fifo_fd, &st) == -1){
        perror("eroare fstat");
        exit(1);
    }
    int i = 0;
    int n = st.st_size/sizeof(int);
    int numar;
    while(read(fifo_fd, &numar, sizeof(int)) > 0 && i<n-1){
        numar = numar*numar;
        map->patrat = numar;
        map->ready = 1;
        while(map->ready != 2){
            usleep(1000);
        }
    }
    if(read(fifo_fd, &numar, sizeof(int)) <0){
        perror("eroare read");
        exit(1);
    }
    map->contor_impare = numar;
    map->ready = -1;
    close(fifo_fd);
    return 0;
}
