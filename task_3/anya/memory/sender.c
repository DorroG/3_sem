#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "../timeutil.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: not enough arguments\n");
        exit(-1);
    }
    double start_time, stop_time;
    char filename[] = "sender.c";
    char *input_name = argv[1];

    key_t key;
    if ((key = ftok(filename, 0)) < 0) {
        printf("Error: cannot ftok\n");
        exit(-1);
    }

    FILE* file;
    if ((file = fopen(input_name, "r")) == NULL) {
        printf("Error: cannot open %s\n", input_name);
        exit(-1);
    }
    struct stat buf;
    fstat(fileno(file), &buf);
    size_t n = buf.st_size;
    int shmid;
    if ((shmid = shmget(key, n, IPC_CREAT | IPC_EXCL | 0666)) < 0) {
        printf("Error: cannot shmget\n");
        exit(-1);
    }

    start_time = get_time();

    char* data = (char*) shmat(shmid, NULL, 0);
    fread(data, 1, n, file);

    stop_time = get_time();
    printf("Time: %f ms\n", stop_time - start_time);

    if (shmdt(data) == -1) {
        perror("Error: cannot shmdt");
        exit(1);
    }
    return 0;
}
