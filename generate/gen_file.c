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

static int gen_file(const char *dst, size_t file_size) {
    FILE *fptr_dst = NULL;
    unsigned char *buf = NULL;
    size_t left_size = file_size;

    if (!dst || (file_size < 0))
        return -1;

    buf = (unsigned char *)malloc(BUFFER_MAXSIZE);
    memset(buf, 0xFF, BUFFER_MAXSIZE);

    // Open destination file for writing
    fptr_dst = fopen(dst, "w");
    if (fptr_dst == NULL) {
        printf("Cannot open file %s \n", dst);
        return -1;
    }

    // Copy contents
    int loop = file_size/BUFFER_MAXSIZE;
    for (int i = 0; i < loop; i++) {
        fwrite(buf, 1, BUFFER_MAXSIZE, fptr_dst);
    }
    if (ferror(fptr_dst))
    printf("(loop)I/O error while writing %s", dst);

    if (file_size%BUFFER_MAXSIZE != 0)
        fwrite(buf, 1, file_size%BUFFER_MAXSIZE, fptr_dst);
    if (ferror(fptr_dst))
        printf("I/O error while writing %s", dst);

    printf("\nGenerated %s\n", dst);

    fclose(fptr_dst);
    if (buf)
        free(buf);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage:\n");
        printf("    %s [targetfile]\n", argv[0]);
        return -1;
    }

    gen_file(argv[1], 1 GB);

    return 0;
}
