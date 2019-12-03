#include <sys/sendfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../timeutil.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: not enough arguments\n");
        exit(-1);
    }
    char filename[] = "fifo";
    char *input_name = argv[1];
    int fd_out, fd_in;
    if ((fd_out = open(filename, O_WRONLY)) < 0) {
        printf("Error: cannot open %s\n", filename);
        exit(-1);
    }
    if ((fd_in = open(input_name, O_RDONLY)) < 0) {
        printf("Error: cannot open %s\n", filename);
        exit(-1);
    }

    FILE* input = fopen(input_name, "r");
    if (input == NULL) {
        perror("file = fopen(argv[1], \"r\")");
        return 1;
    }
    struct stat buf;
    fstat(fd_in, &buf);
    size_t n = buf.st_size;

    ssize_t m;
    // Write size of file
    write(fd_out, &n, sizeof(size_t));
    while ((m = sendfile(fd_out, fd_in, 0, n)) > 0) {
        if (m < 0) {
            printf("Error: cannot write to fifo\n");
            exit(-1);
        }
    }

    if (fclose(input) < 0) {
        printf("Error: cannot close %s", input_name);
        exit(-1);
    }
    if (close(fd_out) < 0) {
        printf("Error: cannot close %s", filename);
        exit(-1);
    }
    return 0;
}
