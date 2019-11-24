#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <endian.h>
#include <string.h>
#include <errno.h>

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

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename.bin>\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd <= 0) {
        fprintf(stderr, "Unable to open binary partition file: %s\n", strerror(errno));
        return -1;
    }

    printf("# Name,               Type, SubType,  Offset,     Size,       Flags\n");

    while ((read(fd, &part, sizeof(struct partition))) > 0) {
        if (le16toh(part.magic) == 0x50AA) {

            printf("%s, ", part.name);

            for (i = strlen(part.name); i < 20; i++) {
                printf(" ");
            }

            if (part.type == 0) {
                printf("app,  ");

                switch (part.subtype) {
                    case 0x00: printf("factory,  "); break;
                    case 0x10: printf("ota_0,    "); break;
                    case 0x11: printf("ota_1,    "); break;
                    case 0x12: printf("ota_2,    "); break;
                    case 0x13: printf("ota_3,    "); break;
                    case 0x14: printf("ota_4,    "); break;
                    case 0x15: printf("ota_5,    "); break;
                    case 0x16: printf("ota_6,    "); break;
                    case 0x17: printf("ota_7,    "); break;
                    case 0x18: printf("ota_8,    "); break;
                    case 0x19: printf("ota_9,    "); break;
                    case 0x1a: printf("ota_10,   "); break;
                    case 0x1b: printf("ota_11,   "); break;
                    case 0x1c: printf("ota_12,   "); break;
                    case 0x1d: printf("ota_13,   "); break;
                    case 0x1e: printf("ota_14,   "); break;
                    case 0x1f: printf("ota_15,   "); break;
                    case 0x20: printf("test,     "); break;
                    default: printf("unknown,   "); break;
                }
            } else if (part.type == 1) { 
                printf("data, ");
            
                switch (part.subtype) {
                    case 0x00: printf("ota,      "); break;
                    case 0x01: printf("phy,      "); break;
                    case 0x02: printf("nvs,      "); break;
                    case 0x03: printf("coredump, "); break;
                    case 0x04: printf("nvs_keys, "); break;
                    case 0x80: printf("esphttpd, "); break;
                    case 0x81: printf("fat,      "); break;
                    case 0x82: printf("spiffs,   "); break;
                    default: printf("0x%02x,     ", part.subtype); break;
                }
            } else {
                printf("unknown, ");
            }

            printf("0x%08x, ", le32toh(part.start));
            printf("0x%08x, ", le32toh(part.size));

            printf("\n");
        }


    }
    close(fd);
    return 0;
}
