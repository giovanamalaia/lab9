#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
    struct stat sb;
    off_t len;
    char *p;
    int fd;
    struct timeval start, end;
    long elapsed_mmap, elapsed_read;

    if (argc < 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        exit(1);
    }

    if (!S_ISREG(sb.st_mode)) {
        fprintf(stderr, "%s is not a file\n", argv[1]);
        exit(1);
    }

    gettimeofday(&start, NULL);
    
    p = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    for (len = 0; len < sb.st_size; len++) putchar(p[len]);

    gettimeofday(&end, NULL);
    elapsed_mmap = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

    if (munmap(p, sb.st_size) == -1) {
        perror("munmap");
        exit(1);
    }

    if (close(fd) == -1) {
        perror("close");
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    gettimeofday(&start, NULL);

    char buffer[4096];
    while ((len = read(fd, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, len, stdout);
    }

    gettimeofday(&end, NULL);
    elapsed_read = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

    if (close(fd) == -1) {
        perror("close");
        exit(1);
    }

    printf("\nTime taken using mmap: %ld microseconds\n", elapsed_mmap);
    printf("Time taken using read: %ld microseconds\n", elapsed_read);

    return 0;
}
