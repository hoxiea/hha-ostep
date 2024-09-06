/**
 * @file 8-pipe-children.c
 * @author Hoxie Ackerman (hoxiea@gmail.com)
 * @brief Exercise 5-8 from OSTEP (http://pages.cs.wisc.edu/~remzi/OSTEP/)
 * @date 6 Sept, 2024
 *
 * Write a program that creates two children and connects the STDOUT of one to the STDIN
 * of the other, using the `pipe()` system call.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int pipefd[2];
    pid_t pid1, pid2;

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Create first child
    pid1 = fork();
    if (pid1 < 0) {
        perror("fork1");
        exit(1);
    } else if (pid1 == 0) {
        // First child (writer)
        close(pipefd[0]);                // Close unused read end
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe
        close(pipefd[1]);                // Close original write end

        execlp("ls", "ls", "-l", NULL);  // Execute 'ls -l'
        perror("execlp");                // If execlp returns, it failed
        exit(1);
    }

    // Create second child
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork2");
        exit(1);
    } else if (pid2 == 0) {
        // Second child (reader)
        close(pipefd[1]);               // Close unused write end
        dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin to pipe
        close(pipefd[0]);               // Close original read end

        execlp("wc", "wc", "-l", NULL);  // Execute 'wc -l'
        perror("execlp");
        exit(1);
    }

    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for both children
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("Parent: Both children have finished.\n");

    return 0;
}