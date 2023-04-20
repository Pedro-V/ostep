/**
 * This file explores Unix's process api, such as fork, wait and the exec family, and 
 * corresponds to the code homeworks of chapter 5 of OSTEP.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


/**
 * Creates a variable, forks the process and changes it to different values in the parent 
 * and the child process, then prints it.
 * 
 * Each process prints its respective value.
*/
void q1(void) {
    int x = 100;
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(EXIT_FAILURE);
    }
    if (rc == 0) { // child
        x = 5;
    }
    else { // parent
        wait(NULL);
        x = 200;
    }
    printf("x = %d (pid: %d)\n", x, getpid());
}


/**
 * Open a file, forks and both processes try to write to the opened file.
 * 
 * The first scheduled process writes first to file.
*/
void q2(void) {
    int fd = open("./q2.txt", O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
    if (fd < 0) {
        fprintf(stderr, "Opening the file failed\n");
        exit(EXIT_FAILURE);
    }
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(EXIT_FAILURE);
    }
    
    // both processes try to write concurrently to q2.txt
    if (rc == 0) { //child
        char* str = "child\n";
        write(fd, str, 6);
    }
    else {
        char* str = "parent\n";
        write(fd, str, 7);
    }
}

/**
 * TODO: FIX THIS
 * Forks the process, and makes sure the child prints first without using `wait`
*/
void q3(void) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(EXIT_FAILURE);
    }
    if (rc == 0) { // child
        printf("Hello\n");
    }
    else { // parent
        printf("Goodbye\n");
    }
}


/**
 * Forks the process and calls different exec-family functions. 
 * Comment-out the desired functions to be called.
*/
void q4(void) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(EXIT_FAILURE);
    }
    if (rc == 0) {
        printf("Child calling some execs (pid: %d)\n", getpid());

        // char* args[] = {"/usr/bin/ls", "-al", "*.c", NULL};
        // char* env[] = {NULL}; 
        // if (execve(args[0], args, env) < 0) {
        //     perror("execve");
        //     exit(EXIT_FAILURE);
        // }

        if (execlp("ls", "ls", "-la", "-G", NULL) < 0) {
            perror("execlp");
            exit(EXIT_FAILURE);
        }
    }
    else {
        wait(NULL);
        printf("Parent calling some execs (pid: %d)\n", getpid());

        // char* args[] = {"ps", NULL};
        // if (execvp(args[0], args) < 0) {
        //     perror("execvp");
        //     exit(EXIT_FAILURE);
        // }

        char* env[] = {NULL};
        if (execle("/usr/bin/echo", "echo", "hi", NULL, env) < 0)  {
            perror("execle");
            exit(EXIT_FAILURE);
        }
    }
}

void q5(void) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(EXIT_FAILURE);
    }
    if (rc == 0) { // child 
        wait(NULL); // since this process has no child, wait has no effect at all
    }
    else { // parent
        printf("Hi, I'm parent. Child (pid: %d) just finished\n", wait(NULL));
    }
}

/**
 * Forks 2 times, and uses waitpid() for waiting the second child.
 * waitpid() is useful when there are more than 1 process, and we would like to 
 * specify which to wait
*/
void q6(void) {
    int p1 = fork();
    if (p1 < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(EXIT_FAILURE);
    }

    if (p1 > 0) { // parent
        int p2 = fork();
        if (p2 < 0) {
            fprintf(stderr, "Fork failed\n");
            exit(EXIT_FAILURE);
        }

        if (p2 > 0) { // parent
            waitpid(p2, NULL, 0);
            printf("Hey! I'm parent (pid: %d)\n", getpid());
            return;
        }
    }
    printf("Hey! I'm child (pid: %d)\n", getpid());
}

/**
 * Forks a child, closes `stdout` and tries to print to it.
*/
void q7(void) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(EXIT_FAILURE);
    }
    if (rc == 0) { // child
        close(STDOUT_FILENO);
        printf("This won't print\n");
    }
    else { // parent
        wait(NULL);
        printf("But this will\n");
    }
}

/**
 * Creates two children and sends messages between them using pipe()
*/
void q8(void) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int p1 = fork();
    if (p1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (p1 == 0) { // 1st child
        char msg[] = "Hello from p1\n";
        close(pipefd[0]); // close the read end of pipe
        write(pipefd[1], msg, sizeof(msg));
    }
    if (p1 > 0) { // parent
        int p2 = fork();
        if (p2 < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (p2 == 0) { // 2nd child
            close(pipefd[1]); // close the write end of pipe
            char buf[100];

            read(pipefd[0], buf, sizeof(buf));
            printf("p2 just received: %s", buf);

            close(pipefd[0]);
        }
        else { // parent
            // the parent doesn't meddle in the children communication, so we close the pipe
            close(pipefd[0]);
            close(pipefd[0]);

            wait(NULL);
            wait(NULL);
        }
    }
}

/**
 * Call the corresponding function here
 * 
 * You may as well create an array of function pointers (since they all have
 * the same type there won't be any need for typecasting) and call the corresponding
 * function depending on input
*/ 
int main(void) {
    q8();
}