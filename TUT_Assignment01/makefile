ROOT=/home/rini-debian/git-stash/Tutorium_SYSPROG_2023/
C=gcc
INC=-iquote${ROOT}/TUT_Assignment01/inc
CFLAGS=-Wall -Wextra -pedantic -g -Wno-unused-parameter
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

# Here are my targets

all: myfind

myfind: main.c
	$(CC) $(CFLAGS) $^ -o $(BUILD)myfind

clean:
	rm $(BUILD)*.o $(BUILD)myfind

test: myfind
	./test_myfind.sh

.PHONY: clean

# TODO Doxygen Target