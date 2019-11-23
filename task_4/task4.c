#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

/**
 * This program integrate function lnx from 'left' to 'right' using Monte-Carlo method
 * N - number of points used; N = 2^22
 */
#define N 4194304
// function definition area
#define left 1
#define right 10
// function value range
#define down 0
#define up 3

struct thread_arg {
    double x;
    double dx;
    double result;
    int n; // number of points
    pthread_t thread_id;
};

double random_in_range(double from, double to) {
    return drand48() * (to - from) + from;
}

// Function to integrate f(x) = ln(x)
double f(double x) {
    return log(x);
}

// Antiderivative of function f
double F(double x) {
    return x*(log(x) - 1);
}

void *integrate(void* _arg) {
    struct thread_arg *arg = (struct thread_arg *) _arg;
    for (int i = 0; i < arg->n; i++) {
        double first_rand = random_in_range(arg->x, arg->x + arg->dx);
        double second_rand = random_in_range(down, up);
        if (f(first_rand) > second_rand)
            arg->result++;
    }
}

// Return current system time in microseconds
long long get_time() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_sec * 1000000 + currentTime.tv_usec;
}

int main() {
    // Open file to save data for graph
    FILE *file;
    if ((file = fopen("data.txt", "wb")) == NULL) {
        printf("Error: cannot open file\n");
        exit(-1);
    }
    long long start_time, stop_time;
    double time_in_millis;
    double rectangle_area = (right - left) * (up - down);
    double real_answer = F(right) - F(left);
    for (int i = 1; i * i <= N; i++) {
        if (N % i != 0)
            continue;
        double dx = (double) (right - left) / i;
        struct thread_arg *threads_args = (struct thread_arg *) malloc(i * sizeof(struct thread_arg));
        for (int j = 0; j < i; j++) {
            threads_args[j].x = j * dx + left;
            threads_args[j].dx = dx;
            threads_args[j].n = N / i;
            threads_args[j].result = 0;
        }
        // Measuring time
        start_time = get_time();
        // Creating threads
        for (int j = 0; j < i; j++)
            pthread_create(&(threads_args[j].thread_id), NULL, integrate, &threads_args[j]);
        for (int j = 0; j < i; j++)
            pthread_join(threads_args[j].thread_id, NULL);
        // Measuring time
        stop_time = get_time();
        time_in_millis = (double) (stop_time - start_time) / 1000.0;

        // Calculating answer
        double answer = 0;
        for (int j = 0; j < i; j++) {
            answer += threads_args[j].result;
        }
        answer = rectangle_area * answer / N;
        // Printing answer
        // σ - difference between calculated integral and analytic integral
        printf("Integral(f) = %f, σ = %f, %d threads, time = %f ms\n",
                answer, fabs(real_answer - answer), i, time_in_millis);
        // Save graph data to data.txt
        fprintf(file, "%d %f\n", i, time_in_millis);
    }
    fclose(file);
    return 0;
}
