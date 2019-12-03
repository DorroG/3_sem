#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include "../timeutil.h"

#define BUF_SIZE 256

typedef struct {
    int mtype;
    char mtext[BUF_SIZE];
} message;

int main() {
    char filename[] = "sender.c";
    message msg;
    key_t key;
    if ((key = ftok(filename, 0)) < 0) {
        printf("Error: cannot ftok\n");
        exit(-1);
    }
    int msqid;
    if ((msqid = msgget(key, IPC_CREAT | IPC_EXCL | 0666)) < 0) {
        printf("Error: cannot shmget\n");
        exit(-1);
    }

    double start_time, stop_time;

    int i = 0;
    ssize_t n;
    while((n = msgrcv(msqid, &msg, BUF_SIZE, 0, 0)) > 0) {
        if (i == 0) {
            start_time = get_time();
            i++;
        }
        printf("Server received %ld bytes\n", n);
    }
    stop_time = get_time();
    printf("Time: %f ms", stop_time - start_time);
    return 0;
}
