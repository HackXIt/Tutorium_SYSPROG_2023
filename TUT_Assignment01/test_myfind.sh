#!/bin/bash

# Initialize test case counter
count=1

# Utility function to test the program
run_test() {
    command=$1
    expected=$2
    test_case=$3

    output=$(eval "$command")

    if [ "$output" == "$expected" ]; then
        printf "\nTest case $count ($test_case): Passed\n"
    else
        printf "\nTest case $count ($test_case): Failed\n"
        printf "\nExpected:\n$expected"
        printf "\nGot:\n$output"
    fi
    printf "\n==========================================================\n"
    ((count++))
}

# Create a temporary directory and files for testing
mkdir -p testdir/subdir
touch testdir/file1 testdir/file2 testdir/subdir/file3

# Test 1: Verify myfind without any arguments starts at the current directory
run_test "./myfind" "$(find .)" "Default starting point"

# Test 2: Verify starting point as an explicit argument
run_test "./myfind testdir" "$(find testdir)" "Explicit starting point"

# Test 3: Test invalid starting point
run_test "./myfind invalid_dir" "" "Invalid starting point"

# Test 4: Verify -print action
run_test "./myfind testdir -print" "$(find testdir -print)" "-print action"

# Test 5: Verify output without any tests or actions
run_test "./myfind testdir" "$(find testdir)" "No tests or actions"

# Test 6: Verify -name action for specific file
run_test "./myfind testdir -name file1" "$(find testdir -name file1)" "-name action"

# Test 7: Verify -type action for directories
run_test "./myfind testdir -type d" "$(find testdir -type d)" "-type action for directories"

# Test 8: Verify -type action for regular files
run_test "./myfind testdir -type f" "$(find testdir -type f)" "-type action for files"

# Test 9: Verify -user action (replace username with an actual user on your system)
run_test "./myfind testdir -user rini" "$(find testdir -user username)" "-user action"

# Clean up
rm -r testdir

echo "Testing completed."