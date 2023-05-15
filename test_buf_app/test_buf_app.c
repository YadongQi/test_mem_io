#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "../driver/test_buf/test_buf.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage:\n");
        printf("    %s [loop count] [set_memory_ro symbol addr] [set_memory_rw symbol addr]\n", argv[0]);
        return -1;
    }

    uint32_t loop = strtoul(argv[1], NULL, 0);

    uint64_t ksmro = strtoul(argv[2], NULL, 0);
    uint64_t ksmrw = strtoul(argv[3], NULL, 0);

    uint32_t set_mem_ro = 1;

    int fd = open("/dev/tbuf", O_RDWR);
    if (fd < 0) {
        printf("Failed to open /dev/tbuf\n");
        return -1;
    }
    ioctl(fd, TBUF_SETLOOPS, &loop);
    ioctl(fd, TBUF_SET_KSETMRO, &ksmro);
    ioctl(fd, TBUF_SET_KSETMRW, &ksmrw);
    ioctl(fd, TBUF_SETMEMRO, &set_mem_ro);
    ioctl(fd, TBUF_START, NULL);

    close(fd);

    return 0;
}
