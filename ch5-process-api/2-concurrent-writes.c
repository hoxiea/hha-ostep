/**
 * @file 2-concurrent-writes.c
 * @author Hoxie Ackerman (hoxiea@gmail.com)
 * @brief Exercise 5-2 from OSTEP (http://pages.cs.wisc.edu/~remzi/OSTEP/)
 * @date 2024-09-01
 *
 * Write a program that opens a file (with the `open()` system call) and then calls
 * `fork()` to create a new process. Can both the child and parent access the file
 * descriptor returned by `open()`? What happens when they are writing to the file
 * concurrently, i.e. at the same time?
 */

#include <fcntl.h>  // file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_ITERATIONS 100

void write_message(int fd, const char* msg, int num_times) {
    for (int i = 0; i < num_times; i++) {
        ssize_t bytes_written = write(fd, msg, strlen(msg));
        if (bytes_written == -1) {
            perror("Write failed!\n");
            exit(1);
        }
    }
}

int main() {
    int fd = open("./q2.output", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Failed to open file\n");
        exit(1);
    }

    int rc = fork();

    switch (rc) {
        case -1:  // fork failed
            perror("Fork failed\n");
            exit(1);
        case 0:  // child
            write_message(fd, "Child\n", NUM_ITERATIONS);
            printf("Child finished writing\n");
            break;

        default:  // parent
            write_message(fd, "Parent\n", NUM_ITERATIONS);
            printf("Parent finished writing\n");
            // wait(NULL);
    }

    close(fd);
    return 0;
}

/**
 * When `fork` is called, the child process inherits a copy of the parent's
 * file descriptors. So both processes have a valid file descriptor referring
 * to the same open file.
 * Reference: https://man7.org/linux/man-pages/man2/fork.2.html
 *
 * I added the looping to demonstrate that the writes will often happen in an
 * interwoven manner as the two processes get time from the scheduler.
 */