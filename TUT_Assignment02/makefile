ROOT=/home/rini/git-stash/Tutorium_SYSPROG_2023/
C=gcc
INC=-iquote${ROOT}/TUT_Assignment02/inc
CFLAGS=-Wall -Wextra -pedantic -g -Wno-unused-parameter
POPENTEST=-lpopentest -ldl
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
${BUILD}%.o: %.c %.h
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Automatic variable for object files without headers
# MUST be after the automatic variable above
${BUILD}%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Here are my targets

all: pipetest

pipetest: pipetest.c $(BUILD)mypopen.o
	$(CC) $(CFLAGS) $(INC) $^ -o $(BUILD)$@

clean:
	rm $(BUILD)*.o $(BUILD)pipetest

popentest: $(BUILD)mypopen.o
	$(CC) $(CFLAGS) $^ -o $(BUILD)$@ $(POPENTEST)

instlib:
	sudo cp ./inc/*.a /usr/local/lib

.PHONY: clean instlib

# TODO Doxygen Target