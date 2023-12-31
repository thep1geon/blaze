#include "include/err.h"
#include <stdlib.h>
#include <stdio.h>

void err(const char*  msg, i32 line, i32 col) {
    printf("Error: %s. Line: %d Column: %d\n", msg, line, col);
    exit(1);
}
