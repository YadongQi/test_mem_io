#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "../driver/test_buf/test_buf.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage:\n");
        printf("    %s [loop count]\n", argv[0]);
        return -1;
    }

    uint32_t loop = strtoul(argv[1], NULL, 0);

    int fd = open("/dev/tbuf", O_RDWR);
    if (fd < 0) {
        printf("Failed to open /dev/tbuf\n");
        return -1;
    }
    ioctl(fd, TBUF_SETLOOPS, &loop);
    ioctl(fd, TBUF_START, NULL);

    close(fd);

    return 0;
}
