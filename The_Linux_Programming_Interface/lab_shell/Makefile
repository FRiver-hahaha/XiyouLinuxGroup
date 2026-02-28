CC=cc
CFLAGS=-std=c11 -Wall -Wextra -O2 -g
TARGET=xxx-super-shell
SRCS=xxx-super-shell.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

