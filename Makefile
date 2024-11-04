CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -o

all: repeat

repeat: repeat.c
	$(CC) $(CFLAGS) repeat repeat.c

clean:
	rm -f repeat
