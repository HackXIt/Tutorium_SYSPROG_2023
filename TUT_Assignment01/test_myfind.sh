# 
# File: test_myfind.sh
# Created on: Thursday, 2023-10-05 @ 19:21:03
# Author: HackXIt (<hackxit@gmail.com>)
# -----
# Last Modified: Thursday, 2023-10-05 @ 22:12:20
# Modified By:  HackXIt (<hackxit@gmail.com>) @ HACKXIT
# -----
# 
#!/bin/bash

# Initialize test case counter
count=1
target_executable=myfind
cwd=$(pwd)

if [ $# -eq 1 ]; then
    root_path=$1
else
    root_path=./build
fi

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

testpath_level1=testdir
testpath_level2=${testpath_level1}/subdir
testpath_level3=${testpath_level2}/subsubdir

# Create a temporary directory and files for testing
mkdir -p ${root_path}/${testpath_level1}
touch ${testpath_level1}/file1 ${testpath_level1}/file2 ${testpath_level2}/file3
cd ${root_path}

# Test 1: Verify myfind without any arguments starts at the current directory
run_test "./${target_executable}" "$(find $testpath_level1)" "Default starting point"

# Test 2: Verify starting point as an explicit argument
run_test "./${target_executable} testdir" "$(find testdir)" "Explicit starting point"

# Test 3: Test invalid starting point
run_test "./${target_executable} invalid_dir" "" "Invalid starting point"

# Test 4: Verify -print action
run_test "./${target_executable} testdir -print" "$(find testdir -print)" "-print action"

# Test 5: Verify output without any tests or actions
run_test "./${target_executable} testdir" "$(find testdir)" "No tests or actions"

# Test 6: Verify -name action for specific file
run_test "./${target_executable} testdir -name file1" "$(find testdir -name file1)" "-name action"

# Test 7: Verify -type action for directories
run_test "./${target_executable} testdir -type d" "$(find testdir -type d)" "-type action for directories"

# Test 8: Verify -type action for regular files
run_test "./${target_executable} testdir -type f" "$(find testdir -type f)" "-type action for files"

# Test 9: Verify -user action (replace username with an actual user on your system)
run_test "./${target_executable} testdir -user rini" "$(find testdir -user username)" "-user action"

# Clean up
rm -r ${testpath_level1}
cd ${cwd}

echo "Testing completed."