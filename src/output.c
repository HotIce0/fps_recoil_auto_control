#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h> /* open */
#include <sys/stat.h> /* open */
#include <fcntl.h> /* open */
#include <unistd.h> /* close */

#include "log.h"
#include "hid.h"
#include "bswap.h"
#include "output.h"


// #define LOG_DUMP
#ifdef LOG_DUMP
#include <stdio.h>
#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
void dump_hex1(const uint8_t *ptr, uint16_t buflen)
{
    unsigned char *buf = (unsigned char*)ptr;
    int i, j;

    for (i=0; i<buflen; i+=16)
    {
        printf("%08X: ", i);

        for (j=0; j<16; j++)
            if (i+j < buflen)
                printf("%02X ", buf[i+j]);
            else
                printf("   ");
        printf(" ");

        for (j=0; j<16; j++)
            if (i+j < buflen)
                printf("%c", __is_print(buf[i+j]) ? buf[i+j] : '.');
        printf("\n");
    }
}
#else
#define dump_hex1(_p, _len)
#endif


typedef struct output_dev {
    hid_dev dev;
    const char *path;
    int fd;
} output_dev;

static output_dev s_mouse = {
    .path = "/dev/hidg0",
    .fd = -1,
    .dev = {
        .report_length = 7,
        .type = HID_DEV_MOUSE,
        .mouse = {
            .btn = {
                .physical_minimum = 1,
                .physical_maximum = 16,
                .logical_minimum = 0,
                .logical_maximum = 1,
                .size = 1,
                .count = 16,
                .report_buffer_offset = 0
            },
            .orien = {
                .physical_minimum = 0,
                .physical_maximum = 0,
                .logical_minimum = -32767,
                .logical_maximum = 32767,
                .size = 16,
                .count = 2,
                .report_buffer_offset = 2
            },
            .wheel = {
                .physical_minimum = 0,
                .physical_maximum = 0,
                .logical_minimum = -127,
                .logical_maximum = 127,
                .size = 8,
                .count = 1,
                .report_buffer_offset = 6
            }
        }
    }
};


static output_dev s_kbd = {
    .path = "/dev/hidg1",
    .fd = -1,
    .dev = {
        .report_length = 8,
        .type = HID_DEV_KEYBOARD,
        .kbd = {
            .ctrl_btn = {
                .physical_minimum = 0xe0,
                .physical_maximum = 0xe7,
                .logical_minimum = 0,
                .logical_maximum = 1,
                .size = 1,
                .count = 8,
                .report_buffer_offset = 0
            },
            .led = {
                .physical_minimum = 0x01,
                .physical_maximum = 0x05,
                .logical_minimum = 0,
                .logical_maximum = 1,
                .size = 8,
                .count = 1,
                .report_buffer_offset = 1
            },
            .key = {
                .physical_minimum = 0x00,
                .physical_maximum = 0xFF,
                .logical_minimum = 0,
                .logical_maximum = 255,
                .size = 8,
                .count = 6,
                .report_buffer_offset = 2
            }
        }
    }
};

static int send_report(output_dev *dev)
{
    hid_dev *hdev = &dev->dev;
    log_debug("DUMP");
    dump_hex1(hdev->report_buffer, hdev->report_length);
    ssize_t size = write(dev->fd, hdev->report_buffer, hdev->report_length);
    if (size != hdev->report_length) {
        fprintf(stderr, "write failed, path=%s fd=%d, len=%zu\n",
            dev->path, dev->fd, hdev->report_length);
        return -1;
    }
    return 0;
}

static int send_mouse_report(hid_dev *idev)
{
    hid_dev *odev = &s_mouse.dev;
    hid_copy(odev, idev);
    return send_report(&s_mouse);
}

static int send_keyboard_report(hid_dev *idev)
{
    hid_dev *odev = &s_kbd.dev;
    hid_copy(odev, idev);
    return send_report(&s_kbd);
}

int output_send_report(hid_dev *dev)
{
    if (dev->type == HID_DEV_MOUSE) {
        return send_mouse_report(dev);
    } else if (dev->type == HID_DEV_KEYBOARD) {
        return send_keyboard_report(dev);
    }
    return -1;
}

static void close_output_dev(output_dev *dev)
{
    if (dev->fd >= 0) {
        close(dev->fd);
        dev->fd = -1;
    }
}

void output_close(void)
{
    close_output_dev(&s_mouse);
    close_output_dev(&s_kbd);
}

static int open_output_dev(output_dev *dev)
{
    int fd = open(dev->path, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "open failed, path=%s, errno=%d\n",
            dev->path, errno);
        return -1;
    }

    assert(dev->dev.report_length > 0);

    dev->fd = fd;
    return 0;
}

int output_open(void)
{
    int ret = -1;

    ret = open_output_dev(&s_mouse);
    if (ret < 0) {
        fprintf(stderr, "open mouse dev failed");
        return -1;
    }
    ret = open_output_dev(&s_kbd);
    if (ret < 0) {
        fprintf(stderr, "open keyboard dev failed");
        close_output_dev(&s_mouse);
        return -1;
    }

    return 0;
}
