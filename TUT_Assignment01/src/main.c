//
// Created by rini on 29.09.23.
//

// Imports
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

/*
typedef struct options {
    bool print;
    bool ls;
    bool user;
    char username;      // <name> for -user
    int uid;            // <uid> for -user
    bool name;
    char *pattern;      // <pattern> for -name
    bool type;
    char type_c;        // <t> for -type
} options_t;
*/
typedef struct option {
    char *option;
    char *parameter;
    struct option *next;
} option_t;

typedef struct option_list {
    int options;
    option_t *first;
    option_t *last;
} option_list_t;
//#define MAX_FULL_PATH   1024



// Function prototypes
void traverse_directory(const char *dir_path, option_list_t * options);
options_t parse_options(char *argv[], option_list_t * options);
void write_permissions(mode_t fileMode);

int main(int argc, char *argv[]) {
    char *starting_point = ".";
    option_list_t options = {.options = 0, .first = NULL, .last = NULL};

    // Step 1: Parsing & Validation
    parse_options(argv, &options);

    // Step 2: Iterating recursively
    // 2nd statement in the if won't be evaluated when no parameters were provided, so it is safe to access
    if(argc > 2 && argv[1][0] != '-') { // when 1st parameter isn't an option, use provided starting directory
        traverse_directory(argv[1], &options);
    } else { // otherwise use default current working directory
        traverse_directory(starting_point, &options);
    }

    // Program done
    return EXIT_SUCCESS;
}

// Main functionality

int traverse_directory(const char *dir_path, option_list_t * options) {
    struct dirent *entry;
    DIR *dir = opendir(dir_path);

    // Error handling von opendir()
    if(dir == NULL) {
        fprintf(stderr, "Failed to open directory: %s\n", dir_path);
        return;
    }

    //printf("%s\n", dir_path); // for "." and ".."
    while ((entry = readdir(dir)) != NULL) {
        /*
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            //
            //printf("%s\n", entry->d_name);
            break;
        }
        */
        option_t *current = options->first;
        while(current != NULL) {

        }

        struct stat sb;
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        if(stat(full_path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
            traverse_directory(full_path, options);
        }
    }
}

// Argument parsing

options_t parse_options(char *argv[], option_list_t * options) {
    char **current = argv;

    while(*current != NULL) {
        if(strcmp(*current, "-print") == 0) {
            option_t *print = (option_t*)calloc(sizeof(option_t), 1);
            if(print == NULL) {
                fprintf(stderr, "Out of memory!");
            }
            print->option = calloc(sizeof(char), strlen("-print")+1);
            strcpy(print->option, "-print");
            options.
        } else if(strcmp(*current, "-ls") == 0) {
            options.ls = true;
        } else if(strcmp(*current, "-user") == 0) {
            options.user = true;
        } else if(strcmp(*current, "-name") == 0) {
            // TODO handle <pattern> after name
            options.name = true;
        } else if(strcmp(*current, "-type") == 0) {
            // TODO handle <t> after type
            options.type = true;
        } else if(strcmp(*current, "-user") == 0) {
            // TODO handle <name>|<uid> after user
            options.user = true;
        }
        current++;
    }

    return options;
}

void print_default(char *path, char *entry_name) {
    printf("%s/%s\n", path, entry_name);
}

// Option handlers

void print_print(struct stat sb, const char *file_path) {
    printf("%s\n", file_path);
}

void print_ls(struct stat sb, const char *file_path) {
    // Step 1: Print the inode number
    printf("%ld ", (long)sb.st_ino);

    // Step 2: Print the number of used blocks
    printf("%ld ", (long)sb.st_blocks);

    // Step 3: Print the file permissions
    write_permissions(sb.st_mode);

    // Step 4: Print number of links
    printf("%ld ", (long)sb.st_nlink);

    // Step 5: Print owner and group
    struct passwd *pw = getpwuid(sb.st_uid);
    if (pw != NULL) {
        printf("%s ", pw->pw_name);
        struct group *g = getgrgid(pw->pw_gid);
        if(g != NULL) {
            printf("%s ", g->gr_name);
        } else {
            // TODO error handling if gid not found
        }
    } else {
        // TODO error handling if uid not found
    }

    // Step 6: Print file size
    printf("%lld ", (long long)sb.st_size);

    // Step 7: Print modification time
    // Convert time_t to string using strftime() here
    char timestr[128];
    time_t t = timespec_get(&sb.st_mtim, time(0));
    strftime(timestr, sizeof(timestr), "%b %d %k:%M", &t);
    printf("%s ", timestr);

    // Step 8: Print file name
    printf("%s\n", file_path);
}

void print_name(char *name, char *pattern) {
    // TODO do file mat
}

// Helper functions

void write_permissions(mode_t mode) {
    // the '?' is a ternary operator => short way of writing if-else
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}