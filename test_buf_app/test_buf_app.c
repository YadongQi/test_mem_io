#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "../driver/test_buf/test_buf.h"


void print_usage(void) {
    printf("Usage:\n");
    printf("    test-buf [start/stop] [set_memory_ro symbol addr] [set_memory_rw symbol addr]\n");
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        print_usage();
        return -1;
    }

    int action_ioc = -1;
    if (strcmp(argv[1], "start") == 0) {
	printf("IOCTL_TBUF_START!\n");
        action_ioc = TBUF_START;
    } else if(strcmp(argv[1], "stop") == 0) {
	printf("IOCTL_TBUF_STOP!\n");
        action_ioc = TBUF_STOP;
    } else {
        printf("Unknow action!\n");
        print_usage();
        return -1;
    }

    uint64_t ksmro = strtoul(argv[2], NULL, 0);
    uint64_t ksmrw = strtoul(argv[3], NULL, 0);

    uint32_t set_mem_ro = 1;

    int fd = open("/dev/tbuf", O_RDWR);
    if (fd < 0) {
        printf("Failed to open /dev/tbuf\n");
        return -1;
    }

    if (ksmro != 0)
        ioctl(fd, TBUF_SET_KSETMRO, &ksmro);
    if (ksmrw != 0)
        ioctl(fd, TBUF_SET_KSETMRW, &ksmrw);

    ioctl(fd, TBUF_SETMEMRO, &set_mem_ro);
    ioctl(fd, action_ioc, NULL);

    close(fd);

    return 0;
}
