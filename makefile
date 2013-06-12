# define the compiler
CC = gcc

# define compiler flags
CFLAGS = -I /usr/include/PCSC -l pcsclite

# define the script name
TARGET = sample

all: $(TARGET).c
	$(CC) $(TARGET).c $(CFLAGS) -o $(TARGET).out

clean:
	$(RM) $(TARGET).out