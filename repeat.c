#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_usage() {
    printf("Usage: repeat <command> -n<number_of_repetitions>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        print_usage();
        return 1;
    }

    int repetitions = 0;
    char *command = NULL;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-n", 2) == 0) {
            repetitions = atoi(argv[i] + 2);
            if (repetitions <= 0) {
                fprintf(stderr, "Error: The number of repetitions must be a positive integer.\n");
                return 1;
            }
        } else {
            command = argv[i];
        }
    }

    if (repetitions == 0 || command == NULL) {
        print_usage();
        return 1;
    }

    for (int i = 0; i < repetitions; i++) {
        int status = system(command);
        if (status != 0) {
            fprintf(stderr, "Command failed with status %d on iteration %d\n", status, i + 1);
            return status;
        }
    }

    return 0;
}
