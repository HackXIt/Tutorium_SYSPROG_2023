//
// Created by rini on 29.09.23.
//

#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void traverse_directory(const char *dir_path);

int main(int argc, char *argv[]) {
    char *starting_point;

    // Step 1: Command-Line Parsing & Validation
    if (argc < 2) {
        starting_point = ".";
    } else {
        struct stat sb;
        if (stat(argv[1], &sb) == 0 && S_ISDIR(sb.st_mode)) {
            starting_point = argv[1];
        } else {
            printf("Invalid directory: %s\n", argv[1]);
            return EXIT_FAILURE;
        }
    }

    // Step 2: Iterate Through the Directory Tree
    traverse_directory(starting_point);

    return EXIT_SUCCESS;
}

void traverse_directory(const char *dir_path) {
    struct dirent *entry;
    DIR *dir = opendir(dir_path);

    if (dir == NULL) {
        printf("Failed to open directory: %s\n", dir_path);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("%s/%s\n", dir_path, entry->d_name);

            struct stat sb;
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

            if (stat(full_path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
                traverse_directory(full_path);
            }
        }
    }

    closedir(dir);
}
