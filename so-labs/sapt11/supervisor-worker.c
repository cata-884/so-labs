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
#include<time.h>
#define BINARY "operanzi.bin"
#define OP "operatori.txt"
int pid;
void binary_init(int n){
    FILE* fptr = fopen(BINARY, "wb+");
    if(fptr == NULL){
        perror("eroare fisier");
        exit(1);
    }
    srand(time(0));
    int *list = (int*)malloc(sizeof(int)*n);
    for(int i=0; i<n; i++){
        list[i] = rand()%10000;
    }
    fwrite(list, sizeof(int), n, fptr);
    fclose(fptr);
    free(list);
}
void op_init(int n){
    srand(time(0));
    int fd = open(OP, O_RDWR | O_CREAT, 0666);
    if(fd == -1){
        perror("eroare creare ");
        exit(1);
    }
    ftruncate(fd, n);
    char* map = mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    char* list = "+-*/";
    for(int i=0; i<n; i++){
        map[i] = list[rand()%4];
    }
    if(msync(map, n, MS_SYNC) == -1){
        perror("eroare sync");
        exit(1);
    }
    close(fd);
    munmap(map, n);
}
int main(int argc, char* argv[]){
    if(argc!=2) {
        perror("eroare argc");
        exit(1);
    }
    int n = atoi(argv[1]);
    pid = fork();
    if(pid == 0){
        //copil1
        sleep(1);
        binary_init(n);
        exit(0);
    }
    else{
        //parinte
        pid = fork();
        if(pid == 0){
            //copil2
            op_init(n-1);
            exit(0);
        }
        else{
            wait(0);
            wait(0);
            FILE* fptr = fopen(BINARY, "rb+");
            if(fptr == NULL) {
                perror("eroare");
                exit(1);
            }
            int fd = open(OP, O_RDWR);
            if(fd == -1){
                perror("eroare");
                exit(1);
            }
            int* numbers = (int*)malloc(sizeof(int)*n);
            fread(numbers, sizeof(int), n, fptr);
            char* map = mmap(NULL, n-1, PROT_READ, MAP_SHARED, fd, 0);
            for(int i=0; i<n-1; i++){
                char op = map[i];
                int num1 = numbers[i], num2 = numbers[i+1];
                int res;
                if(op == '+') res = num1+num2;
                if(op == '-') res = num1-num2;
                if(op == '*') res = num1*num2;
                if(op == '/') res = num1/num2;
                printf("%d %c %d = %d\n", num1, op, num2, res);   
            }
            free(numbers);
            close(fd);
            fclose(fptr);
            munmap(map, n);

        }
    }
}
