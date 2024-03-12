#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        execl("./process", "process", NULL);
        perror("Exec failed");
        exit(EXIT_FAILURE);
    } else { // Parent process
        // Sleep for 5 seconds
        sleep(5);

        // Send SIGINT signal to the child process
        if (kill(pid, SIGINT) == -1) {
            perror("Kill failed");
            exit(EXIT_FAILURE);
        }

        // Wait for the child process to terminate
        wait(NULL);
    }

    return EXIT_SUCCESS;
}

