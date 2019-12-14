#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/**
 * На вход программы передается одно число m - количество клиентов.
 * Затем запускается 1 сервер и m клиентов. Первым запускается сервер.
*/

int main(int argc, char *argv[]) {
	if (argc < 2) {
        printf("Manager: not enough arguments\n");
        exit(-1);
    }
    int m = strtol(argv[1], NULL, 10);

    const char client[] = "client";
    const char server[] = "server";

    for (int i=0; i<m + 1; i++) {
	    int res = fork();
		if (res == -1) {
			printf("Manager: cannot fork on step %d\n", i);
			exit(-1);
		} else
		if (res == 0) {
			if (i == 0)
				execl(server, server, NULL);
			else
				execl(client, client, NULL);
			printf("Manager: cannot exec\n");
			exit(-1);
		}
		if (i == 0)
		    sleep(1);
	}
    return 0;
}