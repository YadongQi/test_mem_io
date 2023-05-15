#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define PAGE_SHIFT 12
#define PAGEMAP_LENGTH 8

int main(int argc, char **argv)
{
	unsigned long vaddr, pid, paddr = 0, offset;
	char *endptr;
	FILE *pagemap;
	char filename[1024] = {0};
	int ret = -1;
	int page_size, page_shift = -1;

	page_size = getpagesize();
	pid = strtol(argv[1], &endptr, 10);
	vaddr = strtol(argv[2], &endptr, 16);
	printf("getting page number of virtual address %lu of process %ld\n",vaddr, pid);

	sprintf(filename, "/proc/%ld/pagemap", pid);

	printf("opening pagemap %s\n", filename);
	pagemap = fopen(filename, "rb");
	if (!pagemap) {
		perror("can't open file. ");
		goto err;
	}

	offset = (vaddr / page_size) * PAGEMAP_LENGTH;
	printf("moving to %ld\n", offset);
	if (fseek(pagemap, (unsigned long)offset, SEEK_SET) != 0) {
		perror("fseek failed. ");
		goto err;
	}

	if (fread(&paddr, 1, (PAGEMAP_LENGTH-1), pagemap) < (PAGEMAP_LENGTH-1)) {
		perror("fread fails. ");
		goto err;
	}
	paddr = paddr & 0x7fffffffffffff;
	printf("physical frame address is 0x%lx\n", paddr);

	offset = vaddr % page_size;

	/* PAGE_SIZE = 1U << PAGE_SHIFT */
	while (!((1UL << ++page_shift) & page_size));

	paddr = (unsigned long)((unsigned long)paddr << page_shift) + offset;
	printf("physical address is 0x%lx\n", paddr);

	ret = 0;
err:
	fclose(pagemap);
	return ret;
}
