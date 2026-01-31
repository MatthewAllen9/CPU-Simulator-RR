#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Process: Holds pid, arrive, burst, remaining, first run and completion times
typedef struct{
    int pid;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int firstRunTime;
    int completionTime;
}Process;

//Read from input file and add each process to an array
Process *readProcesses(const char *filename, int *processCount);

//Queue: FIFO data structure to store Processes
typedef struct{
    int *pids;
    int size;
    int max;
}Queue;

//Functions for queue
Queue createQueue(int maxSize);
int isEmpty(Queue *q);
int isFull(Queue *q);
void enqueue(Queue *q, int pid);
int dequeue(Queue *q);






int main(int argc, char *argv[]) {
    //Check for correct command line structure
    if (argc < 2){
        printf("Command Line must be in the format: ./rr input.txt [quantum] [cSwitch]\n");
        return 1;
    }

    //Set quantim and context switch to defaults
    int quantum = 10;
    int cSwitch = 0;

    //Create pointer for parsing
    char *endptr;

    //Parse quantum and context switch if defined
    if (argc >= 3){
        quantum = (int)strtol(argv[2], &endptr, 10);
        if (*endptr != '\0'){
            printf("Invalid quantum.\n");
            exit(EXIT_FAILURE);
        }
    }
    if (argc >= 4){
        cSwitch = (int)strtol(argv[3], &endptr, 10);
        if (*endptr != '\0'){
            printf("Invalid context switch.\n");
            exit(EXIT_FAILURE);
        }
    }


    //Confirm valid command line arguements
    if (quantum <= 0){
        printf("Invalid quantum. Must be greater than 0\n");
        exit(EXIT_FAILURE);
    }
    if (cSwitch < 0){
        printf("Invalid context switch. Must be greater than or equal to 0\n");
        exit(EXIT_FAILURE);
    }


    //Create array of processes by reading them in
    int processCount = 0;
    Process *processes = readProcesses(argv[1], &processCount);

    //Print nothing for an empty file
    if (processCount == 0){
        free(processes);
        return 0;
    }


    //Create queue for ready processes
    Queue readyQ = createQueue(processCount);

    //Create variables to act as CPU
    int time = 0;
    int finished = 0;
    int next = 0;
    int current = -1;
    int qRemaining = 0;
    int lastPid = -1;
    int wasIdle = 1;

    //Create variables to store combined values for avgs
    double totalTurnaround = 0.0;
    double totalWaiting = 0.0;
    double totalResponse = 0.0;

    //RR ALGORITHM
    //Loop until all processes are complete
    while (finished < processCount){

        //Check if CPU is idle and ready queue is empty to time skip to the next arrival
        if (current == -1 && isEmpty(&readyQ)){
            //Check to ensure there is a next process
            if (next < processCount){
                if (time < processes[next].arrivalTime){
                    //Jump to next process arrival time
                    time = processes[next].arrivalTime;
                }
            }
            wasIdle = 1;
        }

        //Add all processes that have arrived
        while (next < processCount && processes[next].arrivalTime <= time){
            //Add to ready queue and increment to the next process
            enqueue(&readyQ, processes[next].pid);
            next++;
        }

        //Check if CPU is idle and there is a process ready
        if (current == -1 && !isEmpty(&readyQ)){
            //Set next pid to pid of the first process in the queue
            int nextPid = dequeue(&readyQ);

            //Check if the CPU was active and a new process is starting rather than the same
            if (!wasIdle && lastPid != -1 && nextPid != lastPid && cSwitch > 0){
                //Sim through the context switch and add new processes that arrive
                for (int k = 0; k < cSwitch; k++){
                    time++;

                    //Add new proceses to ready queue
                    while (next < processCount && processes[next].arrivalTime <= time){
                        enqueue(&readyQ, processes[next].pid);
                        next++;
                    }
                }
            }

            //Continue after context switch
            current = nextPid;
            qRemaining = quantum;
            wasIdle = 0;

           //Check if process is starting now
            if (processes[current].firstRunTime == -1){
                //Store first run time
                processes[current].firstRunTime = time;
            }
        }

        //Reset if idle again
        if (current == -1){
            continue;
        }

        //Move 1 tick
        processes[current].remainingTime--;
        qRemaining--;
        time++;

        //Add all processes that have arrived
        while (next < processCount && processes[next].arrivalTime <= time){
            //Add to ready queue and increment to the next process
            enqueue(&readyQ, processes[next].pid);
            next++;
        }

        //Check if current process is done or quantum remaining is finished
        if (processes[current].remainingTime == 0){
            //Store current time as completion time
            processes[current].completionTime = time;

            //Calculate process's stats
            int turnaround = processes[current].completionTime - processes[current].arrivalTime;
            int waiting = turnaround - processes[current].burstTime;
            int response = processes[current].firstRunTime - processes[current].arrivalTime;

            //Increment stats to later avg them
            totalTurnaround += turnaround;
            totalWaiting += waiting;
            totalResponse += response;

            //Increment finished processes and set CPU to idle
            finished++;
            lastPid = current;
            current = -1;
            wasIdle = 1;
        } 
        else if (qRemaining == 0){
            //Preempt and add process to the back of the queue
            enqueue(&readyQ, current);
            lastPid = current;
            current = -1;
        }
    }

    //Print avgs
    printf("%.2f %.2f %.2f\n", totalTurnaround / processCount, totalWaiting / processCount, totalResponse / processCount);

    //Free dynamically allocated memory
    free(processes);
    free(readyQ.pids);
    return 0;
}






//Read processes from input file and input the values into processes
Process *readProcesses(const char *filename, int *processCount){
    FILE *file = fopen(filename, "r");
    if (file == NULL){
        printf("Cannot open file.\n");
        exit(EXIT_FAILURE);
    }

    //Create variables to store initial max array size and process count
    int max = 10;
    int i = 0;

    //Dynamically allocate initial sized array
    Process *processes = malloc(sizeof(Process) * max);
    if (processes == NULL){
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    //Variable to store each read line
    char line[100];

    //Loop until end of file
    while (fgets(line, sizeof(line), file)){
        //Create variables to store text from 
        int arrival;
        int burst;

        //Only add to array if correct format (%d,%d)
        if (sscanf(line, "%d,%d", &arrival, &burst) == 2){

            //Double array size when max is reached
            if (i == max){
                max *= 2;
                processes = realloc(processes, sizeof(Process) * max);
                if (processes == NULL){
                    printf("Memory reallocation failed.\n");
                    exit(EXIT_FAILURE);
                }
            }

            //Store new process wih its initial values
            processes[i].pid = i;
            processes[i].arrivalTime = arrival;
            processes[i].burstTime = burst;
            processes[i].remainingTime = burst;
            processes[i].firstRunTime = -1;
            processes[i].completionTime = -1;

            //Increment count
            i++;
        }
    }

    //Close and return when end of file is reached
    fclose(file);
    *processCount = i;
    return processes;
}









//Create empty queue
Queue createQueue(int maxSize){
    Queue q;
    q.pids = malloc(sizeof(int) * maxSize);
    if (q.pids == NULL){
        printf("Queue memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    q.size = 0;
    q.max = maxSize;
    return q;
}

//Check if queue is empty
int isEmpty(Queue *q){
    return q->size == 0;
}


//Check if queue is full
int isFull(Queue *q){
    return q->size == q->max;
}


//Add process to end of queue
void enqueue(Queue *q, int pid){
    if (isFull(q)){
        printf("Queue is already full.\n");
        exit(EXIT_FAILURE);
    }

    q->pids[q->size] = pid;
    q->size++;
}


//Pop off first process
int dequeue(Queue *q){
    if (isEmpty(q)){
        printf("Queue is already empty\n");
        exit(EXIT_FAILURE);
    }

    int first = q->pids[0];

    //Shift all elements left
    for (int i = 1; i < q->size; i++){
        q->pids[i - 1] = q->pids[i];
    }

    q->size--;

    return first;
}
