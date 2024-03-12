#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

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

// Function to remove and return the next item from the queue
struct proc pop(struct queue *q) {
    if (q->front == NULL) {
        fprintf(stderr, "Queue is empty\n");
        exit(EXIT_FAILURE);
    }

    struct proc process = q->front->process;
    struct node *temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    return process;
}

// Function to delete a process from the queue by name
struct proc delete_name(struct queue *q, char *name) {
    struct node *current = q->front;
    struct node *prev = NULL;
    while (current != NULL) {
        if (strcmp(current->process.name, name) == 0) {
            if (prev == NULL) { // First node is the match
                return pop(q);
            } else {
                struct proc process = current->process;
                prev->next = current->next;
                free(current);
                return process;
            }
        }
        prev = current;
        current = current->next;
    }
    struct proc empty_proc = {"", 0, 0, 0};
    return empty_proc;
}

// Function to delete a process from the queue by pid
struct proc delete_pid(struct queue *q, int pid) {
    struct node *current = q->front;
    struct node *prev = NULL;
    while (current != NULL) {
        if (current->process.pid == pid) {
            if (prev == NULL) { // First node is the match
                return pop(q);
            } else {
                struct proc process = current->process;
                prev->next = current->next;
                free(current);
                return process;
            }
        }
        prev = current;
        current = current->next;
    }
    struct proc empty_proc = {"", 0, 0, 0};
    return empty_proc;
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

    // Delete the process named emacs
    printf("Deleting process named emacs:\n");
    delete_name(&q, "emacs");

    // Delete the process with the pid 12235
    printf("Deleting process with pid 12235:\n");
    delete_pid(&q, 12235);

    // Print the remaining processes in the queue
    printf("Remaining processes in the queue:\n");
    print_queue(&q);

    // Pop and print each process in the queue
    printf("Popping all processes in the queue:\n");
    while (q.front != NULL) {
        struct proc process = pop(&q);
        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n",
               process.name, process.priority, process.pid, process.runtime);
    }

    return EXIT_SUCCESS;
}

