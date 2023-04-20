#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

void simple_fork(void) {
    printf("Hello World (pid: %d)\n", getpid());
    // creates an almost exact copy of the parent (and original) process
    int rc = fork();
    // difference: fork's return is 0 for the child, but for the parent it is the child's PID
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    if (rc == 0) {
        // fork gera um novo processo
        printf("Hello, I am child (pid: %d)\n", getpid());
    }
    else {
        printf("Hello, I am parent of %d (pid: %d)\n", rc, getpid());
    }
    /**
     * This function is non-deterministic: The CPU scheduler can prioritize executing either 
     * the parent or child process first. 
    */ 
}

void fork_and_wait(void) {
    printf("Hello World (pid: %d)\n", getpid());
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(1);
    }
    if (rc == 0) {
        printf("Hello, I am child (pid: %d)\n", getpid());
    }
    else {
        int rc_wait = wait(NULL); // Waits for a child to die and returns its PID
        printf("Hello, I am parent of %d (rc_wait:%d) (pid:%d)\n",
                rc, rc_wait, getpid());
    }
    /**
     * This function is deterministic: Even if the parent gets scheduled first, its first
     * instruction will be to wait.
    */
}

void execute_wc(void) {
    printf("Hello World (pid: %d)\n", getpid());
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "Fork failed.\n");
        exit(1);
    }
    if (rc == 0) { // child
        printf("Hello, I am child (pid: %d)\n", getpid());
        char* myargs[3];
        myargs[0] = "wc";           // program: wc
        myargs[1] = "forking.c";    // argument: file to count
        myargs[2] = NULL;           // indicates end of args
        // transforms child process into `wc forking.c`, so the following print never occurs
        execvp(myargs[0], myargs); 
        printf("This shouldn't print out\n");
    }
    else {
        int rc_wait = wait(NULL);
        printf("Hello, I am parent of %d (rc_wait: %d) (pid: %d)\n",
                rc, rc_wait, getpid());
    }
}

void wc_and_redirect(void) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(1);
    }
    if (rc == 0) { // child
        close(STDOUT_FILENO);
        open("./forking.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
        char* myargs[3];
        myargs[0] = "wc";
        myargs[1] = "forking.c";
        myargs[2] = 0;
        execvp(myargs[0], myargs);
    }
    else { // parent
        wait(NULL); 
    }
}

int main(int argc, char* argv[]) {
    // simple_fork();
    // fork_and_wait();
    // execute_wc();
    // wc_and_redirect();
}