TARGET = prog
LIBS = -lm
CC = gcc
OO = -O3
WARN = -Wall
PROFILE = -pg --static -g3
CFLAGS = $(WARN)

.PHONY: default all clean

default: $(TARGET)
all: default
d: CFLAGS += -g -DDEBUG
d: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
	-rm -f *.dat *.png
