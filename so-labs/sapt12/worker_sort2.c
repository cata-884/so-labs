#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define SHM_NAME "/sort_shm"

typedef struct {
    int start_line;
    int end_line;
    int worker_id;
    char input_file[256];
    char output_file[256];
} Task;

void extract_lines(const char *input_file, const char *output_file, int start_line, int end_line) {
    FILE *in = fopen(input_file, "r");
    if (!in) {
        perror("Eroare la deschiderea fisierului de intrare");
        exit(1);
    }

    FILE *out = fopen(output_file, "w");
    if (!out) {
        perror("Eroare la deschiderea fisierului de iesire");
        exit(1);
    }

    char line[1024];
    int current_line = 0;
    while (fgets(line, sizeof(line), in)) {
        if (current_line >= start_line && current_line <= end_line) {
            fputs(line, out);
        }
        current_line++;
        if (current_line > end_line) break;
    }

    fclose(in);
    fclose(out);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizare: %s <shm_name>\n", argv[0]);
        exit(1);
    }

    char *shm_name = argv[1];
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Eroare la shm_open");
        exit(1);
    }

    struct stat sb;
    if (fstat(shm_fd, &sb) == -1) {
        perror("Eroare la fstat");
        exit(1);
    }

    Task *tasks = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (tasks == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(1);
    }

    int num_tasks = sb.st_size / sizeof(Task);
    int worker_id = getpid() % num_tasks;
    Task *my_task = &tasks[worker_id];

    extract_lines(my_task->input_file, "temp_part.txt", my_task->start_line, my_task->end_line);

    char command[1024];
    sprintf(command, "sort -n -t: -k3 temp_part.txt > %s", my_task->output_file);
    system(command);
    remove("temp_part.txt");

    munmap(tasks, sb.st_size);
    close(shm_fd);

    return 0;
}
