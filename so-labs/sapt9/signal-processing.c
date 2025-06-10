#include<stdio.h>
#include <fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/wait.h>
#include<string.h>
#include<termios.h>
#include <sys/stat.h>

int N;
int cnt = 1;
FILE* file;
int pid;
int exit_code = 0;
char* file_name;
void alarm_handler(int sig){
    if(cnt == 5) {
        printf("s-au scurs %d secunde", N*cnt);
        fclose(file);
        kill(pid, SIGUSR2);
        wait(0);
        printf("parinte : sfarsit executie\n");
        exit(1);
    }
    printf("s-au scurs %d secunde. scrie ceva la tastatura\n", N*cnt);
    cnt++;
    alarm(N);
}
void child_handler(int sig){
    printf("parinte terminat cu codul 0\n");
    exit(0);
}
void usr2_handle(int sig){
   printf("semnal usr2 receptionat de copil\n");

   FILE* f = fopen(file_name, "r");
   if(f == NULL){
       printf("eroare deschidere fisier in copil\n");
       exit(0);
   }

   char ch;
   while((ch = fgetc(f)) != EOF){
       putchar(ch);
   }

   fclose(f);
   exit(0);
}
int main(int argc, char* argv[]){
    if(argc != 3){
        printf("nu au fost primite doua argumente\n");
        exit(-1);
    }
    N = atoi(argv[2]);
    file_name = argv[1];
    signal(SIGALRM, alarm_handler);
    alarm(N);
    file = fopen(argv[1], "w+");
    pid = fork();
    if(pid == -1){
        printf("eroare fork\n");
        exit(-1);
    }
    if(pid == 0){
        //proces copil
        signal(SIGUSR2, usr2_handle);
        pause(); //wait for sigusr2
        exit(0);
    }
    else{
        //proces parinte
        signal(SIGCHLD, child_handler);
        char ch;
        while((ch = getchar()) != EOF){
            fwrite(&ch, 1, 1, file);
        }
        fclose(file);
        kill(pid, SIGUSR2);
        wait(0);
        printf("parinte : sfarsit executie\n");
        exit(0);
    }
}
