#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 256

typedef struct {
    int mtype;
    char mtext[BUF_SIZE];
} message;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: not enough arguments\n");
        exit(-1);
    }

    char filename[] = "sender.c";

    key_t key;
    if ((key = ftok(filename, 0)) < 0) {
        printf("Error: cannot ftok\n");
        exit(-1);
    }
    int msqid;
    if ((msqid = msgget(key, 0666)) < 0) {
        printf("Error: cannot shmget\n");
        exit(-1);
    }
    message message;
    int fd = open(argv[1], O_RDONLY);
    while (read(fd, message.mtext, BUF_SIZE) > 0) {
        if (msgsnd(msqid, &message, BUF_SIZE, 0) == -1) {
            printf("Error");
            exit(-1);
        }
    }

    if (close(fd) < 0) {
        printf("Error: cannot close %s", argv[1]);
        exit(-1);
    }
    if (msgctl(msqid, IPC_RMID, NULL) < 0) {
        printf("Error: cannot delete message %s", filename);
        exit(-1);
    }
    return 0;
}
