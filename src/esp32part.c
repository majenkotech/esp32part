#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#ifdef linux
#include <endian.h>
#else
#define le32toh(X) (X)
#define le16toh(X) (X)
#endif

#include "config.h"

struct partition {
    uint16_t magic;
    uint8_t type;
    uint8_t subtype;
    uint32_t start;
    uint32_t size;
    char name[20];
} __attribute__((packed));

int main(int argc, char **argv) {
    int fd;
    struct partition part;
    int i;
    int opt;
    const char *out = NULL;
    FILE *outfd = stdout;

    int version = 0;
    int usage = 0;

    while ((opt = getopt(argc, argv, "vho:")) != -1) {
        switch (opt) {
            case 'v': version++; break;
            case 'o': out = optarg; break;
            case 'h': usage++; break;
            default: usage++; break;
        }
    }
    
    
    if (version > 0) {
        printf("esp32part version " VERSION " (c) 2019 Majenko Technologies\n");
        return 0;
    }

    if (optind >= argc) {
        usage++;
    }

    if (usage > 0) {
        printf("Usage: %s [options] <filename.bin>\n", argv[0]);
        printf("  Options:\n");
        printf("    -h: this help text\n");
        printf("    -v: print version and exit\n");
        printf("    -o [filename.csv]: output to file\n");
        return 0;
    }

    fd = open(argv[optind], O_RDONLY);
    if (fd <= 0) {
        fprintf(stderr, "%s: Unable to open %s for reading: %s\n", argv[0], argv[optind], strerror(errno));
        return -1;
    }

    if (out != NULL) {
        outfd = fopen(out, "w");
        if (!outfd) {
            fprintf(stderr, "%s: Unable to open %s for writing: %s\n", argv[0], out, strerror(errno));
            return -1;
        }
    }

    fprintf(outfd, "# Name,               Type, SubType,  Offset,     Size,       Flags\n");

    while ((read(fd, &part, sizeof(struct partition))) > 0) {
        if (le16toh(part.magic) == 0x50AA) {

            fprintf(outfd, "%s, ", part.name);

            for (i = strlen(part.name); i < 20; i++) {
                fprintf(outfd, " ");
            }

            if (part.type == 0) {
                fprintf(outfd, "app,  ");

                switch (part.subtype) {
                    case 0x00: fprintf(outfd, "factory,  "); break;
                    case 0x10: fprintf(outfd, "ota_0,    "); break;
                    case 0x11: fprintf(outfd, "ota_1,    "); break;
                    case 0x12: fprintf(outfd, "ota_2,    "); break;
                    case 0x13: fprintf(outfd, "ota_3,    "); break;
                    case 0x14: fprintf(outfd, "ota_4,    "); break;
                    case 0x15: fprintf(outfd, "ota_5,    "); break;
                    case 0x16: fprintf(outfd, "ota_6,    "); break;
                    case 0x17: fprintf(outfd, "ota_7,    "); break;
                    case 0x18: fprintf(outfd, "ota_8,    "); break;
                    case 0x19: fprintf(outfd, "ota_9,    "); break;
                    case 0x1a: fprintf(outfd, "ota_10,   "); break;
                    case 0x1b: fprintf(outfd, "ota_11,   "); break;
                    case 0x1c: fprintf(outfd, "ota_12,   "); break;
                    case 0x1d: fprintf(outfd, "ota_13,   "); break;
                    case 0x1e: fprintf(outfd, "ota_14,   "); break;
                    case 0x1f: fprintf(outfd, "ota_15,   "); break;
                    case 0x20: fprintf(outfd, "test,     "); break;
                    default: fprintf(outfd, "unknown,   "); break;
                }
            } else if (part.type == 1) { 
                fprintf(outfd, "data, ");
            
                switch (part.subtype) {
                    case 0x00: fprintf(outfd, "ota,      "); break;
                    case 0x01: fprintf(outfd, "phy,      "); break;
                    case 0x02: fprintf(outfd, "nvs,      "); break;
                    case 0x03: fprintf(outfd, "coredump, "); break;
                    case 0x04: fprintf(outfd, "nvs_keys, "); break;
                    case 0x80: fprintf(outfd, "esphttpd, "); break;
                    case 0x81: fprintf(outfd, "fat,      "); break;
                    case 0x82: fprintf(outfd, "spiffs,   "); break;
                    default: fprintf(outfd, "0x%02x,     ", part.subtype); break;
                }
            } else {
                fprintf(outfd, "unknown, ");
            }

            fprintf(outfd, "0x%08x, ", le32toh(part.start));
            fprintf(outfd, "0x%08x, ", le32toh(part.size));

            fprintf(outfd, "\n");
        }


    }
    if (out != NULL) {
        fclose(outfd);
    }
    close(fd);
    return 0;
}
