#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_WORKERS 100
#define SHM_NAME "/sort_shm"

typedef struct {
    int start_line;
    int end_line;
    int worker_id;
    char input_file[256];
    char output_file[256];
} Task;

int count_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Eroare la deschiderea fisierului");
        exit(1);
    }
    
    int count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') count++;
    }
    
    fclose(file);
    return count;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Utilizare: %s <fisier_intrare> <num_workers>\n", argv[0]);
        exit(1);
    }

    char *input_file = argv[1];
    int num_workers = atoi(argv[2]);
    if (num_workers <= 0 || num_workers > MAX_WORKERS) {
        fprintf(stderr, "Numar invalid de workeri. Trebuie intre 1 si %d\n", MAX_WORKERS);
        exit(1);
    }

    int total_lines = count_lines(input_file);
    int lines_per_worker = total_lines / num_workers;
    int remaining_lines = total_lines % num_workers;

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Eroare la shm_open");
        exit(1);
    }

    if (ftruncate(shm_fd, num_workers * sizeof(Task)) == -1) {
        perror("Eroare la ftruncate");
        exit(1);
    }

    Task *tasks = mmap(NULL, num_workers * sizeof(Task), 
                      PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (tasks == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(1);
    }

    int current_line = 0;
    for (int i = 0; i < num_workers; i++) {
        tasks[i].start_line = current_line;
        tasks[i].end_line = current_line + lines_per_worker - 1 + (i < remaining_lines ? 1 : 0);
        tasks[i].worker_id = i;
        strcpy(tasks[i].input_file, input_file);
        sprintf(tasks[i].output_file, "output_part_%d.txt", i);
        current_line = tasks[i].end_line + 1;
    }

    for (int i = 0; i < num_workers; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            execl("./worker_sort2", "./worker_sort2", SHM_NAME, NULL);
            perror("Eroare la exec");
            exit(1);
        } else if (pid < 0) {
            perror("Eroare la fork");
            exit(1);
        }
    }

    for (int i = 0; i < num_workers; i++) {
        wait(NULL);
    }

    FILE *final_output = fopen("sorted_output.txt", "w");
    if (!final_output) {
        perror("Eroare la deschiderea fisierului de iesire");
        exit(1);
    }

    for (int i = 0; i < num_workers; i++) {
        FILE *part_file = fopen(tasks[i].output_file, "r");
        if (!part_file) {
            perror("Eroare la deschiderea fisierului partial");
            exit(1);
        }

        char line[1024];
        while (fgets(line, sizeof(line), part_file)) {
            fputs(line, final_output);
        }
        fclose(part_file);
        remove(tasks[i].output_file);
    }

    fclose(final_output);
    munmap(tasks, num_workers * sizeof(Task));
    close(shm_fd);
    shm_unlink(SHM_NAME);

    printf("Sortare finalizata. Rezultatul in sorted_output.txt\n");
    return 0;
}
