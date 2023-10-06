//
// Created by rini on 29.09.23.
//

// Imports
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>    // for isdigit() and isalpha()
#include <stdio.h>    // for printf()
#include <stdbool.h>  // for bool type
#include <stdlib.h>   // for EXIT_SUCCESS
#include <string.h>   // for strcmp()
#include <dirent.h>   // for directory traversal
#include <sys/stat.h> // for stat() and struct stat
#include <pwd.h>      // for username matching
#include <grp.h>      // for groupname matching
#include <time.h>     // for time-related functions (strftime())
#include <fnmatch.h>  // for file-name matching

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

typedef enum options
{
    PRINT, // -print
    LS,    // -ls
    USER,  // -user <name>|<uid>
    NAME,  // -name <pattern>
    TYPE   // -type <t>
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
    option_t *first;  // first option
    option_t *last;   // last option
} option_list_t;
// #define MAX_FULL_PATH   1024

// Function prototypes
void traverse_directory(const char *dir_path, option_list_t *options);
void parse_options(char *argv[], option_list_t *options);
void print_default(char *path, char *entry_name);
void print_print(struct stat sb, const char *file_path);
void print_ls(struct stat sb, const char *file_path);
void print_user(const char *file_path, struct stat sb, char *user);
void print_name(char *name, char *pattern);
void print_type(const char *file_path, struct stat sb, char *type);
void write_permissions(mode_t fileMode);
int is_string_or_id(char *input);

int main(int argc, char *argv[])
{
    char *starting_point = ".";
    option_list_t options = {.option_count = 0, .first = NULL, .last = NULL};

    // Step 1: Parsing & Validation
    parse_options(argv, &options);

    // Step 2: Iterating recursively
    // 2nd statement in the if won't be evaluated when no parameters were provided, so it is safe to access
    if (argc > 2 && argv[1][0] != '-')
    { // when 1st parameter isn't an option, use provided starting directory
        traverse_directory(argv[1], &options);
    }
    else
    { // otherwise use default current working directory
        traverse_directory(starting_point, &options);
    }

    // Program done
    return EXIT_SUCCESS;
}

// Main functionality

void traverse_directory(const char *dir_path, option_list_t *options)
{
    struct dirent *entry;
    DIR *dir = opendir(dir_path);

    // Error handling von opendir()
    if (dir == NULL)
    {
        fprintf(stderr, "Failed to open directory: %s\n", dir_path);
        return;
    }

    // printf("%s\n", dir_path); // for "." and ".."
    while ((entry = readdir(dir)) != NULL)
    {
        /*
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            //
            //printf("%s\n", entry->d_name);
            break;
        }
        */
        struct stat sb;
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        if (stat(full_path, &sb) == 0)
        {
            option_t *current = options->first;
            if (current == NULL)
            {
                print_default(full_path, entry->d_name);
            }
            while (current != NULL)
            {
                switch (current->option_type)
                {
                case PRINT:
                    print_print(sb, entry->d_name);
                    break;
                case LS:
                    print_ls(sb, entry->d_name);
                    break;
                case USER:
                    print_user(entry->d_name, sb, current->parameter);
                    break;
                case NAME:
                    print_name(entry->d_name, current->parameter);
                    break;
                case TYPE:
                    print_type(entry->d_name, sb, current->parameter);
                    break;
                }
                current = current->next;
            }
            if (S_ISDIR(sb.st_mode))
            {
                traverse_directory(full_path, options);
            }
        }
    }
}

// Argument parsing

void parse_options(char *argv[], option_list_t *options)
{
    char **current = argv;

    while (*current != NULL)
    {
        option_t *option = NULL;
        if (strcmp(*current, "-print") == 0)
        {
            option = (option_t *)calloc(sizeof(option_t), 1);
            if (option == NULL)
            {
                fprintf(stderr, "Out of memory!");
            }
            option->option_type = PRINT;
            option->option = calloc(sizeof(char), strlen("-print") + 1);
            strcpy(option->option, "-print");
            option->parameter = NULL;
        }
        else if (strcmp(*current, "-ls") == 0)
        {
            option = (option_t *)calloc(sizeof(option_t), 1);
            if (option == NULL)
            {
                fprintf(stderr, "Out of memory!");
            }
            option->option_type = LS;
            option->option = calloc(sizeof(char), strlen("-ls") + 1);
            strcpy(option->option, "-ls");
            option->parameter = NULL;
        }
        else if (strcmp(*current, "-user") == 0)
        {
            option = (option_t *)calloc(sizeof(option_t), 1);
            if (option == NULL)
            {
                fprintf(stderr, "Out of memory!");
            }
            option->option_type = USER;
            option->option = calloc(sizeof(char), strlen("-user") + 1);
            strcpy(option->option, "-user");
            option->parameter = *(current + 1);
        }
        else if (strcmp(*current, "-name") == 0)
        {
            option = (option_t *)calloc(sizeof(option_t), 1);
            if (option == NULL)
            {
                fprintf(stderr, "Out of memory!");
            }
            option->option_type = NAME;
            option->option = calloc(sizeof(char), strlen("-name") + 1);
            strcpy(option->option, "-name");
            option->parameter = *(current + 1);
        }
        else if (strcmp(*current, "-type") == 0)
        {
            option = (option_t *)calloc(sizeof(option_t), 1);
            if (option == NULL)
            {
                fprintf(stderr, "Out of memory!");
            }
            option->option_type = TYPE;
            option->option = calloc(sizeof(char), strlen("-type") + 1);
            strcpy(option->option, "-type");
            option->parameter = *(current + 1);
        }
        else
        {
            fprintf(stderr, "Unknown option: %s\n", *current);
        }
        if (option != NULL)
        {
            if (options->first == NULL)
            {
                options->first = option;
                options->last = option;
            }
            else
            {
                options->last->next = option;
            }
            options->last = option;
            options->option_count++;
        }
        current++;
    }
}

void print_default(char *path, char *entry_name)
{
    printf("%s/%s\n", path, entry_name);
}

// Option handlers

void print_print(struct stat sb, const char *file_path)
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

void print_name(char *name, char *pattern)
{
    if (fnmatch(pattern, name, 0) == 0)
    {
        printf("%s\n", name);
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

// Helper functions

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