CC=gcc
CFLAGS= -Wall -Wextra -Werror -std=c99 -pedantic
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
BIN=compiler.out


.PHONY: clean, run, test

all: $(BIN)


$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -r $(BIN) $(OBJS)

run: $(BIN)
	./$(BIN)
