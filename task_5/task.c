#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

int out = 0;
int counter = 128;
pid_t pid;

void successful_exit() {
    exit(0);
}

void nothing() {}

void one() {
    out += counter;
    counter /= 2;
    kill(pid, SIGUSR1);
}

void zero() {
    counter /= 2;
    kill(pid, SIGUSR1);
}

// Redefine actions: SIGCHLD, SIGUSR1, SIGUSR2
void set_up_actions() {
    // Exit on SIGCHLD
    struct sigaction act_exit;
    memset(&act_exit, 0, sizeof(act_exit));
    act_exit.sa_handler = successful_exit;
    sigfillset(&act_exit.sa_mask);
    sigaction(SIGCHLD, &act_exit, NULL);

    // one() on SIGUSR1
    struct sigaction act_one;
    memset(&act_one, 0, sizeof(act_one));
    act_one.sa_handler = one;
    sigfillset(&act_one.sa_mask);
    sigaction(SIGUSR1, &act_one, NULL);

    // zero() on SIGUSR1
    struct sigaction act_zero;
    memset(&act_zero, 0, sizeof(act_zero));
    act_zero.sa_handler = zero;
    sigfillset(&act_zero.sa_mask);
    sigaction(SIGUSR2, &act_zero, NULL);
}

void send_data(char *filename, int ppid, sigset_t *set) {
    char c;
    int fd;
    if ((fd = open(filename, O_RDONLY)) < 0) {
        printf("Error: cannot open file");
        exit(-1);
    }
    int i;
    while (read(fd, &c, 1) > 0) {
        // Check if parent is alive
        alarm(1);
        // Send data
        for (i = 128; i >= 1; i /= 2) {
            if (i & c)
                kill(ppid, SIGUSR1);
            else
                kill(ppid, SIGUSR2);
            // Wait for parent response to check if it is alive
            sigsuspend(set);
        }
    }
}

/*
 * Receiving file
 * Loop is endless because it receive data until child is alive.
 * When child dies, parent receive SIGALRM and exit
*/
void receive_data(sigset_t *set) {
    while (1) {
        if (counter == 0) {
            write(STDOUT_FILENO, &out, 1);
            counter = 128;
            out = 0;
        }
        sigsuspend(set);
    }
}

int main() {
    char filename[] = "task.c";
    // Parent pid
    pid_t ppid = getpid();

    sigset_t set;
    set_up_actions();

    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, NULL);
    sigemptyset(&set);

    pid = fork();
    if (pid == -1) {
        printf("Error: cannot fork");
        exit(-1);
    } else if (pid == 0) {
        // Child
        sigemptyset(&set);
        // do nothing on SIGUSR1
        struct sigaction act_empty;
        memset(&act_empty, 0, sizeof(act_empty));
        act_empty.sa_handler = nothing;
        sigfillset(&act_empty.sa_mask);
        sigaction(SIGUSR1, &act_empty, NULL);
        // exit on SIGALRM
        struct sigaction act_alarm;
        memset(&act_alarm, 0, sizeof(act_alarm));
        act_alarm.sa_handler = successful_exit;
        sigfillset(&act_alarm.sa_mask);
        sigaction(SIGALRM, &act_alarm, NULL);
        // Send file
        send_data(filename, ppid, &set);
    } else {
        receive_data(&set);
    }
    return 0;
}
