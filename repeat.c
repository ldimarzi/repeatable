#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage() {
    printf("Usage: repeat [-n <number_of_repetitions>] <command>\n");
    printf("Example: repeat -n 5 \"echo Hello\"\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage();
        return 1;
    }

    int repetitions = 0;
    char command[1024] = "";  // Buffer to hold the command

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-n", 2) == 0) {
            // Handle both "-n5" and "-n 5" formats
            if (strlen(argv[i]) > 2) {
                // Format is "-n5"
                repetitions = atoi(argv[i] + 2);
            } else if (i + 1 < argc) {
                // Format is "-n 5"
                repetitions = atoi(argv[i + 1]);
                i++;  // Skip the next argument since it's part of -n
            } else {
                fprintf(stderr, "Error: -n flag requires a number\n");
                return 1;
            }

            // Ensure repetitions is positive
            if (repetitions <= 0) {
                fprintf(stderr, "Error: The number of repetitions must be a positive integer.\n");
                return 1;
            }
        } else {
            // Append arguments to command string
            strcat(command, argv[i]);
            strcat(command, " ");
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

