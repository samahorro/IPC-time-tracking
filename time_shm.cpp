#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

void process(int argc, char *argv[]);

int isValidCommand(const char *cmd);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: ./time <command [args...]\n");

    return -1;
  }

  process(argc, argv);

  return 0;
}

void process(int argc, char *argv[]) {
  const char *valid_commands[] = {"dir", "help", "path", "echo",
                                  "ls",  "ping", "exit", NULL};

  
  key_t key = 1234; // Shared memory is established before child process
  int shm_id;
  struct timeval *start_time, *end_time;

  shm_id = shmget(key, sizeof(struct timeval), 0666 | IPC_CREAT); // creating shared memory segment

  if (shm_id < 0) {
    perror("shmget failed");
    exit(1);
  }

  start_time = (struct timeval *)shmat(shm_id, NULL, 0);
  if (start_time == (struct timeval *)-1) {
    perror("shmat failed");
    exit(1);
  }

  pid_t pid = fork();  // creates child process

  if (pid < 0) {  // if child process failed
    printf("ERROR\n");
    return;

  } else if (pid == 0) {  // beginning of child process

    gettimeofday(start_time, NULL);  // Captures start time of child process

    if (isValidCommand(argv[1])) {
      execvp(argv[1], argv + 1);
      perror("execvp failed");
      exit(1);
    } else if (!isValidCommand(argv[1])) {
      fprintf(stderr, "Invalid command: %s\n", argv[1]);
      return;
    }

  } else {  /*parent process (parent reads the starting time from shared memory)*/

    wait(NULL);

    start_time = (struct timeval *)shmat(shm_id, NULL, 0);  /*Start time is attached to shared memory so parent process can read the start time */
    if (start_time == (struct timeval *)-1) {
      perror("shmat failed");
      exit(1);
    }
    
    end_time = (struct timeval *)malloc( // Get the end time
        sizeof(struct timeval));  // Allocate memory for end_time
    if (end_time == NULL) {
      perror("Memory allocation for end_time failed");
      exit(1);
    }

    gettimeofday(end_time, NULL);

    long elapsed_sec = end_time->tv_sec - start_time->tv_sec;
    long elapsed_usec = end_time->tv_usec - start_time->tv_usec;

    // Prints the elapsed time in seconds and microseconds
    double elapsed_time_in_seconds = elapsed_sec + (elapsed_usec / 1000000.0);
    printf("Elapsed time: %.6f seconds\n", elapsed_time_in_seconds);

    // Detaches start time and removed from shared memory
    shmdt(start_time);
    shmctl(shm_id, IPC_RMID, NULL);
    free(end_time);  // allocates free memory for end_time
  }
}
int isValidCommand(const char *command) {
  const char *validCommands[] = {"ls",    "dir",  "echo", "ping",
                                 "clear", "exit", NULL};

  // Loop through the valid commands and check if the command exists in the list
  for (int i = 0; validCommands[i] != NULL; i++) {
    if (strcmp(validCommands[i], command) == 0) {
      return 1;
    }
  }

  return 0;
}
