# Repeat Command Utility

`repeat` is a Linux command-line utility written in C that allows users to execute a specified command multiple times in succession. It supports options for delay, verbose output, logging errors, retry attempts on failure, timeout, and repeating until a command succeeds.

---

## Features

- **Run commands multiple times**: Execute a command N times with a single command.
- **Delay between executions**: Specify a delay between each run.
- **Logging**: Log errors to a specified file.
- **Retries**: Retry a failed command up to a specified number of times.
- **Timeout**: Set a maximum time for the utility to run.
- **Verbose Output**: Display details of each execution attempt.

---

## Compilation

To compile the program, use the following command:

```bash
gcc -o repeat repeat.c

