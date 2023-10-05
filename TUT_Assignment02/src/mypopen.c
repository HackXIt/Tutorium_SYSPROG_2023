//
// Created by rini on 30.09.23.
//

/*--- COMMON LIBRARIES ---*/
#include <stdio.h>	   // -> fdopen()
#include <unistd.h>	   // -> pipe(), fork(), dup2(), exec(), read()/write()
#include <stdlib.h>	   // -> system() -> sh
#include <sys/wait.h>  // -> waitpid() & stat_loc
#include <sys/stat.h>  // -> fstat()
#include <sys/types.h> // -> st_mode
#include <string.h>	   // -> strsep()
#include <errno.h>

/*--- CUSTOM LIBRARIES ---*/
#include "../inc/mypopen.h"

// This is not best practice, but we're only supposed to deal with ONE process, so it's fine here
pid_t mypid = -1;
int myfd = -1;

FILE *mypopen(const char *command, const char *type)
{
    // initial error handling
    if (mypid != -1)
    {
        errno = EAGAIN;
        return NULL;
    }
    if ((command == NULL) || (type == NULL))
    {
        errno = EINVAL;
        return NULL;
    }
    // start mypopen routine
    FILE *pipe_stream;
    int pipefd[2];
    if (pipe(pipefd) == -1) // create a pipe
    {
        errno = EMFILE;
        return NULL;
    }

#ifdef DEBUG_CHILD
    char buf[BUFFER];																// Buffer for debug-output;
	printf("\n--- mypopen ---\n--- command: %s | mode: %s ---\n\n", command, type); // Seperator for readability
#endif

    mypid = fork();

    // ----- process was cloned -----

    if (mypid == -1) // Failed to fork
    {
        errno = EAGAIN;
        close(pipefd[0]);
        close(pipefd[1]);
        return NULL;
    }
    if (mypid == 0)
    {								// This is the child-process block.
        if (strcmp(type, "r") == 0) // read-block of child-process
        {

            close(pipefd[0]); // closing unused read-end
            // in the child, the STDOUT is on the read end of the pipe
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]); // closing write-end because duplication exists in STDOUT_FILENO
        }
        else if (strcmp(type, "w") == 0) // write-block of child-process
        {
            close(pipefd[1]); // closing unused write-end
            // in the child, the STDIN is on the write end of the pipe
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
        }
        else // invalid type given
        {
            close(pipefd[0]);
            close(pipefd[1]);
            _exit(EINVAL);
        }
        execl("/bin/sh", "sh", "-c", command, NULL);
#ifdef DEBUG_CHILD
        sleep(5);
#endif
        _exit(127); // Exits and closes all file-descriptors
    }
    else
    { // This is the parent-process block.
        // Currently only reading output of child-process, because testing with 'ls'
        if (strcmp(type, "r") == 0) // read-block of parent-process
        {
            close(pipefd[1]); // Closing unused write-end
            myfd = pipefd[0];
            pipe_stream = fdopen(pipefd[0], type);
#ifdef DEBUG_CHILD
            int ret = 0;
			while ((ret = read(pipefd[0], &buf, 1)) > 0)
			{
				write(fileno(stdout), &buf, 1);
			}
#endif
        }
        else if (strcmp(type, "w") == 0) // write-block of parent-process
        {
            close(pipefd[0]); // Closing unused read-end
            myfd = pipefd[1];
            pipe_stream = fdopen(pipefd[1], type);
#ifdef DEBUG_CHILD
            int ret = 0;
			while ((ret = write(pipefd[1], &buf, 1)) > 0)
			{
				read(fileno(stdin), &buf, 1);
			}
#endif
        }
        else
        {
            close(pipefd[0]);
            close(pipefd[1]);
            errno = EINVAL;
            return NULL;
        }
        if (pipe_stream == NULL)
        {
            errno = EMFILE;
            // Only 1 check necessary to determine the pipe-end to close
            // program would have failed earlier if type would be incorrect
            if (strcmp(type, "w") == 0)
            {
                close(pipefd[1]);
            }
            else
            {
                close(pipefd[0]);
            }
            return NULL;
        }
    }

    return pipe_stream;
}

int mypclose(FILE *stream)
{
    if (mypid == -1) // No child process was opened!
    {
        errno = ECHILD;
        return -1;
    }
    if (stream == NULL) // The given file-stream is NULL and thus invalid
    {
        errno = EINVAL;
        if (myfd > 0)
        {
            close(myfd);
        }
        return -1;
    }
    if (myfd != fileno(stream)) // The given file-stream was not created with mypopen()
    {
        errno = EINVAL;
        if (myfd > 0)
        {
            close(myfd);
        }
        return -1;
    }
    int child_exit_status = 0;
    pid_t child = -1;
    if (fclose(stream) == EOF)
    {
        errno = ECHILD;
        return -1;
    }
    else
    {
        myfd = -1;
    }
    // child = wait(&child_exit_status); // This waits for the next child process to exit
    child = waitpid(mypid, &child_exit_status, 0);
    if (!WIFEXITED(child_exit_status))
    {
        errno = ECHILD;
        if (myfd > 0)
        {
            close(myfd);
        }
        return -1;
    }
    if (child != mypid)
    {
        errno = ECHILD;
        return -1;
    }
#ifdef VERBOSE
    printf("PID: %d - EXIT_CODE: %d\n", child, child_exit_status);
#endif
    mypid = -1;
    return child_exit_status;
}