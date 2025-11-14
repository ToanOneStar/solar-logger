#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

char* read_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);
    char *buf = malloc(len + 1);
    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

char* utils_int_to_str(int num) {
    static char buf[8];
    snprintf(buf, sizeof(buf), "%d", num);
    return buf;
}