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
int shm_fd;

int main(int argc, char* argv[]){
    sh_t* map2 = mmap(NULL, sizeof(sh_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(map == MAP_FAILED){
        perror("eroare la mapare");
        exit(1);
    }
    int suma = 0;
    while(map->ready!=-1){
        while(map->ready!=1){
            usleep(1000);
        }
        suma+=map->patrat;
        map->ready = 2;
    }
    map2->nr = suma;
    map2->contor = map->contor_impare;
    munmap(map2, sizeof(sh_t));
    return 0;
}
