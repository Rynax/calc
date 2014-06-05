CC = gcc
LD = gcc
CFLAGS = -Wall -c -g
LDFLAGS = 

SRCS = $(wildcard *.c source/*.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))
TARGET = calc

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%o: %c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o $(TARGET)
