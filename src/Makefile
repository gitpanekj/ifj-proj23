CC=gcc
CFLAGS= -Wall -std=c99 -g
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
