/*
 * File: main.c
 * Created on: Friday, 2023-10-06 @ 14:28:34
 * Author: HackXIt (<hackxit@gmail.com>)
 * -----
 * Last Modified: Friday, 2023-10-06 @ 22:07:53
 * Modified By:  HackXIt (<hackxit@gmail.com>) @ dev-machine
 * ----- About the code -----
 * Purpose:
 *
 * Example call:
 *
 * Example Output:
 *
 * References:
 */

// Imports
#include <sys/types.h> // for opendir(), readdir(), closedir()
#include <sys/stat.h>  // for types of files (S_IFMT, S_IFBLK, S_IFCHR, S_IFDIR, S_IFIFO, S_IFLNK, S_IFREG, S_IFSOCK)
#include <ctype.h>     // for isdigit() and isalpha()
#include <stdio.h>     // for printf()
#include <stdbool.h>   // for bool type
#include <stdlib.h>    // for EXIT_SUCCESS
#include <string.h>    // for strcmp()
#include <dirent.h>    // for directory traversal
#include <sys/stat.h>  // for stat() and struct stat
#include <pwd.h>       // for username matching
#include <grp.h>       // for groupname matching
#include <time.h>      // for time-related functions (strftime())
#include <fnmatch.h>   // for file-name matching

typedef enum options
{
    DEFAULT, // default option
    PRINT,   // -print
    LS,      // -ls
    USER,    // -user <name>|<uid>
    NAME,    // -name <pattern>
    TYPE     // -type <t>
} options_t;

typedef struct option
{
    options_t option_type;
    char *option;        // string representation of option
    char *parameter;     // parameter of option
    struct option *next; // next option in list
} option_t;

typedef struct option_list
{
    int option_count; // amount of options
    char *starting_point;
    option_t *first; // first option
    option_t *last;  // last option
} option_list_t;
// #define MAX_FULL_PATH   1024This is my main function for traversing the directories:

// Function prototypes
void traverse_directory(const char *dir_path, option_list_t *options);
void parse_options(int argc, char *argv[], option_list_t *options);
void print_default(char *path, char *entry_name);
void print_print(const char *file_path);
void print_ls(struct stat sb, const char *file_path);
void print_user(const char *file_path, struct stat sb, char *user);
void print_name(char *file_path, char *name, char *pattern);
void print_type(const char *file_path, struct stat sb, char *type);
option_t *initialize_option(options_t option_type, char *option);
void clear_options(option_list_t *options);
void write_permissions(mode_t fileMode);
int is_string_or_id(char *input);

int main(int argc, char *argv[])
{
    option_list_t options = {.option_count = 0, .starting_point = ".", .first = NULL, .last = NULL};

    // Step 1: Parsing & Validation
    parse_options(argc, argv, &options);

    // Step 2: Iterating recursively
    traverse_directory(options.starting_point, &options); // starting point is handled by parse_options()

    clear_options(&options);

    // Program done
    return EXIT_SUCCESS;
}

// Main functionality

void traverse_directory(const char *dir_path, option_list_t *options)
{
    struct dirent *entry;
    DIR *dir = opendir(dir_path);
    bool print_starting_point = true;

    // Error handling von opendir()
    if (dir == NULL)
    {
        fprintf(stderr, "Failed to open directory: %s\n", dir_path);
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            // Skip "." and ".." entries (current and parent directory = infinite loop)
            continue;
        }
        struct stat sb;
        char file_full_path[1024];
        char file_dir_path[1024];
        snprintf(file_full_path, sizeof(file_full_path), "%s/%s", dir_path, entry->d_name);
        snprintf(file_dir_path, sizeof(file_dir_path), "%s", dir_path);

        // fill sb with information about the file
        if (stat(file_full_path, &sb) != 0)
        {
            printf("Failed to stat file %s: ", file_full_path);
            perror(""); // prints meaning of errno
            continue;
        }

        // print directory structure
        option_t *current = options->first;
        while (current != NULL)
        {
            // handle special cases where starting point is not needed
            if ((current->option_type == NAME) ||
                (current->option_type == TYPE && strcmp(current->parameter, "d") != 0))
            {
                print_starting_point = false;
            }
            // print starting point (conditionally)
            if (print_starting_point && strcmp(dir_path, options->starting_point) == 0)
            {
                printf("%s\n", dir_path); // for printing the starting point
                print_starting_point = false;
            }
            switch (current->option_type)
            {
            case DEFAULT:
                print_default(file_dir_path, entry->d_name);
                break;
            case PRINT:
                print_print(file_full_path);
                break;
            case LS:
                print_ls(sb, entry->d_name);
                break;
            case USER:
                print_user(file_full_path, sb, current->parameter);
                break;
            case NAME:
                print_name(file_dir_path, entry->d_name, current->parameter);
                break;
            case TYPE:
                print_type(file_full_path, sb, current->parameter);
                break;
            }
            current = current->next;
        }
        if (S_ISDIR(sb.st_mode))
        {
            traverse_directory(file_full_path, options);
        }
    }
}

// Argument parsing

void parse_options(int argc, char *argv[], option_list_t *options)
{
    option_t *option = NULL;
    int next_arg = 1;

    while (next_arg < argc)
    {
        bool has_parameter = false;
        if (strcmp(argv[next_arg], "-print") == 0)
        {
            option = initialize_option(PRINT, "-print");
        }
        else if (strcmp(argv[next_arg], "-ls") == 0)
        {
            option = initialize_option(LS, "-ls");
        }
        else if (strcmp(argv[next_arg], "-user") == 0)
        {
            option = initialize_option(USER, "-user");
            has_parameter = true;
        }
        else if (strcmp(argv[next_arg], "-name") == 0)
        {
            option = initialize_option(NAME, "-name");
            has_parameter = true;
        }
        else if (strcmp(argv[next_arg], "-type") == 0)
        {
            option = initialize_option(TYPE, "-type");
            has_parameter = true;
        }
        else
        {
            if (next_arg == 1)
            {
                options->starting_point = argv[next_arg];
            }
            else
            {
                fprintf(stderr, "Unknown option: %s\n", argv[next_arg]);
                exit(EXIT_FAILURE);
            }
        }
        if (has_parameter)
        {
            if (next_arg + 1 >= argc)
            {
                // Handles case of missing parameter for last option
                fprintf(stderr, "Missing parameter for option %s\n", option->option);
            }
            else if (argv[next_arg + 1][0] == '-')
            {
                // Handles case of missing parameter for option in the middle
                fprintf(stderr, "Missing parameter for option %s\n", option->option);
            }
            option->parameter = argv[++next_arg]; // same as doing "next_arg = next_arg + 1" and after that "argv[next_arg]"
        }
        if (option != NULL)
        {
            if (options->first == NULL) // first option
            {
                options->first = option;
                options->last = option;
            }
            else // append to last option
            {
                options->last->next = option;
            }
            // set last option and increment option count
            options->last = option;
            options->option_count++;
        }
        next_arg++;
    }
    // add default option if no options were given
    if (options->first == NULL)
    {
        option = initialize_option(DEFAULT, "default");
        options->first = option;
        options->last = option;
        options->option_count++;
    }
}

// Printing functions

void print_default(char *path, char *entry_name)
{
    printf("%s/%s\n", path, entry_name);
}

// Option handlers for printing various options

void print_print(const char *file_path)
{
    printf("%s\n", file_path);
}

void print_ls(struct stat sb, const char *file_path)
{
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
    if (pw != NULL)
    {
        printf("%s ", pw->pw_name);
        struct group *g = getgrgid(pw->pw_gid);
        if (g != NULL)
        {
            printf("%s ", g->gr_name);
        }
        else
        {
            // TODO error handling if gid not found
        }
    }
    else
    {
        // TODO error handling if uid not found
    }

    // Step 6: Print file size
    printf("%lld ", (long long)sb.st_size);

    // Step 7: Print modification time
    // Convert time_t to string using strftime() here
    char timestr[128];
    struct tm *tm_info;
    tm_info = localtime(&sb.st_mtim.tv_sec);
    strftime(timestr, sizeof(timestr), "%b %d %H:%M", tm_info);
    printf("%s ", timestr);

    // Step 8: Print file name
    printf("%s\n", file_path);
}

void print_user(const char *file_path, struct stat sb, char *user)
{
    struct passwd *pw;
    if (is_string_or_id(user) == 1)
    {
        pw = getpwnam(user);
    }
    else if (is_string_or_id(user) == 2)
    {
        char *endptr;
        unsigned long id = strtoul(user, &endptr, 10);
        if ((endptr == user) || (*endptr != '\0'))
        {
            // Invalid user ID: Conversion error or string has extra characters
            return;
        }
        if (id > (__uid_t)-1)
        {
            // Overflow: User ID out of range
            return;
        }
        pw = getpwuid(id);
    }
    if (pw != NULL && sb.st_uid == pw->pw_uid)
    {
        printf("%s\n", file_path);
    }
}

void print_name(char *file_path, char *name, char *pattern)
{
    if (fnmatch(pattern, name, 0) == 0)
    {
        printf("%s/%s\n", file_path, name);
    }
}

void print_type(const char *file_path, struct stat sb, char *type)
{
    char actual_type;
    switch (sb.st_mode & S_IFMT)
    {
    case S_IFBLK:
        actual_type = 'b';
        break;
    case S_IFCHR:
        actual_type = 'c';
        break;
    case S_IFDIR:
        actual_type = 'd';
        break;
    case S_IFIFO:
        actual_type = 'p';
        break;
    case S_IFLNK:
        actual_type = 'l';
        break;
    case S_IFREG:
        actual_type = 'f';
        break;
    case S_IFSOCK:
        actual_type = 's';
        break;
    default:
        actual_type = '?';
        break;
    }
    if (actual_type == type[0])
    {
        printf("%s\n", file_path);
    }
}

// helper functions for argument parsing
option_t *initialize_option(options_t option_type, char *option)
{
    option_t *new_option = (option_t *)calloc(sizeof(option_t), 1);
    if (new_option == NULL)
    {
        fprintf(stderr, "Out of memory!");
        exit(EXIT_FAILURE);
    }
    new_option->option_type = option_type;
    new_option->option = calloc(sizeof(char), strlen(option) + 1);
    strcpy(new_option->option, option);
    return new_option;
}

void clear_options(option_list_t *options)
{
    option_t *current = options->first;
    while (current != NULL)
    {
        option_t *next = current->next; // save pointer to next option
        free(current->option);          // frees the memory allocated for the option string
        free(current);                  // frees the memory allocated for the option struct
        current = next;
    }
}

// Helper functions for options

void write_permissions(mode_t mode)
{
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

int is_string_or_id(char *input)
{
    int isString = 0;
    int isID = 0;

    while (*input)
    {
        if (isdigit(*input))
        {
            isID++;
        }
        else if (isalpha(*input))
        {
            isString++;
        }
        else
        {
            return -1; // contains characters other than digits and alphabets
        }
        input++;
    }

    if (isString > 0 && isID == 0)
    {
        return 1; // only contains alphabets
    }
    else if (isID > 0 && isString == 0)
    {
        return 2; // only contains digits
    }

    return -1; // mixed or empty
}