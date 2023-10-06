ROOT=/home/rini/git-stash/Tutorium_SYSPROG_2023/
C=gcc
INC=-iquote$(ROOT)/TUT_Assignment03/inc
LIBS=-lpthread -lrt
CFLAGS=-Wall -Wextra -pedantic -g -Wno-unused-parameter
# CFLAGS=-g -Wno-unused-parameter
BUILD=./build/
# VPATH:=src:build
vpath %.o ./build
vpath %.c ./src
vpath %.h ./inc

# This is required for source-files without header-files
#$(BUILD)hashcount.o: hashcount.c
#	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Automatic variable for object files
$(BUILD)%.o: %.c %.h
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Automatic variable for object files without headers
# MUST be after the automatic variable above
$(BUILD)%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Here are my targets

all: sender receiver

# Required project target
sender: sender.c $(BUILD)ipc.o
	$(CC) $(CFLAGS) $(INC) $^ -o $(BUILD)$@ $(LIBS)

# Required project target
receiver: receiver.c ipc.o
	$(CC) $(CFLAGS) $(INC) $^ -o $(BUILD)$@ $(LIBS)

clean:
	rm $(BUILD)*.o $(BUILD)pipetest

.PHONY: clean

# TODO Doxygen Target