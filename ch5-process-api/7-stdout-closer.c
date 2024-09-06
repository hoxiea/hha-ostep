/**
 * @file 7-execs.c
 * @author Hoxie Ackerman (hoxiea@gmail.com)
 * @brief Exercise 5-7 from OSTEP (http://pages.cs.wisc.edu/~remzi/OSTEP/)
 * @date 6 Sept, 2024
 *
 * Write a program that creates a child process, and then in the child closes standard
 * output (STDOUT_FILENO). What happens if the child calls `printf()` to print some
 * output after closing the decriptor?
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/**
 * In my terminal, this prints:
 *   Wrote 15 bytes
 *   Parent saw child (pid = xxxxx) finish
 *
 * From this, we can see that `printf` attempts to write and returns the number of
 * characters it would have written, but because STDOUT is closed, `printf` fails
 * silently. Thankfully STDERR is still open, so we can make something appear as output.
 *
 * We also confirm that the parent and child have separate file descriptors, since the
 * parent's printf prints as expected.
 */
int black_hole() {
    pid_t rc = fork();
    switch (rc) {
        case -1:  // fork failed
            perror("Fork failed\n");
            exit(1);
        case 0: {  // child
            close(STDOUT_FILENO);
            int bytes_written = printf("Where do I go?\n");
            dprintf(STDERR_FILENO, "Wrote %d bytes\n", bytes_written);
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
    black_hole();
    return 0;
}