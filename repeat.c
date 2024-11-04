#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage() {
    printf("Usage: repeat <command> -n <number_of_repetitions>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        print_usage();
        return 1;
    }

    int repetitions = 0;
    char command[1024] = "";  // Buffer to hold the command

    // Parse arguments
    int i = 1;
    while (i < argc) {
        // Check for the "-n" flag
        if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc) {
                repetitions = atoi(argv[i + 1]);
                if (repetitions <= 0) {
                    fprintf(stderr, "Error: The number of repetitions must be a positive integer.\n");
                    return 1;
                }
                i += 2; // Skip the "-n" and the number after it
            } else {
                print_usage();
                return 1;
            }
        } else {
            // Append the command arguments to form the command string
            strcat(command, argv[i]);
            strcat(command, " ");
            i++;
        }
    }

    if (repetitions == 0 || strlen(command) == 0) {
        print_usage();
        return 1;
    }

    // Execute the command N times
    for (int j = 0; j < repetitions; j++) {
        int status = system(command);
        if (status != 0) {
            fprintf(stderr, "Command failed with status %d on iteration %d\n", status, j + 1);
            return status;
        }
    }

    return 0;
}

