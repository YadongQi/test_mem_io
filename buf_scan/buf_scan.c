#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <malloc.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096U


#define KB *(1024U)
#define MB *(1024U KB)

#define BUFF_SIZE (2048 MB)
#define BUFF_CNT64 (BUFF_SIZE/8)

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

static void
handler(int sig, siginfo_t *si, void *unused) {
    printf("Got SIGSEGV at address: 0x%lx\n",
            (long) si->si_addr);
    exit(EXIT_FAILURE);
}


int malloc_scan(uint32_t count, int protect) {
    uint64_t *buf = memalign(PAGE_SIZE, BUFF_SIZE);
    if (!buf) {
        printf("malloc buffer failed! %s\n", strerror(errno));
        return -1;
    }
    for (uint64_t i = 0; i < BUFF_CNT64; i++) {
        buf[i] = -1ULL;
    }

    uint32_t loops = (count == 0) ? 10 : count;
    printf("Buffer:[%p, 0x%x], Loop counts:%d\n", buf, BUFF_SIZE, loops);

    if (protect) {
        printf("Set buffer to read-only with mprotect!\n");
            struct sigaction sa;

        sa.sa_flags = SA_SIGINFO;
        sigemptyset(&sa.sa_mask);
        sa.sa_sigaction = handler;
        if (sigaction(SIGSEGV, &sa, NULL) == -1)
            handle_error("sigaction");        

        if (mprotect(buf, BUFF_SIZE, PROT_READ) == -1)
            handle_error("mprotect");
    }

    int corrupted = 0;
    for (uint32_t i = 0; i < loops; i++) {
        for (uint64_t j = 0; j < BUFF_CNT64; j++) {
            if (buf[j] != -1ULL) {
                printf("Loop[%u],Buf[%p]=0x%lx\n", i, &buf[j], buf[j]);
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
