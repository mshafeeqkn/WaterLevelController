# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g -lpthread -lncurses

# Target executable name
TARGET = wlc_ui

# Source files
SRCS = main.c \
ncurses_gui.c \
user_input.c \
stm_interface.c \
backend.c

# Object files (compiled source files)
OBJS = $(SRCS:.c=.o)

# Default target (what gets built when running `make`)
all: $(TARGET)

# Rule for linking the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)

# Rule for compiling .c files into .o files (object files)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target (removes the executable and object files)
clean:
	rm -f $(TARGET) $(OBJS)

# PHONY is used to declare that 'all' and 'clean' are not files
.PHONY: all clean
