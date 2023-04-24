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

static char destfile[1024];

static void bufset(void *dest, unsigned char c, size_t size) {
    __asm__ __volatile__ (
        "cld        \n\t"
        "rep stosb  \n\t"
        :: "D" (dest), "a" (c), "c" (size)
        );
}

static int bufcmp(const unsigned char *dest, unsigned char c, size_t size, int block) {
    int ret = 0;
    if (!dest || size < 0)
        return 1;

    for (int i = 0; i < size; i++) {
        if (dest[i] != c) {
            printf("%s:0x%x: 0x%x\n", destfile, block*1024 + i, dest[i]);
            ret = 1;
        }
    }

    return ret;
}

static size_t lenstr(const char *str, size_t maxlen) {
    size_t count;

    if (!str)
        return 0;

    count = 0;
    while (*str && maxlen) {
        count++;
        maxlen--;
        str++;
    }

    return count;
}

static void copystr(char *dst, const char *src, size_t len) {
    if (!dst || !src || (len <= 0) || (len >= 1024))
        return;

    for (size_t i = 0; i < len; i++) {
        dst[i] = src[i];
        if (src[i] == '\0')
            break;
    }
}

static int copy_file(const char *src, const char *dst) {
    FILE *fptr_src = NULL, *fptr_dst = NULL;
    uint64_t file_size = 0;
    size_t rd_size = 0;
    unsigned char *buf = NULL;

    if (!src || !dst)
        return -1;

    buf = (unsigned char *)malloc(COPY_BUFFER_MAXSIZE);
    bufset(buf, 0, COPY_BUFFER_MAXSIZE);

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
    copystr(destfile, dst, lenstr(dst, 1024));

    // Copy contents
    int found_corrupt = 0;
    int block = 0;
    do {
        rd_size = fread(buf, 1, COPY_BUFFER_MAXSIZE, fptr_src);
        if (bufcmp(buf, 0xFF, rd_size, block) != 0) {
            printf("Found corrupt data: rd_size=%ld\n", rd_size);
            found_corrupt = 1;
        }
        fwrite(buf, 1, rd_size, fptr_dst);
        block++;
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
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage:\n");
        printf("    %s [sourcefile] [targetfile]\n", argv[0]);
        return -1;
    }

    return copy_file(argv[1], argv[2]);
}
