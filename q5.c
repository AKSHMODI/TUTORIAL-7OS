#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// Define the structure for a process
typedef struct Process {
    char name[256];
    int priority;
    pid_t pid;
    int runtime;
    struct Process *next;
} Process;

// Define the structure for the queue
typedef struct Queue {
    Process *front;
    Process *rear;
} Queue;

// Function prototypes
void push(Queue *queue, Process process);
Process pop(Queue *queue);
Process delete_name(Queue *queue, char *name);
void execute_process(Process process);

int main(void) {
    // Initialize the queue
    Queue queue;
    queue.front = queue.rear = NULL;

    // Open the file for reading
    FILE *file = fopen("processes_q5.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read processes from the file and add them to the queue
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Process process;
        sscanf(line, "%255s %d %d", process.name, &process.priority, &process.runtime);
        process.pid = 0; // Initialize pid to 0
        process.next = NULL;
        push(&queue, process);
    }

    // Close the file
    fclose(file);

    // Execute processes with priority 0 first
    Process current_process;
    while ((current_process = delete_name(&queue, "0")).pid != -1) {
        execute_process(current_process);
    }

    // Execute remaining processes in the queue
    while (queue.front != NULL) {
        current_process = pop(&queue);
        execute_process(current_process);
    }

    return EXIT_SUCCESS;
}

// Function to add a process to the queue
void push(Queue *queue, Process process) {
    Process *new_process = (Process *)malloc(sizeof(Process));
    if (new_process == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    *new_process = process;
    new_process->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = new_process;
    } else {
        queue->rear->next = new_process;
        queue->rear = new_process;
    }
}

// Function to remove and return the front process from the queue
Process pop(Queue *queue) {
    if (queue->front == NULL) {
        Process empty_process = {"", -1, -1, -1, NULL};
        return empty_process;
    }

    Process *temp = queue->front;
    Process popped_process = *temp;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    return popped_process;
}

// Function to delete a process by name from the queue
Process delete_name(Queue *queue, char *name) {
    if (queue->front == NULL) {
        Process empty_process = {"", -1, -1, -1, NULL};
        return empty_process;
    }

    if (strcmp(queue->front->name, name) == 0) {
        return pop(queue);
    }

    Process *current_process = queue->front->next;
    Process *prev_process = queue->front;

    while (current_process != NULL && strcmp(current_process->name, name) != 0) {
        prev_process = current_process;
        current_process = current_process->next;
    }

    if (current_process == NULL) {
        Process empty_process = {"", -1, -1, -1, NULL};
        return empty_process;
    }

    prev_process->next = current_process->next;
    if (queue->rear == current_process) {
        queue->rear = prev_process;
    }

    Process popped_process = *current_process;
    free(current_process);
    return popped_process;
}

// Function to execute a process
void execute_process(Process process) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        // Execute the process binary
        execl(process.name, process.name, NULL);
        perror("Exec failed");
        exit(EXIT_FAILURE);
    } else { // Parent process
        // Update the process ID in the structure
        process.pid = pid;

        // Sleep for the specified runtime
        sleep(process.runtime);

        // Send SIGINT to terminate the process
        kill(pid, SIGINT);

        // Wait for the child process to terminate
        int status;
        waitpid(pid, &status, 0);

        // Print process information
        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n", 
               process.name, process.priority, process.pid, process.runtime);
    }
}

