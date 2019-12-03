#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../timeutil.h"

int main() {
    char filename[] = "fifo";
    double start_time, stop_time;

    if (mkfifo(filename, 0666) < 0) {
        printf("Error: cannot mkfifo\n");
        exit(-1);
    }

    printf("Waiting...\n");
    int fd;
    if ((fd = open(filename, O_RDONLY)) < 0) {
        printf("Error: cannot open file\n");
        exit(-1);
    }
    start_time = get_time();
    size_t n;
    if (read(fd, &n, sizeof(size_t)) < 0) {
        printf("Error: cannot read from fifo");
        exit(-1);
    }

    printf("Total size: %lu\n", n);

    char* input = (char*) malloc(n);
    ssize_t m;
    while ((m = read(fd, input, n)) > 0) {
        printf("Receive %ld bytes\n", m);
        input += m;
    }
    printf("%s\n", input);
    stop_time = get_time();
    printf("Time: %f ms\n", stop_time - start_time);

    if (close(fd) < 0) {
        printf("Error: cannot close %s", filename);
        exit(-1);
    }
    if (remove(filename) < 0) {
        printf("Error: cannot remove %s", filename);
        exit(-1);
    }
    return 0;
}
