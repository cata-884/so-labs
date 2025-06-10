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
char* out = "fis_impar.txt";
int i = 1;
int fd, fd_out;
int bro_pid;
//citire para
void usr1_handler(int sig){
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
    if(i>st.st_size) exit(0);
    lseek(fd, i, SEEK_SET);
    char ch;
    read(fd, &ch, 1);
    fd_out = open(out, O_RDWR | O_CREAT, 0666);
    write(fd_out, &ch, 1);
    i+=2;
    kill(bro_pid, SIGUSR2);
    printf("semnal trimis catre bro 1\n");
}
int main(){
    printf("pid - %d\n", getpid());
    scanf("%d", &bro_pid);
    kill(bro_pid, SIGUSR2);
    signal(SIGUSR1, usr1_handler);
    pause();
    close(fd);
    close(fd_out);
}
