/**
 * @file 6-execs.c
 * @author Hoxie Ackerman (hoxiea@gmail.com)
 * @brief Exercise 5-6 from OSTEP (http://pages.cs.wisc.edu/~remzi/OSTEP/)
 * @date 6 Sept, 2024
 *
 * Write a slight modification of the previous program, this time using `waitpid()`
 * instead of `wait()`. When would `waitpid()` be useful?
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int waiter_pid() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("Child process (PID: %d) is running\n", getpid());
        sleep(2);  // Simulate some work
        printf("Child process (PID: %d) is done\n", getpid());
        exit(0);
    } else {
        // Parent process
        printf("Parent process (PID: %d) is waiting for child (PID: %d)\n", getpid(), pid);

        int status;
        pid_t waited_pid = waitpid(pid, &status, 0);

        if (waited_pid == -1) {
            perror("waitpid failed");
            exit(1);
        }

        if (WIFEXITED(status)) {
            printf("Child process (PID: %d) exited with status %d\n", waited_pid, WEXITSTATUS(status));
        } else {
            printf("Child process (PID: %d) did not exit normally\n", waited_pid);
        }
    }

    return 0;
}

/**
 * Then I had an interesting conversation with Claude about a parent creating multiple
 * child processes. The first attempts just involved a second call to `fork()` in the
 * "else" block but then that required a second set of if/else if/else nested with the
 * first, i.e. the pyramid of doom. When I asked about that, we came up with the
 * following, which is apparently a common pattern for more complex multi-process
 * programs (not to be confused with multi-thread programs...):
 */
#define NUM_CHILDREN 3

void child_process(int child_num) {
    printf("Child %d (PID: %d) is running\n", child_num, getpid());
    sleep(child_num);  // Simulate different amounts of work
    printf("Child %d (PID: %d) is done\n", child_num, getpid());
    exit(0);
}

int many_childen() {
    pid_t pids[NUM_CHILDREN];

    // Create children
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("Fork failed");
            exit(1);
        } else if (pids[i] == 0) {
            // Child process
            child_process(i + 1);
            // child_process doesn't return, but just in case:
            exit(0);
        }
    }

    // Parent process
    printf("Parent (PID: %d) created %d children\n", getpid(), NUM_CHILDREN);

    // Wait for all children
    for (int i = 0; i < NUM_CHILDREN; i++) {
        int status;
        pid_t waited_pid = waitpid(pids[i], &status, 0);
        if (waited_pid == -1) {
            perror("waitpid failed");
        } else {
            printf("Child %d (PID: %d) has finished\n", i + 1, waited_pid);
        }
    }

    printf("Parent (PID: %d) is done\n", getpid());

    return 0;
}

int main() {
    waiter_pid();
    printf("\n");
    many_childen();
    return 0;
}