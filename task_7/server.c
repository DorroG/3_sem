#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/select.h>
#include "common.h"

typedef struct {
    char fifo1[PATH_SIZE];
    char fifo2[PATH_SIZE];
    int fd_in;
    int fd_out;
} FifoInfo;

int main() {
    mkdir(FIFO_DIR, 0777);
    mknod(FIFO_FILE, S_IFIFO | 0666, 0);
    int fd = open(FIFO_FILE, O_RDWR);
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    FifoInfo clients[CLIENTS_MAX];
    int number_of_clients = 0;
    fd_set des;
    struct timeval timeval;

    printf("--Server-- server is ready!\n");

    char message[MESSAGE_SIZE];
    char token[MESSAGE_SIZE];
    while (1) {
        if (read(fd, message, MESSAGE_SIZE) > -1) {
            printf("--Server-- receive message '%s' \n", message);
            strcpy(token, strtok(message, " "));
            strcpy(clients[number_of_clients].fifo1, strtok(NULL, " "));
            strcpy(clients[number_of_clients].fifo2, strtok(NULL, " "));

            clients[number_of_clients].fd_in = open(clients[number_of_clients].fifo1, O_RDWR);
            clients[number_of_clients].fd_out = open(clients[number_of_clients].fifo2, O_RDWR);
            flags = fcntl(clients[number_of_clients].fd_in, F_GETFL, 0);
            fcntl(clients[number_of_clients].fd_in, F_SETFL, flags | O_NONBLOCK);

            sprintf(message, "%s", "ASK");
            write(clients[number_of_clients].fd_out, message, MESSAGE_SIZE);
            number_of_clients++;
        }
        if (number_of_clients <= 0)
            continue;
        timeval.tv_sec = 0;
        timeval.tv_usec = 0;
        FD_ZERO(&des);
        int vol = 0;
        for (int i = 0; i < number_of_clients; i++) {
            FD_SET(clients[i].fd_in, &des);
            vol += clients[i].fd_in;
        }
        int ret = select(vol + 1, &des, NULL, NULL, &timeval);
        if (ret > 0) {
            for (int i = 0; i < number_of_clients; i++) {
                if (FD_ISSET(clients[i].fd_in, &des)) {
                    while (read(clients[i].fd_in, message, MESSAGE_SIZE) <= 0);
                    printf("--Server-- receive message '%s' from %d\n", message, i);
                    if (strcmp(strtok(message, " "), "GET") == 0) {
                        char *file_content = readFromFile(strtok(NULL, " "));
                        write(clients[i].fd_out, file_content, strlen(file_content));
                    }
                }
            }
        }
    }
    return 0;
}