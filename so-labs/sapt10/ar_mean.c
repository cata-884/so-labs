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
    //trebuie sa fie fisier binar
    int fd = open(argv[1], O_RDONLY);
    if(fd == -1){
        perror("eroare fd");
        exit(1);
    }
    struct stat st;
    if(fstat(fd, &st) == -1){
        perror("eroare stat");
        exit(1);
    }
    printf("%d \n", st.st_size);
    if(st.st_size % sizeof(int) != 0){
        close(fd);
        exit(1);
    }
    int *map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    for(int i=0; i<st.st_size/sizeof(int); i++){
        printf("%d ", map[i]);
    }
    putchar('\n');
    munmap(map, st.st_size);
    close(fd);
}
