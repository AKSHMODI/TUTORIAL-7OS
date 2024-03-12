#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// Define the structure for a process
struct proc {
    char name[256];
    int priority;
    int pid;
    int runtime;
};

// Define the structure for a linked list node
struct node {
    struct proc process;
    struct node *next;
};

// Define the queue structure
struct queue {
    struct node *front;
    struct node *rear;
};

// Function to initialize the queue
void init_queue(struct queue *q) {
    q->front = NULL;
    q->rear = NULL;
}

// Function to add a process to the queue
void push(struct queue *q, struct proc process) {
    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    new_node->process = process;
    new_node->next = NULL;

    if (q->rear == NULL) {
        q->front = new_node;
        q->rear = new_node;
    } else {
        q->rear->next = new_node;
        q->rear = new_node;
    }
}

// Function to print the contents of the queue
void print_queue(struct queue *q) {
    struct node *current = q->front;
    while (current != NULL) {
        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n",
               current->process.name, current->process.priority,
               current->process.pid, current->process.runtime);
        current = current->next;
    }
}

int main(void) {
    FILE *file = fopen("processes.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return EXIT_FAILURE;
    }

    struct queue q;
    init_queue(&q);

    // Read the contents of the file and add processes to the queue
    char name[256];
    int priority, pid, runtime;
    while (fscanf(file, "%255[^,],%d,%d,%d\n", name, &priority, &pid, &runtime) == 4) {
        struct proc new_proc;
        strcpy(new_proc.name, name);
        new_proc.priority = priority;
        new_proc.pid = pid;
        new_proc.runtime = runtime;
        push(&q, new_proc);
    }

    // Close the file
    fclose(file);

    // Print the contents of the queue
    printf("Contents of the queue:\n");
    print_queue(&q);

    return EXIT_SUCCESS;
}

