#include <stdio.h>
#include <stdlib.h> // Include for malloc
#include <stdbool.h>

typedef struct {
    int processId;
    int arrivalTime;
    int burstTime;
    int remainingTime; // Added remaining time
    int priority;
    int waitingTime;
    int turnaroundTime;
    bool isCompleted;
} Process;

// Stack structure
typedef struct {
    Process *stackArray;
    int top;
    int maxSize;
} Stack;

// Function to create a stack
Stack* createStack(int size) {
    Stack *stack = (Stack*)malloc(sizeof(Stack));
    stack->maxSize = size;
    stack->top = -1;
    stack->stackArray = (Process*)malloc(size * sizeof(Process));
    return stack;
}

// Function to check if the stack is full
bool isFull(Stack *stack) {
    return stack->top == stack->maxSize - 1;
}

// Function to check if the stack is empty
bool isEmpty(Stack *stack) {
    return stack->top == -1;
}

// Function to push an element onto the stack
void push(Stack *stack, Process p) {
    if (!isFull(stack)) {
        stack->stackArray[++stack->top] = p;
    }
}

// Function to pop an element from the stack
Process pop(Stack *stack) {
    if (!isEmpty(stack)) {
        return stack->stackArray[stack->top--];
    }
    Process emptyProcess = {0};
    return emptyProcess; // Return an empty process if the stack is empty
}

void roundRobin(Process processes[], int n, int quantum) {
    int currentTime = 0;
    Stack *readyStack1 = createStack(n);
    Stack *readyStack2 = createStack(n);

    // Initialize remaining times for Round Robin
    for (int i = 0; i < n; i++) {
        processes[i].remainingTime = processes[i].burstTime;
    }

    bool allCompleted = false;

    while (!allCompleted) {
        allCompleted = true; // Assume all are completed

        // Push processes to the first stack based on arrival time
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0) {
                push(readyStack1, processes[i]);
            }
        }

        // Process the first stack
        while (!isEmpty(readyStack1)) {
            Process p = pop(readyStack1);

            if (p.remainingTime > 0) {
                allCompleted = false; // At least one process is not completed

                int timeSlice = (p.remainingTime > quantum) ? quantum : p.remainingTime;
                p.remainingTime -= timeSlice;
                currentTime += timeSlice;

                // Push the process to the second stack if it's not completed
                if (p.remainingTime > 0) {
                    push(readyStack2, p);
                } else {
                    // Calculate waiting and turnaround time for completed process
                    p.turnaroundTime = currentTime - p.arrivalTime;
                    p.waitingTime = p.turnaroundTime - p.burstTime;
                    processes[p.processId - 1] = p; // Update the original process array
                }
            }
        }

        // Move processes from the second stack back to the first stack
        while (!isEmpty(readyStack2)) {
            Process p = pop(readyStack2);
            push(readyStack1, p);
        }

        // Increment time if no processes are ready
        if (allCompleted) {
            currentTime++;
        }
    }

    free(readyStack1->stackArray);
    free(readyStack1);
    free(readyStack2->stackArray);
    free(readyStack2);
}

void merge(Process arr[], int left, int middle, int right) {
    int n1 = middle - left + 1;
    int n2 = right - middle;

    Process *L = (Process *)malloc(n1 * sizeof(Process));
    Process *R = (Process *)malloc(n2 * sizeof(Process));

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[middle + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].arrivalTime <= R[j].arrivalTime) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void mergeSort(Process arr[], int left, int right) {
    if (left < right) {
        int middle = left + (right - left) / 2;
        mergeSort(arr, left, middle);
        mergeSort(arr, middle + 1, right);
        merge(arr, left, middle, right);
    }
}

void fcfs(Process processes[], int n) {
    mergeSort(processes, 0, n - 1); // Sort by arrival time
    processes[0].waitingTime = 0;
    processes[0].turnaroundTime = processes[0].burstTime;

    for (int i = 1; i < n; i++) {
        processes[i].waitingTime = processes[i - 1].waitingTime + processes[i - 1].burstTime;
        processes[i].turnaroundTime = processes[i].waitingTime + processes[i].burstTime;
    }
}

void priorityScheduling(Process processes[], int n) {
    int currentTime = 0;

    for (int count = 0; count < n; count++) {
        // Find process with the highest priority that has arrived
        int highestPriorityIndex = -1;
        for (int i = 0; i < n; i++) {
            if (!processes[i].isCompleted && processes[i].arrivalTime <= currentTime &&
                (highestPriorityIndex == -1 || processes[i].priority < processes[highestPriorityIndex].priority)) {
                highestPriorityIndex = i;
            }
        }

        if (highestPriorityIndex != -1) {
            Process *p = &processes[highestPriorityIndex];
            p->waitingTime = currentTime - p->arrivalTime;
            p->turnaroundTime = p->waitingTime + p->burstTime;
            p->isCompleted = true;
            currentTime += p->burstTime; // Move forward in time after this process completes

            // Adjust priorities for other processes (simulate aging)
            for (int j = 0; j < n; j++) {
                if (!processes[j].isCompleted && processes[j].arrivalTime <= currentTime) {
                    processes[j].priority -= 1; // Decrease priority for waiting processes
                }
            }
        } else {
            // If no process is ready, increment time
            currentTime++;
        }
    }
}

void printResults(Process processes[], int n) {
    for (int i = 0; i < n; i++) {
        printf("Process %d: Waiting Time = %d, Turnaround Time = %d, Priority = %d\n",
               processes[i].processId, processes[i].waitingTime, processes[i].turnaroundTime, processes[i].priority);
    }
}

int main() {
    Process processes[] = {
            {1, 0, 5, 3, 0, 0, false},
            {2, 1, 3, 1, 0, 0, false},
            {3, 2, 8, 2, 0, 0, false},
            {4, 3, 6, 4, 0, 0, false}
    };
    int n = sizeof(processes) / sizeof(processes[0]);
    int choice;

    while (1) {
        printf("\nChoose a scheduling algorithm:\n");
        printf("1. First-Come, First-Served (FCFS)\n");
        printf("2. Priority Scheduling\n");
        printf("3. Round Robin Scheduling\n");
        printf("4. Exit\n");
        printf("Enter your choice (1, 2, 3, or 4): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("\nFCFS Scheduling Results:\n");
                fcfs(processes, n);
                printResults(processes, n);
                break;
            case 2:
                printf("\nPriority Scheduling Results:\n");
                priorityScheduling(processes, n);
                printResults(processes, n);
                break;
            case 3: {
                int quantum;
                printf("Enter time quantum for Round Robin: ");
                scanf("%d", &quantum);
                printf("\nRound Robin Scheduling Results:\n");
                roundRobin(processes, n, quantum);
                printResults(processes, n);
                break;
            }
            case 4:
                printf("Exiting the program. Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Please enter 1, 2, 3, or 4.\n");
        }
    }

    return 0;