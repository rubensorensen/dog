#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#define INTERVAL_MS 50

static void
usage(const char *program_name)
{
    if (program_name) {
        printf("usage: %s file ...\n", program_name);
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-h") == 0 ||
        strcmp(argv[1], "--help") == 0) {
        usage(argv[0]);
        exit(EXIT_SUCCESS);
    }

    for (int i = 1; i < argc; ++i) {
        const char* file_path = argv[i];
        struct stat file_stats;

        if (stat(file_path, &file_stats) < 0) {
            fprintf(stderr, "Error reading stats from file %s: %s\n",
                    file_path, strerror(errno));
            exit(1);
        }
        off_t file_size = file_stats.st_size;

        int fd = open(file_path, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "Error opening file %s: %s\n",
                    file_path, strerror(errno));
            exit(1);
        }

        char* data = (char*)mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (data == MAP_FAILED) {
            fprintf(stderr, "Error mapping file %s: %s\n",
                    file_path, strerror(errno));
            exit(1);
        }

        if (close(fd) < 0) {
            fprintf(stderr, "Error closing file %s: %s\n",
                    file_path, strerror(errno));
            exit(1);
        }

        for (size_t j = 0; j < (size_t)file_size; ++j) {
            printf("%c", data[j]);
            fflush(stdout);

            if (data[j] == ' ' || data[j] == '\t' || data[j] == '\n') {
                continue;
            }

            struct timespec req = {
                .tv_sec = (time_t)0,
                .tv_nsec = (long)(INTERVAL_MS * 1000 * 1000)
            };
            while (nanosleep(&req, &req) == -1 && errno == EINTR) {}
        }

        if (munmap((void*)data, file_size) < 0) {
            fprintf(stderr, "Error unmapping file %s: %s\n",
                    file_path, strerror(errno));
        }
    }

    return EXIT_SUCCESS;
}
