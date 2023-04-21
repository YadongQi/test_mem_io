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

#define KB *(1024U)
#define MB *(1024U KB)

#define COPY_BUFFER_MAXSIZE (1 MB)

static int copy_file(const char *src, const char *dst) {
    FILE *fptr_src = NULL, *fptr_dst = NULL;
    uint64_t file_size = 0;
    size_t rd_size = 0;
    unsigned char *buf = NULL;

    if (!src || !dst)
        return -1;

    buf = (unsigned char *)calloc(1, COPY_BUFFER_MAXSIZE);

    // Open source file for reading
    fptr_src = fopen(src, "r");
    if (fptr_src == NULL) {
        printf("Cannot open file %s \n", src);
        return -1;
    }
    fseek(fptr_src, 0, SEEK_END);
    file_size = ftell(fptr_src);
    printf("Source file: %s, size=%ld(Bytes)\n", src, file_size);
    rewind(fptr_src);

    // Open destination file for writing
    fptr_dst = fopen(dst, "w");
    if (fptr_dst == NULL) {
        printf("Cannot open file %s \n", dst);
        return -1;
    }

    // Copy contents
    do {
        rd_size = fread(buf, 1, COPY_BUFFER_MAXSIZE, fptr_src);
        fwrite(buf, 1, rd_size, fptr_dst);
    } while (!feof(fptr_src) && !ferror(fptr_src) && !ferror(fptr_dst));

    if (ferror(fptr_src))
        printf("I/O error while reading %s", src);

    if (ferror(fptr_dst))
        printf("I/O error while writing %s", dst);

    printf("\nContents copied from %s to %s\n", src, dst);

    fclose(fptr_src);
    fclose(fptr_dst);
    if (buf)
        free(buf);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage:\n");
        printf("    %s [sourcefile] [targetfile]\n", argv[0]);
        return -1;
    }

    copy_file(argv[1], argv[2]);
 
    return 0;
}
