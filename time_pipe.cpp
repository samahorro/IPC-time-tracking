// Samantha Ahorro
// time_pipe.cpp: Pipe IPC Model

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

int isValidCommand(const char *command);

void pipeProcess(int argc, char *argv[]) {
  const char *valid_commands[] = {"dir", "echo", "ls", "ping", "exit", NULL};

  // Structure to hold values of start_time and end_time
  struct timeval start_time, end_time;

  // Create File Descriptor to pass into "pipe"
  int fd[2];
  pipe(fd);

  // Create a child process
  pid_t pid = fork();

  if (pid < 0) {
    fprintf(stderr, "The fork failed!");
    exit(1);

    // Child process
  } else if (pid == 0) {
    /*Close unused read end*/
    close(fd[0]);

    /*Captures start time*/
    gettimeofday(&start_time, NULL);

    /*Writes the start_time through pipe (sent to parent)*/
    write(fd[1], &start_time, sizeof(struct timeval));

    /*Executes useres command if valid*/
    if (isValidCommand(argv[1])) {
      execvp(argv[1], argv + 1);
      perror("execvp failed");
      exit(1);
    } else if (!isValidCommand(argv[1])) {
      fprintf(stderr, "Invalid command: %s\n", argv[1]);
      return;
    }

    /*Close write-end*/
    close(fd[1]);

    // Parent process
  } else if (pid > 0) {
    wait(NULL);

    /*Closes unused write-end*/
    close(fd[1]);

    /*Reads the start time from pipe (sent from child)*/
    read(fd[0], &start_time, sizeof(struct timeval));
    /*Closes read-end*/
    close(fd[0]);

    /*Captures end_time*/
    gettimeofday(&end_time, NULL);

    /*Calculates Elapsed Time*/
    long elapsed_sec = end_time.tv_sec - start_time.tv_sec;
    long elapsed_usec = end_time.tv_usec - start_time.tv_usec;
    double elapsed_time_in_seconds = elapsed_sec + (elapsed_usec / 1000000.0);

    printf("Elapsed time: %.6f seconds\n", elapsed_time_in_seconds);
  }
}

int isValidCommand(const char *command) {
  const char *validCommands[] = {"ls",    "dir",  "echo", "ping",
                                 "clear", "exit", NULL};

  for (int i = 0; validCommands[i] != NULL; i++) {
    if (strcmp(validCommands[i], command) == 0) {
      return 1;
    }
  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: ./time <command> [args...]\n");

    return -1;
  }

  pipeProcess(argc, argv);

  return 0;
}
