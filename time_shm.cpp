// Samantha Ahorro
// time_shm.cpp: Shared Memory IPC Model

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

int isValidCommand(const char *cmd);

void process(int argc, char *argv[]) {
  const char *valid_commands[] = {
      "dir"
      "path",
      "echo",
      "ls",
      "ping",
      "exit",
      NULL};

  // Shared memory identifier
  key_t key = 1234;
  int shm_id;
  struct timeval *start_time, *end_time;

  // Created a shared memory segment for struct time interval
  shm_id = shmget(key, sizeof(struct timeval), 0666 | IPC_CREAT);
  if (shm_id < 0) {
    perror("shmget failed");
    exit(1);
  }
 
  // Attaching shared memory to process
  start_time = (struct timeval *)shmat(shm_id, NULL, 0);
  if (start_time == (struct timeval *)-1) {
    perror("shmat failed");
    exit(1);
  }

  // Create a child process
  pid_t pid = fork();

  // If fork process fails
  if (pid < 0) {
    printf("ERROR: Fork failed.\n");
    exit(1);

    // Beginning of child process
  } else if (pid == 0) {
    // Captures start time of child process
    gettimeofday(start_time, NULL);

    // Executes user command if valid
    if (isValidCommand(argv[1])) {
      execvp(argv[1], argv + 1);
      perror("execvp failed");
      exit(1);
    } else if (!isValidCommand(argv[1])) {
      fprintf(stderr, "Invalid command: %s\n", argv[1]);
      exit(1);
    }

  } else {
    /*Parent Process (parent reads the start_time from shared memory)*/
    wait(NULL);

    if (start_time == (struct timeval *)-1) {
      perror("shmat failed");
      exit(1);
    }

    // Declaring the end_time & allocating memory for end_time
    end_time = (struct timeval *)malloc(sizeof(struct timeval));
    if (end_time == NULL) {
      perror("Memory allocation for end_time failed");
      exit(1);
    }

    // Capture end_time
    gettimeofday(end_time, NULL);

    // Calculates the elapsed time from start_time & end_time
    long elapsed_sec = end_time->tv_sec - start_time->tv_sec;
    long elapsed_usec = end_time->tv_usec - start_time->tv_usec;
    double elapsed_time_in_seconds = elapsed_sec + (elapsed_usec / 1000000.0);
    printf("Elapsed time: %.6f seconds\n", elapsed_time_in_seconds);

    // Detaches start time and removed from shared memory
    shmdt(start_time);
    shmctl(shm_id, IPC_RMID, NULL);
    free(end_time);
  }
}

int isValidCommand(const char *command) {
  const char *validCommands[] = {"dir",  "path", "echo", "ls",
                                 "ping", "exit", NULL};

  // Loop through the valid commands and checks if the command exists in the list
  for (int i = 0; validCommands[i] != NULL; i++) {
    if (strcmp(validCommands[i], command) == 0) {
      return 1;
    }
  }

  return 0;
}

// Main Function:

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: ./time <command> [args...]\n");

    return -1;
  }

  process(argc, argv);

  return 0;
}
