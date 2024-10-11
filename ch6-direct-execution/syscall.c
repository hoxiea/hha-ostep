/**
 * @file syscall.c
 * @author Hoxie Ackerman (hoxiea@gmail.com)
 * @brief Chapter 6 exercises from OSTEP (http://pages.cs.wisc.edu/~remzi/OSTEP/)
 * @date 2024-09-23
 *
 * Measure the cost of a system call.
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define NUM_ITERATIONS 10000000

/**
 * @brief Get the total number of nanoseconds in a timespec struct, considering
 *        both the seconds and nanoseconds fields.
 * @param ts timespec struct pointer
 * @return double total number of nanoseconds
 */
double ts_to_nanosecs(struct timespec *ts) {
    return (double)ts->tv_sec * 1e9 + (double)ts->tv_nsec;
}

double measure_time(int do_syscall) {
    struct timespec start, end;
    int pid = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        if (do_syscall) {
            pid = getpid();  // system call
        } else {
            pid = 0;  // dummy operation
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    // Prevent compiler from optimizing out the loop
    if (pid) printf("Last pid: %d\n", pid);

    return ts_to_nanosecs(&end) - ts_to_nanosecs(&start);
}

int main() {
    double time_with_syscall = measure_time(1);
    double time_without_syscall = measure_time(0);

    double syscall_time = time_with_syscall - time_without_syscall;
    double average_syscall_time = syscall_time / NUM_ITERATIONS;

    printf("Average time for system call alone: %.3f ns\n", average_syscall_time);
    return 0;
}
