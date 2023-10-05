//
// Created by rini on 30.09.23.
//

#ifndef TUTORIUM_SYSPROG_2023_MYPOPEN_H
#define TUTORIUM_SYSPROG_2023_MYPOPEN_H

#include <stdio.h>

FILE *mypopen(const char *command, const char *type);
int mypclose(FILE *stream);

#endif //TUTORIUM_SYSPROG_2023_MYPOPEN_H
