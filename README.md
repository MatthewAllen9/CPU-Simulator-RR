## CIS3110 A1 CPU Simulator Round Robin
## Matthew Allen
## 1313528 mallen31


## Description:

This program simulates a CPU's ability to schedule processes with unique arrival and burst times. The program uses a round robin algorithm with a quantum to simulate through processes. The processes are read in from a .txt file in the command line. The processes must be formatted as, '<arrivalTime,burstTime>'. The user also has the option to choose a unique quantum or context switch of their choice in the command line. If no quantums or context switches are defined, the program will run with a default quantum of 10 clock cycles and no context switch. When processes arrive, they are added to a ready queue which is optimized to hold only the pids of each process and to be circular to allow for faster time complexiy. The simulator then decrements the active process by the quantum. If the process is completed before the quantum expires it is listed as finshed, however, if it is not, the process is then moved back to the end of the ready queue. The program also tracks the total turnaround, waiting and response time to display an average of those 3 respectivly in the format, '<avg_turnaround_time> <avg_waiting_time> <avg_response_time>'. The context switch feature is also included to be more accurate to a true CPU by adding additional overhead time like a real CPU. 

## Usage:

Compilation: make  
Run Code: ./rr [inputFile].txt [quantum] [contextSwitch] (quantum and conext switch are optional)

## Output

<avg_turnaround_time> <avg_waiting_time> <avg_response_time>

## Example Input/Output

input.txt:  
0,15  
5,5  
20,10  

./rr input.txt  
13.33 3.33 1.67

./rr input.txt 1  
13.00 3.00 0.00

./rr input.txt 5 5  
30.00 20.00 5.00

./rr input.txt 10 5  
23.33 13.33 8.33
