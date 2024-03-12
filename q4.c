#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
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

        // Send SIGTSTP signal to suspend the child process
        if (kill(pid, SIGTSTP) == -1) {
            perror("Kill SIGTSTP failed");
            exit(EXIT_FAILURE);
        }

        // Sleep for 10 seconds
        sleep(10);

        // Send SIGCONT signal to resume the child process
        if (kill(pid, SIGCONT) == -1) {
            perror("Kill SIGCONT failed");
            exit(EXIT_FAILURE);
        }

        // Wait for the child process to terminate
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Child process terminated with status %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child process terminated by signal %d\n", WTERMSIG(status));
        }
    }

    return EXIT_SUCCESS;
}

