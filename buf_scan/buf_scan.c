#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>


#define KB *(1024U)
#define MB *(1024U KB)

#define BUFF_SIZE (2048 MB)

#if 0
static void
handler(int sig, siginfo_t *si, void *unused) {
    printf("Got SIGSEGV at address: 0x%lx\n",
            (long) si->si_addr);
    exit(EXIT_FAILURE);
}
#endif




int malloc_scan(uint32_t count, int protect) {
    uint64_t *buf = malloc(BUFF_SIZE);
    if (!buf) {
        printf("malloc buffer failed! %s\n", strerror(errno));
        return -1;
    }
    for (uint64_t i = 0; i < (BUFF_SIZE/8); i++) {
        buf[i] = -1ULL;
    }

    uint32_t loops = (count == 0) ? 10 : count;
    printf("Buffer Size=0x%x, Loop counts:%d\n", BUFF_SIZE, loops);

    if (protect) {
        printf("Set buffer to read-only with mprotect!\n");
        
    }

    int corrupted = 0;
    for (uint32_t i = 0; i < loops; i++) {
        for (uint64_t j = 0; j < (BUFF_SIZE/8); j++) {
            if (buf[j] != -1ULL) {
                printf("Loop[%u],Buf[%lx]=0x%lx\n", i, j, buf[j]);
                corrupted = 1;
                break;
            }
        }
        if(corrupted)
            break;
    }

    if (buf)
        free(buf);
    return 0;
}

int main(int argc, char *argv[]) {
    int pflag = 0;
    int c;
    uint32_t count = 0;

    opterr = 0;

    while ((c = getopt (argc, argv, "pc:")) != -1) {
        switch (c) {
        case 'p':
            pflag = 1;
            break;
        case 'c':
            count = strtoul(optarg, NULL, 0);
            break;
        case '?':
            if (optopt == 'c')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
            return 1;
        default:
            abort ();
        }
    }

    malloc_scan(count, pflag);
    return 0;
}
