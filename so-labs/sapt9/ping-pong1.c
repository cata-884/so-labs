#include<stdio.h>
#include <fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/wait.h>
#include<string.h>
#include<termios.h>
#include <sys/stat.h>
char *file = "fis.txt";
char* out = "fis_par.txt";
int i = 0;
int fd, fd_out;
int bro_pid;
//citire para
void usr2_handler(int sig){
    fd = open(file, O_RDWR);
    if(fd == -1) {
        printf("eroare fd");
        exit(1);
    }
    struct stat st;
    if(fstat(fd, &st) == -1){
        printf("eroare lstat\n");
        exit(1);
    }
    lseek(fd, i, SEEK_SET);
    char ch;
    if(read(fd, &ch, 1)<=0){
        close(fd);
        exit(0);
    }
    close(fd);
    fd_out = open(out, O_WRONLY | O_CREAT | O_APPEND, 0666);
    write(fd_out, &ch, 1);
    close(fd_out);
    i+=2;
    if(i<st.st_size){
        kill(bro_pid, SIGUSR1);
        printf("semnal trimis catre proces 2");
    }
    else {
        kill(bro_pid, SIGUSR1);
        exit(0);
    }
}
int main(){
    printf("pid - %d\n", getpid());
    scanf("%d", &bro_pid);
    signal(SIGUSR2, usr2_handler);
    kill(getpid(), SIGUSR2);
    while(1) pause();
    close(fd);
    close(fd_out);
}
