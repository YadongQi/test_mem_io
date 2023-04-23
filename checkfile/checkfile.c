/*
 * Copyright (c) 2023 Qi, Yadong
 * All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define KB *(1024U)
#define MB *(1024U KB)
#define GB *(1024U MB)

#define BUFFER_MAXSIZE (1 MB)

static int check_file(const char *src) {
    FILE *fptr_src = NULL;
    uint64_t file_size = 0;
    size_t rd_size = 0;
    unsigned char *buf = NULL;
    unsigned char *base = NULL;

    if (!src)
        return -1;

    buf = (unsigned char *)malloc(BUFFER_MAXSIZE);
    memset(buf, 0, BUFFER_MAXSIZE);

    base = (unsigned char *)malloc(BUFFER_MAXSIZE);
    memset(base, 0xFF, BUFFER_MAXSIZE);

    // Open destination file for writing
    fptr_src = fopen(src, "r");
    if (fptr_src == NULL) {
        printf("Cannot open file %s \n", src);
        return -1;
    }
    fseek(fptr_src, 0, SEEK_END);
    file_size = ftell(fptr_src);
    printf("Source file: %s, size=%ld(Bytes)\n", src, file_size);
    rewind(fptr_src);

    // read contents and compare
    int loop = file_size/BUFFER_MAXSIZE;
    int found_corrupt = 0;
    for (int i = 0; i < loop; i++) {
        rd_size = fread(buf, 1, BUFFER_MAXSIZE, fptr_src);
        if ((memcmp(buf, base, BUFFER_MAXSIZE) != 0) || (rd_size != BUFFER_MAXSIZE)) {
            printf("Found corrupt data: loop=%d, rd_size=%ld\n", i, rd_size);
            found_corrupt = 1;
            break;
        }
    }
    if (ferror(fptr_src))
        printf("(loop)I/O error while reading %s", src);

    if ((file_size%BUFFER_MAXSIZE != 0) && (found_corrupt == 0)) {
        fread(buf, 1, file_size%BUFFER_MAXSIZE, fptr_src);
        if (memcmp(buf, base, BUFFER_MAXSIZE) != 0) {
            printf("Found corrupt data in tail\n");
            found_corrupt = 1;
        }
    }
    if (ferror(fptr_src))
        printf("I/O error while writing %s", src);

    fclose(fptr_src);
    if (buf)
        free(buf);
    return found_corrupt;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage:\n");
        printf("    %s [targetfile]\n", argv[0]);
        return -1;
    }

    if (check_file(argv[1]) == 0) {
        printf("File check: Pass!\n");
    } else {
        printf("File check: Failed!\n");
    }

    return 0;
}
