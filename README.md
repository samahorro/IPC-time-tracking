# IPC-time-tracking

Time Measurement for Command Execution Using IPC

Two C++ programs that measure the time taken to execute a specified command from the command line using fork(), execvp(), and gettimeofday(). The programs utilize two different inter-process communication (IPC) mechanisms to share the starting time between the parent and child processes.

time_shm.c: This version uses shared memory to share the starting time between the parent and child processes. The child process writes the starting time to a shared memory region before executing the command. The parent process reads the starting time from shared memory after the child process terminates, calculates the elapsed time, and prints the result.

time_pipe.c: This version uses a pipe for IPC. The child process writes the starting time to the pipe, and the parent process reads the starting time from the pipe after the child process terminates. The elapsed time is then calculated and displayed.

Both programs aim to measure the execution time of commands and report the elapsed time in seconds and microseconds. The output provides a detailed timestamp, and the elapsed time is displayed in a human-readable format.

Files included:

time_shm.cpp: Program using shared memory for IPC.
time_pipe.cpp: Program using pipes for IPC.

