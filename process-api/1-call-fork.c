/**
 * @file 1-call-fork.c
 * @author Hoxie Ackerman (hoxiea@gmail.com)
 * @brief Exercise 5-1 from OSTEP (http://pages.cs.wisc.edu/~remzi/OSTEP/)
 * @date 2024-08-31
 *
 * Write a program that calls `fork()`. Before calling `fork()`, have the main
 * process access a variable (e.g., x) and set its value to something (e.g.,
 * 100). What value is the variable in the child process? What happens to the
 * variable when both the child and parent change the value of x?
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int fork_stack() {
    int x = 100;
    int rc = fork();

    switch (rc) {
        case -1:  // fork failed
            fprintf(stderr, "fork failed\n");
            exit(1);

        case 0:  // child
            printf("hello, child! x = %d\n", x);
            x = 200;
            printf("goodbye, child! x = %d\n", x);
            break;

        default:  // parent
            printf("hello, parent! x = %d\n", x);
            x = 300;
            printf("goodbye, parent! x = %d\n", x);
    }
    return 0;
}

/**
 * Running the above code on my machine produces:
 *   hello, parent! x = 100
 *   goodbye, parent! x = 300
 *   hello, child! x = 100
 *   goodbye, child! x = 200
 *
 * So it seems that the fork receives a copy of variable x: it's 100 when
 * the child first accesses it, even though the parent set it to be 300.
 *
 * But after recently reading about Rust's Copy trait and remember the important
 * distinction between the stack and the heap in terms of memory, I wonder
 * if a variable on the heap would legitimately be copied, or if it's just the
 * pointer that would be copied...
 */

int fork_heap() {
    int *ptr = (int *)malloc(sizeof(int));
    if (ptr == NULL) {
        printf("Allocation failed.\n");
        return 1;
    }
    *ptr = 100;
    int rc = fork();

    switch (rc) {
        case -1:  // fork failed
            fprintf(stderr, "fork failed\n");
            exit(1);

        case 0:  // child
            printf("hello, child! x = %d\n", *ptr);
            *ptr = 200;
            printf("goodbye, child! x = %d\n", *ptr);
            free(ptr);
            break;

        default:  // parent
            printf("hello, parent! x = %d\n", *ptr);
            *ptr = 300;
            printf("goodbye, parent! x = %d\n", *ptr);
            free(ptr);
    }

    return 0;
}

/**
 * This works too. And poking around online, it seems like the forked process
 * really does get a deep copy of the parent process's memory, including the
 * heap, though it's a bit more subtle than that - the deep copy isn't actually
 * made until one of the processes starts writing to its memory space, at which
 * point the OS realizes that the memory for the two processes actually needs
 * to diverge, so it makes the copy. But if the processes are only reading,
 * then no copying will occur, which is nice for performance reasons.
 *
 * References:
 * - https://unix.stackexchange.com/q/155017
 * - https://man7.org/linux/man-pages/man2/fork.2.html
 *   "Memory writes... by one of the processes do not affect the other."
 */

int main() {
    /**
     * Interestingly, running both of these causes more than 4 sets of output
     * to occur, due to how forking actually works.
     */

    printf("Fork stack:\n");
    fork_stack();
    printf("\n");
    printf("Fork heap:\n");
    fork_heap();
    return 0;
}