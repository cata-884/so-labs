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
    int fd_in = open(argv[1], O_RDONLY);
    int fd_out = open(argv[2], O_RDWR | O_CREAT, 0666);
    if(fd_in == -1 || fd_out == -1){
        printf("eroare fd\n");
        exit(1);
    }
    struct stat st;
    if(fstat(fd_in, &st) == -1){
        printf("eroare st\n");
        exit(1);
    }
    if(ftruncate(fd_out, st.st_size) == -1){
        printf("eroare truncate\n");
        exit(1);
    }
    char *map_in = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd_in, 0);
    char *map_out = mmap(NULL, st.st_size, PROT_WRITE, MAP_SHARED, fd_out, 0);
    if(map_in == MAP_FAILED || map_out == MAP_FAILED){
        perror("map failed\n");
        exit(1);
    }
    for(int i=0; i< st.st_size; i++){
        map_out[i] = (map_in[i] == argv[3][0]) ? (argv[4][0]) : map_in[i];
    }
    msync(map_out, st.st_size, MS_SYNC);
    munmap(map_in, st.st_size);
    munmap(map_out, st.st_size);
    close(fd_in);
    close(fd_out);
    exit(0);
}

