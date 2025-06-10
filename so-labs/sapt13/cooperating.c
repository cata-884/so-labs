#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#define file "input-data.txt"
#define out "statistics.txt"
#define fifo "/tmp/generic-pipe"
#define max 4096

int main() {
    int pipe1[2], pipe2[2];
    int pid1, pid2;
    
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(1);
    }
    
    mkfifo(fifo, 0666);
    
    pid1 = fork();
    if (pid1 == 0) {
        close(pipe1[1]); 
        close(pipe2[0]); 
        close(pipe2[1]);
        
        char buffer1[max];
        int bytes_read = read(pipe1[0], buffer1, max);
        close(pipe1[0]);
        
        if (bytes_read == -1) {
            perror("read");
            exit(1);
        }
        
        char buffer2[max];
        int k = 0;
        for (int i = 0; i < bytes_read; i++) {
            if (buffer1[i] >= '0' && buffer1[i] <= '9') {
                buffer2[k++] = buffer1[i];
            } else if (buffer1[i] >= 'A' && buffer1[i] <= 'F') {
                buffer2[k++] = buffer1[i] - 'A' + 'a';
            } else if (buffer1[i] >= 'a' && buffer1[i] <= 'f') {
                buffer2[k++] = buffer1[i];
            }
        }
        buffer2[k] = '\0';
        
        int fd_fifo = open(fifo, O_WRONLY);
        if (fd_fifo == -1) {
            perror("open FIFO");
            exit(1);
        }
        write(fd_fifo, buffer2, k);
        close(fd_fifo);
        
        exit(0);
    }
    
    pid2 = fork();
    if (pid2 == 0) {
        close(pipe1[0]); 
        close(pipe1[1]);
        close(pipe2[0]);
        
        int fd_fifo = open(fifo, O_RDONLY);
        if (fd_fifo == -1) {
            perror("open FIFO");
            exit(1);
        }
        
        char buffer[max];
        int bytes_read = read(fd_fifo, buffer, max);
        close(fd_fifo);
        
        if (bytes_read == -1) {
            perror("read");
            exit(1);
        }
        
        int counts[16] = {0}; 
        for (int i = 0; i < bytes_read; i++) {
            char c = buffer[i];
            if (c >= '0' && c <= '9') {
                counts[c - '0']++;
            } else if (c >= 'a' && c <= 'f') {
                counts[c - 'a' + 10]++;
            }
        }
        
        FILE* fptr = fopen(out, "w");
        if (!fptr) {
            perror("fopen");
            exit(1);
        }
        
        int distinct = 0;
        for (int i = 0; i < 16; i++) {
            if (counts[i] > 0) {
                distinct++;
                if (i < 10) {
                    fprintf(fptr, "%c: %d\n", '0' + i, counts[i]);
                } else {
                    fprintf(fptr, "%c: %d\n", 'a' + i - 10, counts[i]);
                }
            }
        }
        fclose(fptr);
        
        write(pipe2[1], &distinct, sizeof(int));
        close(pipe2[1]);
        
        exit(0);
    }
    
    close(pipe1[0]);
    close(pipe2[1]); 
    
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("open input file");
        exit(1);
    }
    
    char buffer[max];
    int bytes_read = read(fd, buffer, max);
    close(fd);
    
    if (bytes_read == -1) {
        perror("read");
        exit(1);
    }
    
    write(pipe1[1], buffer, bytes_read);
    close(pipe1[1]);
    
    int smth;
    read(pipe2[0], &smth, sizeof(int));
    close(pipe2[0]);
    
    printf("%d \n", smth);
    
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    
    unlink(fifo);

    return 0;
}
