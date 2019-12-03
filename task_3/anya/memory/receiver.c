#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "../timeutil.h"

int main() {
    double start_time, stop_time;
    char filename[] = "sender.c";
    key_t key;

    start_time = get_time();
    if ((key = ftok(filename, 0)) < 0) {
        printf("Error: cannot ftok\n");
        exit(-1);
    }

    int shmid;
    if ((shmid = shmget(key, 0, 0666)) < 0) {
        printf("Error: cannot shmget\n");
        exit(-1);
    }
    char* data = (char*) shmat(shmid, NULL, 0);
    stop_time = get_time();
    printf("Receive %ld bytes\n", strlen(data));
    printf("Time: %f ms\n", stop_time - start_time);

    if (shmdt(data) < 0) {
        printf("Error: cannot shmdt");
        exit(1);
    }
    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        printf("Error: cannot shmctl");
        exit(1);
    }
    return 0;
}
