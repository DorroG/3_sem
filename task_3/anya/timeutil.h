#include <unistd.h>
#include <sys/time.h>

double get_time() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_sec * 1000.0 + currentTime.tv_usec / 1000.0;
}
