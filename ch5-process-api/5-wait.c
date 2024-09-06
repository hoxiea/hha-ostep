/**
 * @file 5-execs.c
 * @author Hoxie Ackerman (hoxiea@gmail.com)
 * @brief Exercise 5-5 from OSTEP (http://pages.cs.wisc.edu/~remzi/OSTEP/)
 * @date 6 Sept, 2024
 *
 * Now write a program that uses `wait()` to wait for the child process to finish in the
 * parent.  What does `wait()` return? What happens if you use `wait()` in the child?
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/**
 * @brief Use `wait()` to make the parent wait for its child to finish
 * From the documentation, wait() returns the process ID of the terminated child
 * on success; on failure, -1 is returned
 * Reference: https://man7.org/linux/man-pages/man2/waitpid.2.html#RETURN_VALUE
 */
int parent_waits() {
    pid_t rc = fork();
    switch (rc) {
        case -1:  // fork failed
            perror("Fork failed\n");
            exit(1);
        case 0: {  // child
            printf("Hello from child (pid = %d)\n", getpid());
            break;
        }
        default: {  // parent
            pid_t finished_child = wait(NULL);
            printf("Parent saw child (pid = %d) finish\n", finished_child);
        }
    }
    return 0;
}

/**
 * The fact that wait() returns the process ID of the terminated CHILD, it seems like a
 * bad idea to call wait in the child process, since the parent isn't the child of the
 * child... but thankfully, the program doesn't block waiting for a non-existant child
 * to finish, it just returns -1.
 */
int child_waits() {
    pid_t rc = fork();
    switch (rc) {
        case -1:  // fork failed
            perror("Fork failed\n");
            exit(1);
        case 0: {  // child
            printf("Hello from child (pid = %d)\n", getpid());
            pid_t finished_nobody = wait(NULL);
            if (finished_nobody == -1) {
                perror("Nobody to wait for!\n");
                exit(1);
            }
            printf("Saw child (pid = %d) finish\n", finished_nobody);
            break;
        }
        default: {  // parent
            pid_t finished_child = wait(NULL);
            printf("Parent saw child (pid = %d) finish\n", finished_child);
        }
    }
    return 0;
}

int main() {
    // printf("--- Parent waits ---\n");
    // parent_waits();
    printf("\n--- Child waits ---\n");
    child_waits();
    return 0;
}