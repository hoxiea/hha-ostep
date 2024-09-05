/**
 * @file 4-execs.c
 * @author Hoxie Ackerman (hoxiea@gmail.com)
 * @brief Exercise 5-4 from OSTEP (http://pages.cs.wisc.edu/~remzi/OSTEP/)
 * @date 4 Sept, 2024
 *
 * Write a program that calls `fork()` and then calls some form of `exec()` to run the
 * program `/bin/ls`. See if you can try all of the variants of `exec()`, including (on
 * Linux) `execl()`, `execle()`, `execlp()`, `execv()`, `execvp()`, and `execvpe()`.
 * Why do you think there are so many variants of the same basic call?
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/**
 * The various `exec` functions using the following naming conventions:
 * - l: list, i.e. arguments are passed as a list of string arguments; last one is NULL
 * - v: vector, i.e. arguments are passed as a NULL-terminated array of strings
 * - e: environment, i.e. you can specify the environment vars for the new program
 * - p: path, i.e. searches your $PATH to find executables
 */

/**
 * @brief Run `/bin/ls` using execl
 *
 * This seems redundant - like, I already specified /bin/ls... why do I then have to
 * give it ls too?
 *
 * The answer is that all the arguments after the first one become the contents of argv.
 * And this is actually what happens when we run `ls` in our terminal:
 * - the shell receives the `ls` command
 * - the shell searches $PATH, looking for an executable called `ls`
 * - that search finds /bin/ls
 * - it uses fork and exec to run /bin/ls
 * - BUT CRUCIALLY, argv[0] is what I typed ("ls"), not the executable (/bin/ls)

 * This means that the shell can check argv[0] to see how it was invoked.
 * Maybe it was a symlink! For example, if `mylist` links to `/bin/ls` and I run
 * `mylist`, then argv[0] will actually be "mylist", and the program might be able
 * to use this information in an interestin way.
 *
 * I was also wondering about aliases. For example, I have "ua" (very easy to type on
 * Dvorak) aliased to `eza -la`. But apparently aliases are more of a text substitution -
 * this will usually get expanded to `eza -la`, and argv[0] will be "eza".
 * I confirmed this with the following command (on OSX):
 *   $ ua & ps -p $! -o command=
 *   [1] [pid]
 *   eza -la
 *   [...contents of my directory...]
 *   [1]  + [pid] done          eza -la
 */
void ls_execl() {
    printf("--- execl ---\n");
    execl("/bin/ls", "ls", NULL);
    // will only run if there was an error
    perror("execl");
    exit(1);
}

/**
 * @brief Run `/bin/ls` using execle
 * I thought it would be fun to use the environment variable functionality...
 * This requires us to actually invoke `/bin/sh -c` to run a command
 */
void ls_execle() {
    printf("--- execle ---\n");
    char* envp[] = {"MSG=hello", NULL};
    execle("/bin/sh", "sh", "-c", "ls && echo $MSG", NULL, envp);
    perror("execle");
    exit(1);
}

/**
 * @brief Run `/bin/ls` using execlp
 * Easy one: list of arguments, uses $PATH to find the executable
 */
void ls_execlp() {
    printf("--- execlp ---\n");
    execlp("ls", "ls", NULL);
    perror("execlp");
    exit(1);
}

/**
 * @brief Run `/bin/ls` using execvp
 * Easy one: vector of arguments, uses $PATH to find the executable
 */
void ls_execvp() {
    printf("--- execvp ---\n");
    char* args[] = {"ls", NULL};
    execvp(args[0], args);
    perror("execvp");
    exit(1);
}

int forker() {
    pid_t rc = fork();
    switch (rc) {
        case -1:  // fork failed
            perror("Fork failed\n");
            exit(1);
        case 0: {  // child
            /**
             * Since the execs replace the current process image with a new one,
             * only the first of these will execute...
             */
            ls_execlp();
            // ls_execle();
            // ls_execl();
            // ls_execvp();
        }
        default: {  // parent
            wait(NULL);
        }
    }
    return 0;
}

int main() {
    forker();
    return 0;
}