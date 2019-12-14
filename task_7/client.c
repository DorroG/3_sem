#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include "common.h"

int main() {
    int pid = getpid();
    int fd = open(FIFO_FILE, O_RDWR);
    char fifo1[PATH_SIZE];
    char fifo2[PATH_SIZE];
    char message[MESSAGE_SIZE];
    // Create and open fifo files
    sprintf(fifo1, "fifo/t%d", pid);
    sprintf(fifo2, "fifo/r%d", pid);
    mknod(fifo1, S_IFIFO | 0666, 0);
    mknod(fifo2, S_IFIFO | 0666, 0);
    int fd1, fd2;
    fd1 = open(fifo1, O_RDWR);
    fd2 = open(fifo2, O_RDWR);
    int flags = fcntl(fd2, F_GETFL, 0);
    fcntl(fd2, F_SETFL, flags | O_NONBLOCK);

    // Send 'REGISTER' command
    sprintf(message, "REGISTER %s %s", fifo1, fifo2);
    write(fd, message, MESSAGE_SIZE);
    printf("--Client(%d)-- Send 'REGISTER' command\n", pid);
    // Wait for 'ASK' command
    while (read(fd2, message, MESSAGE_SIZE) <= 0 && strcmp(message, "ASK") != 0);
    // Choose random file name and get it from server
    srand(time(NULL));
    sprintf(message, "GET %s", files[rand() % FILES_NUMBER]);
    write(fd1, message, MESSAGE_SIZE);
    printf("--Client(%d)-- Message '%s' was sent\n", pid, message);

    // Read first MESSAGE_SIZE-1 bytes of file content
    while (read(fd2, message, MESSAGE_SIZE - 1) <= 0);
    message[MESSAGE_SIZE - 1] = '\0';
    // Read rest file
    char *rest = readFromFileDescriptor(fd2);
    printf("--Client(%d)-- receive file: \n%s%s\n", pid, message, rest);
    return 0;
}