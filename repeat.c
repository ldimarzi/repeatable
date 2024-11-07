#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> // For wait functions
#include <time.h>     // For time and timing functions
#include <errno.h>    // For error handling

void print_usage() {
    printf("Usage: repeat [options] <command>\n");
    printf("Options:\n");
    printf("  -n <count>    Number of repetitions (required)\n");
    printf("  -d <seconds>  Delay between repetitions (default: 0)\n");
    printf("  -v            Verbose output\n");
    printf("  -l <file>     Log errors to specified file\n");
    printf("  -u            Repeat until command succeeds\n");
    printf("  -t <seconds>  Timeout duration\n");
    printf("  -r <retries>  Maximum number of retries on failure\n");
    printf("Example: repeat -n 5 -d 1 -v -l errors.log -r 3 \"echo Hello\"\n");
}

int main(int argc, char *argv[]) {
    int repetitions = 0;
    int delay = 0;
    int verbose = 0;
    int until_success = 0;
    int timeout = 0;
    int max_retries = 0;  // New retry count option
    char command[1024] = "";  // Buffer to hold the command
    FILE *log_file = NULL;
    time_t start_time, current_time;

    // Parse options using getopt
    int opt;
    while ((opt = getopt(argc, argv, "n:d:vl:ut:r:")) != -1) {
        switch (opt) {
            case 'n':
                repetitions = atoi(optarg);
                if (repetitions <= 0) {
                    fprintf(stderr, "Error: The number of repetitions must be a positive integer.\n");
                    return 1;
                }
                break;
            case 'd':
                delay = atoi(optarg);
                if (delay < 0) {
                    fprintf(stderr, "Error: Delay must be a non-negative integer.\n");
                    return 1;
                }
                break;
            case 'v':
                verbose = 1;
                break;
            case 'l':
                log_file = fopen(optarg, "a");
                if (!log_file) {
                    perror("Error opening log file");
                    return 1;
                }
                break;
            case 'u':
                until_success = 1;
                break;
            case 't':
                timeout = atoi(optarg);
                if (timeout <= 0) {
                    fprintf(stderr, "Error: Timeout must be a positive integer.\n");
                    return 1;
                }
                break;
            case 'r':
                max_retries = atoi(optarg);
                if (max_retries < 0) {
                    fprintf(stderr, "Error: Retry count must be a non-negative integer.\n");
                    return 1;
                }
                break;
            default:
                print_usage();
                return 1;
        }
    }

    // Check that we have a command to execute after options
    if (optind >= argc) {
        print_usage();
        return 1;
    }

    // Construct the command string from remaining arguments
    for (int i = optind; i < argc; i++) {
        strcat(command, argv[i]);
        strcat(command, " ");
    }

    // Initialize timing for timeout check
    start_time = time(NULL);

    // Run the command for the specified number of repetitions or until success
    for (int i = 0; until_success || i < repetitions; i++) {
        int attempt = 0;
        while (1) {
            if (timeout > 0) {
                // Check if the timeout has been reached
                current_time = time(NULL);
                if (difftime(current_time, start_time) >= timeout) {
                    fprintf(stderr, "Timeout reached. Exiting...\n");
                    if (log_file) {
                        fprintf(log_file, "Timeout reached after %d repetitions.\n", i);
                    }
                    return 1;
                }
            }

            if (verbose) {
                printf("Executing iteration %d: %s\n", i + 1, command);
            }

            // Fork a new process to execute the command
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                return 1;
            } else if (pid == 0) {
                // Child process: execute the command using /bin/sh
                execl("/bin/sh", "sh", "-c", command, (char *) NULL);
                perror("execl"); // If execl fails
                exit(1);
            } else {
                // Parent process: wait for the child to complete
                int status;
                waitpid(pid, &status, 0);

                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    // Command succeeded
                    if (verbose) {
                        printf("Command succeeded on iteration %d\n", i + 1);
                    }
                    break; // Exit the retry loop
                } else {
                    // Command failed
                    if (verbose) {
                        fprintf(stderr, "Command failed with status %d on iteration %d, attempt %d\n", 
                                WEXITSTATUS(status), i + 1, attempt + 1);
                    }
                    if (log_file) {
                        fprintf(log_file, "Command failed with status %d on iteration %d, attempt %d\n", 
                                WEXITSTATUS(status), i + 1, attempt + 1);
                    }

                    attempt++;
                    if (attempt >= max_retries) {
                        fprintf(stderr, "Max retries reached for iteration %d. Moving to next iteration.\n", i + 1);
                        break; // Move to the next iteration
                    }
                }
            }

            // Delay if specified before retrying
            if (delay > 0) {
                sleep(delay);
            }
        }

        // Delay if specified between repetitions
        if (delay > 0) {
            sleep(delay);
        }
    }

    // Cleanup
    if (log_file) {
        fclose(log_file);
    }

    return 0;
}

