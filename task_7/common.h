#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

#define FILES_NUMBER 3
#define PATH_SIZE 64
#define MESSAGE_SIZE 256
#define CLIENTS_MAX 64

#define FIFO_FILE "fifo/common_fifo"
#define FIFO_DIR "fifo/"

const char files[FILES_NUMBER][PATH_SIZE] = {"server.c", "client.c", "manager.c"};

#define BUF_SIZE 1024

char *readFromFileDescriptor(int fd);
char *readFromFile(char *fileName);

char *readFromFileDescriptor(int fd) {
    char *result = NULL;
    char *buffer = (char *) malloc(BUF_SIZE * sizeof(char));
    ssize_t read_num = 0;
    int length = 0;
    while ((read_num = read(fd, buffer, BUF_SIZE)) > 0) {
        length += read_num;
        if (result == NULL) {
            result = (char *) calloc(read_num, sizeof(char));
        } else {
            result = realloc(result, (length * 2) * sizeof(char));
        }
        strncat(result, buffer, read_num);
    }
    free(buffer);
    if (close(fd) < 0) {
        printf("-- libfile.readFromFileDescriptor -- Error: could not close file\n");
    }
    return result;
}

char *readFromFile(char *fileName) {
    int fd;
    if ((fd = open(fileName, O_RDONLY)) < 0) {
        printf("-- libfile.readFromFile -- Error: cannot open file %s\n", fileName);
        exit(-1);
    }
    return readFromFileDescriptor(fd);
}