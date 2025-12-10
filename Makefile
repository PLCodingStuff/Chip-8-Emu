CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
TARGET = chip8

SRCS = src/chip8.c src/display.h src/main.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

sdl_cflags := $(shell pkg-config --cflags sdl3)
sdl_libs := $(shell pkg-config --libs sdl3)
override CFLAGS += $(sdl_cflags)
override LIBS += $(sdl_libs)

display.o: display.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean