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
int fd, fd_out;
struct stat st;
char *map, *map_out;
int stoi_nr(char* str){
    int i=0;
    while(str[i]!='\0') i++;
    int n = atoi(str);
    int k = 0;
    while(n) {
        k++;
        n/=10;
    }
    return (k == i) ? (atoi(str)) : -1;
}
void head_c(char* str, int nr, int multi){
    if(multi) printf(" ==> %s <== \n", str);
    fd = open(str, O_RDONLY);
    if(fd == -1) {perror("eroare"); exit(1);}
    if(fstat(fd, &st) == -1) {perror("eroare"); exit(1);}
    if(st.st_size<nr) nr = st.st_size;
    map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(map == MAP_FAILED) {perror("eroare"); exit(1);}
    for(int i=0; i<nr; i++){
        char ch = map[i];
        putchar(ch);
    }
    munmap(map, st.st_size);
    close(fd);
    printf("\n");
}
void head_n(char* str, int nr, int multi){
    if(multi) printf(" ==> %s <== \n", str);
    fd = open(str, O_RDONLY);
    if(fd == -1) {perror("eroare"); exit(1);}
    if(fstat(fd, &st) == -1) {perror("eroare"); exit(1);}
    map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(map == MAP_FAILED) {perror("eroare"); exit(1);}
    int cnt = 1;
    for(int i=0; i<st.st_size; i++){
        if(cnt == nr+1) return ; 
        if(map[i] == '\n') cnt++;
        char ch = map[i];
        putchar(ch);
    }
    munmap(map, st.st_size);
    close(fd);
    putchar('\n');
}
int main(int argc, char* argv[]){
    int mode = 0; //0 pentru n, 1 pentru c
    int nr = 5;
    int cnt = 1;
    int multi = 0;
    for(int i=1; i<argc; i++){
        if(strcmp(argv[i], "-n") == 0){
            mode = 0;
        }
        else if(strcmp(argv[i], "-c") == 0){
            mode = 1;
        }
        else if(stoi_nr(argv[i]) != -1){
            nr = stoi_nr(argv[i]);
        }
        else cnt++;
    }
    if(cnt > 1) multi = 1;
    for(int i=1; i<argc; i++){
        if(strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "-n") == 0 || stoi_nr(argv[i]) != -1) continue;
        if(mode) head_c(argv[i], nr, multi);
        else head_n(argv[i], nr, multi);
    }
}

