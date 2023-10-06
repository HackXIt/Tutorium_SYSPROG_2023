# 
# File: test_myfind.sh
# Created on: Thursday, 2023-10-05 @ 19:21:03
# Author: HackXIt (<hackxit@gmail.com>)
# -----
# Last Modified: Friday, 2023-10-06 @ 21:17:51
# Modified By:  HackXIt (<hackxit@gmail.com>) @ dev-machine
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
mkdir -p ${root_path}/${testpath_level3}
cd ${root_path}
touch ${testpath_level1}/file1 ${testpath_level1}/file2 ${testpath_level2}/file3

# Test 1: Verify myfind without any arguments starts at the current directory
run_test "./${target_executable}" "$(find .)" "Default starting point"

# Test 2: Verify starting point as an explicit argument
run_test "./${target_executable} ${testpath_level1}" "$(find ${testpath_level1})" "Explicit starting point"

# Test 3: Test invalid starting point
run_test "./${target_executable} invalid_dir" "" "Invalid starting point"

# Test 4: Verify -print action
run_test "./${target_executable} ${testpath_level2} -print" "$(find ${testpath_level2} -print)" "-print action"

# Test 5: Verify output without any tests or actions
run_test "./${target_executable} ${testpath_level2}" "$(find ${testpath_level2})" "No tests or actions"

# Test 6: Verify -name action for specific file
run_test "./${target_executable} testdir -name file1" "$(find testdir -name file1)" "-name action"

# Test 7: Verify -name action for directories
run_test "./${target_executable} testdir -name subdir" "$(find testdir -name subdir)" "-name action for directories"

# Test 8: Verify -type action for directories
run_test "./${target_executable} testdir -type d" "$(find testdir -type d)" "-type action for directories"

# Test 9: Verify -type action for regular files
run_test "./${target_executable} testdir -type f" "$(find testdir -type f)" "-type action for files"

# Test 10: Verify -user action (replace username with an actual user on your system)
run_test "./${target_executable} testdir -user rini" "$(find testdir -user rini)" "-user action"

# Clean up
rm -r ${testpath_level1}
cd ${cwd}

echo "Testing completed."