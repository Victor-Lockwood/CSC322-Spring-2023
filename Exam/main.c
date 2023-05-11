#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

//Problem 4 - Part I

/*
int counter = 0;

int main() {
    int i;

    for(i=0; i<2; i++) {
        fork();
        counter++;
        printf("counter = %d\n", counter);
    }

    printf("counter = %d\n", counter);
    return 0;
}
 */

// -----------------------

//Part II

/*
pid_t pid;
int counter = 0;

void handler1(int sig) {
    counter++;
    printf("counter = %d\n", counter);
    fflush(stdout);
    kill(pid, SIGUSR1);
}

void handler2(int sig) {
    counter+=3;
    printf("counter = %d\n", counter);
    exit(0);
}

int main() {
    signal(SIGUSR1, handler1);

    if((pid = fork()) == 0) {
        signal(SIGUSR1, handler2);
        kill(getppid(), SIGUSR1);
        while(1) {};
    } else {
        pid_t p; int status;

        if((p = wait(&status)) > 0) {
            counter += 2;
            printf("counter = %d\n", counter);
        }
    }

    //return 0;
}
 */


// -----------------------

//Part III

int counter = 0;

void handler(int sig) {
    counter++;
}

int main() {
    int i;
    signal(SIGCHLD, handler);

    for(i = 0; i<5; i++) {
        if(fork() == 0) {
            exit(0);
        }
    }

    while(wait(NULL) != -1);

    printf("counter = %d\n", counter);
    return 0;
}