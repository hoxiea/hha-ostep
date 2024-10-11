# OSTEP Ch 6: Limited Direct Execution

Hamilton Hoxie Ackerman (hha)

## Measuring the Cost of a System Call

The obvious way to measure the cost of a system call is to:

- Make the note of the time before the system call
- Make the system call
- Make the note of the time after the system call
- Subtract the two times

The authors suggest that `gettimeofday()` might not provide the resolution needed to measure the cost of a system call. [This](https://stackoverflow.com/questions/5362577/c-gettimeofday-for-computing-time) StackOverflow thread confirms this issue and suggests using `clock_gettime(CLOCK_MONOTONIC)` instead, so I came up with this:

```c
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int pid = getpid();  // system call
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Time to make a system call: %ld ns\n", end.tv_nsec - start.tv_nsec);
    printf("pid = %d\n", pid);
    return 0;
}
```

Running this code produces `0 ns`. Pretty fast!

Seriously though, I think this is happening because the nanosecond resolution of `CLOCK_MONOTONIC` is too low to measure the cost of a single system call on a system capable of billions of operations per second - my M2 Max has peak performance around 3.5 GHz, meaning it can perform a simple operation in `1 / 3.5 GHz ≈ .286 nanoseconds` under ideal conditions.

Since I don't think I can find a clock with sub-nano precision, the next-best option is to make repeated system calls and measure the average time. That might look something like this:

```c
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
double timespec_to_nanoseconds(struct timespec *ts) {
    return (double)ts->tv_sec * 1e9 + (double)ts->tv_nsec;
}

int main() {
    struct timespec start, end;
    double total_time;
    int pid;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pid = getpid();  // make a system system call
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    total_time = timespec_to_nanoseconds(&end) - timespec_to_nanoseconds(&start);
    double average_time = total_time / NUM_ITERATIONS;

    printf("Average time to make a system call: %.3f ns\n", average_time);
    printf("pid = %d\n", pid);
    return 0;
}
```

This code reports `Average time to make a system call: 3.120 ns` on my machine, which is about an order of magnitude higher than the ideal time of `.286 ns` - we're getting into the right ballpark.

The issue with this approach is that we're just not performing `NUM_ITERATIONS` systems calls in this code - we're also performing the loop overhead (and possibly the assignment, depending on the compiler). This adds extra time and means that we're not strictly measuring the system call.

The solution to this issue would seem to be to:

1. Measure the above code, including loop, assignment, etc. and system call.
2. Run the same code, except without the system call.
3. Find the difference, which should just be the system call time.

That might look something like this:

```c
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define NUM_ITERATIONS 10000000

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
```

This reports `Average time for system call alone: 1.855 ns`, a little more than half of what we got before. This implies a clock speed of `1 / 1.855 = .539 GHz`, which is definitely the right order of magnitude.

## Measuring the Cost of a Context Switch
