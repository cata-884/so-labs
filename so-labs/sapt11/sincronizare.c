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
typedef struct {
    int ready;
} sh_t;
void citire1(char* fisier){
    FILE* fptr = fopen(fisier, "r+");
    int sh_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    ftruncate(sh_fd, sizeof(sh_t));
    sh_t* map = mmap(NULL, sizeof(sh_t), PROT_READ | PROT_WRITE, MAP_SHARED, sh_fd, 0);
    char buffer[100];
    while(fgets(buffer, 100, fptr) > 0){
        printf("%s \n", buffer);
        map->ready = 1;
        while(map->ready!=2){
            usleep(1000);
        }
    }
    map->ready = -1;
    close(sh_fd);
    fclose(fptr);
    munmap(map, sizeof(sh_t));
}
   void citire2(char* fisier){
       FILE* fptr = fopen(fisier, "r+");
       int sh_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
       ftruncate(sh_fd, sizeof(sh_t));
       sh_t* map = mmap(NULL, sizeof(sh_t), PROT_READ | PROT_WRITE, MAP_SHARED, sh_fd, 0);
       char buffer[100];
       while(fgets(buffer, 100, fptr) != NULL && map->ready!=-1){
           if(map->ready!=1){
               usleep(1000);
           }
           printf("%s \n", buffer);
           map->ready = 2;
       }
        close(sh_fd);
        fclose(fptr);
        munmap(map, sizeof(sh_t));

   }

int main(int argc, char* argv[]){
    //fisier1 = replici.txt, fisier2 = replici1.txt    
    char *fisier1 = argv[1], *fisier2 = argv[2];
     
    if(fork() == 0){
        citire2(fisier2); 
        exit(0);
    }
    citire1(fisier1);
    wait(0);
    shm_unlink(SHM_NAME);
    exit(0);
}
