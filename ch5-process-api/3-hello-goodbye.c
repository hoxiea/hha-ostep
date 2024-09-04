/**
 * @file 3-hello-goodbye.c
 * @author Hoxie Ackerman (hoxiea@gmail.com)
 * @brief Exercise 5-3 from OSTEP (http://pages.cs.wisc.edu/~remzi/OSTEP/)
 * @date 4 Sept, 2024
 *
 * Write another program using `fork()`. The child process should print "hello";
 * the parent process should print "goodbye". You should try to ensure that the
 * child process always prints first; can you do this *without* calling `wait()`
 * in the parent?
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/**
 * If we can't use the `wait()` function at the beginning of the parent branch,
 * we could instead just put the parent branch to sleep for some reasonable
 * amount of time.
 *
 * This isn't a great solution, though - there's no guarantee that the parent
 * process will sleep until the child has printed, and if we make the sleep time
 * too big in order to make the correct sequence likely, then there's a delay
 * between the child and parent printing.
 */
int option1_sleep() {
    pid_t rc = fork();
    switch (rc) {
        case -1:  // fork failed
            perror("Fork failed\n");
            exit(1);
        case 0:  // child
            printf("hello\n");
            break;
        default: {  // parent
            // wait(NULL);  // parent waits for child to finish
            // sleep(1);    // works, but bit of a delay
            struct timespec sleep_time = {0, 400000000L};
            nanosleep(&sleep_time, NULL);
            printf("goodbye\n");
        }
    }
    return 0;
}

/**
 * A better solution uses pipes: the child process can run first, and then send a
 * "message" to the parent process letting it know that it completed.
 *
 * This is a nice approach, in terms of the code! A few interesting points:
 *
 * When a process forks, the child receives a copy of the parent's file descriptors
 * (FDs). These FDs are independent references to the same underlying kernel resource,
 * which is why the child can close both FDs and the parent will still be able to use
 * its copy.
 *
 * It turns out that the kernel maintains a reference count for each open FD. The
 * file/pipe is only truly closed when all references to it are closed. Said another
 * way, a pipe remains open as long as at least one process has an open FD to it.
 *
 * Reading from a pipe blocks if:
 * - the pipe is empty
 * - there's an open write end somewhere
 *
 * Writing to a pipe with no open read end typically raises a SIGPIPE signal.
 *
 * `exit()` closes all FDs associated with the process, no matter the exit code.
 * So technically, we don't need to close the unused pipe ends like we do below.
 * But it's a good idea to close unused pipe ends regardless! Why? Because:
 * - It makes the intention clearer
 * - It releases the resource as soon as it's not needed
 * - If a child calls something else via `exec`, open file descriptors are inherited.
 *   But you might not want the other process you're exec'ing to have access to the FD
 *   you weren't using...
 */
int option2_pipe() {
    // array to hold the two pipe file descriptors
    // index0 is the READ end, and index1 is the WRITE end
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe error");
        exit(1);
    }

    pid_t rc = fork();
    switch (rc) {
        case -1:  // fork failed
            perror("Fork failed\n");
            exit(1);
        case 0:                 // child
            close(pipefd[0]);   // close unused READ end of the pipe
            printf("hello\n");  // print child message

            if (write(pipefd[1], "x", 1) == -1) {  // send message to parent
                perror("child write failed");
                exit(1);
            }

            close(pipefd[1]);  // close WRITE end of the pipe
            exit(0);
        default: {             // parent
            close(pipefd[1]);  // close unused WRITE end of the pipe
            char buf;
            if (read(pipefd[0], &buf, 1) == -1) {  // block on child message
                perror("parent read failed");
                exit(1);
            }
            printf("goodbye\n");  // print parent message
            close(pipefd[0]);     // close READ end of the pipe
        }
    }
    return 0;
}

int main() {
    // option1_sleep();
    option2_pipe();
    return 0;
}