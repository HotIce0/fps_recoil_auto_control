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

#include "bswap.h"
#include "output.h"

#define IS_MOUSE(_output_dev)   ((_output_dev)->type == OUTPUT_DEV_TYPE_MOUSE)
#define IS_KBD(_output_dev)     ((_output_dev)->type == OUTPUT_DEV_TYPE_KEYBOARD)

#define GET_MOUSE_BTN_BUFFER(_output_dev) \
    ((_output_dev)->report_buffer + (_output_dev)->mouse.btn_bytes_off)
#define GET_MOUSE_ORIEN_BUFFER(_output_dev) \
    ((_output_dev)->report_buffer + (_output_dev)->mouse.orien_bytes_off)
#define GET_MOUSE_WHEEL_BUFFER(_output_dev) \
    ((_output_dev)->report_buffer + (_output_dev)->mouse.wheel_bytes_off)

#define AXIS_X 0
#define AXIS_Y 1

typedef enum output_dev_type {
    OUTPUT_DEV_TYPE_NONE = 0,
    OUTPUT_DEV_TYPE_MOUSE,
    OUTPUT_DEV_TYPE_KEYBOARD,
} output_dev_type;

typedef struct hid_data_desc {
    int32_t physical_minimum;
	int32_t physical_maximum;
	int32_t logical_minimum;
	int32_t logical_maximum;
	uint32_t size; // bit
	uint32_t count;
} hid_data_desc;

typedef struct output_mouse {
    hid_data_desc btn;
    hid_data_desc orien;
    hid_data_desc wheel;
    // report buffer offset
    uint32_t btn_bytes_off;
    uint32_t orien_bytes_off;
    uint32_t wheel_bytes_off;
} output_mouse;

typedef struct output_kbd {
    hid_data_desc ctrl_btn;
    hid_data_desc led;
    hid_data_desc key;
    // report buffer offset
    uint32_t ctrl_btn_bytes_off;
    uint32_t led_bytes_off;
    uint32_t key_bytes_off;
} output_kbd;

typedef struct output_dev {
    const char *path;
    int fd;
    char *report_buffer;
    int report_length;
    output_dev_type type;
    union {
        output_mouse mouse;
        output_kbd kbd;
    };
} output_dev;

static output_dev s_mouse = {
    .path = "/dev/hidg0",
    .fd = -1,
    .report_buffer = NULL,
    .report_length = 7,
    .type = OUTPUT_DEV_TYPE_MOUSE,
    .mouse = {
        .btn = {
            .physical_minimum = 1,
            .physical_maximum = 16,
            .logical_minimum = 0,
            .logical_maximum = 1,
            .size = 1,
            .count = 16,
        },
        .btn_bytes_off = 0,

        .orien = {
            .physical_minimum = 0,
            .physical_maximum = 0,
            .logical_minimum = -32767,
            .logical_maximum = 32767,
            .size = 16,
            .count = 2,
        },
        .orien_bytes_off = 2,

        .wheel = {
            .physical_minimum = 0,
            .physical_maximum = 0,
            .logical_minimum = -127,
            .logical_maximum = 127,
            .size = 8,
            .count = 1,
        },
        .wheel_bytes_off = 6
    },
};

static output_dev s_kbd = {
    .path = "/dev/hidg1",
    .fd = -1,
    .report_buffer = NULL,
    .report_length = 8,
    .type = OUTPUT_DEV_TYPE_KEYBOARD,
    .kbd = {
        .ctrl_btn = {
            .physical_minimum = 0xe0,
            .physical_maximum = 0xe7,
            .logical_minimum = 0,
            .logical_maximum = 1,
            .size = 1,
            .count = 8,
        },
        .ctrl_btn_bytes_off = 0,
        .led = {
            .physical_minimum = 0x01,
            .physical_maximum = 0x05,
            .logical_minimum = 0,
            .logical_maximum = 1,
            .size = 8,
            .count = 1,
        },
        .led_bytes_off = 1,
        .key = {
            .physical_minimum = 0x00,
            .physical_maximum = 0xFF,
            .logical_minimum = 0,
            .logical_maximum = 255,
            .size = 8,
            .count = 6,
        },
        .key_bytes_off = 2,
    },
};

static int send_report(output_dev *dev)
{
    ssize_t size = write(dev->fd, dev->report_buffer, dev->report_length);
    if (size != dev->report_length) {
        fprintf(stderr, "write failed, path=%s fd=%d, len=%zu\n",
            dev->path, dev->fd, dev->report_length);
        return -1;
    }
    return 0;
}

static int mouse_get_button(output_dev *dev, int button)
{
    assert(IS_MOUSE(dev));
    char *buf = GET_MOUSE_BTN_BUFFER(dev);
    int byte_off = button / 8;
    int bit_off = button % 8;

    return buf[byte_off] & (0x01 << bit_off) ? 1: 0;
}

static void mouse_set_button(output_dev *dev, int button, int press)
{
    assert(IS_MOUSE(dev));
    char *buf = GET_MOUSE_BTN_BUFFER(dev);
    int byte_off = button / 8;
    int bit_off = button % 8;

    if (press) {
        buf[byte_off] = buf[byte_off] | (0x01 << bit_off);
    } else {
        buf[byte_off] = buf[byte_off] & ~(0x01 << bit_off);
    }
}

static int32_t mouse_get_orien(output_dev *dev, int axis)
{
    assert(IS_MOUSE(dev));
    hid_data_desc *desc = &dev->mouse.orien;
    char *buf = GET_MOUSE_ORIEN_BUFFER(dev);
    int value_bytes = desc->size / 8;

    switch (value_bytes) {
        case 1:
            return ((int8_t *)buf)[axis];
        case 2:
            return le16_to_cpu(((int16_t *)buf)[axis]);
        case 4:
            return le32_to_cpu(((int32_t *)buf)[axis]);
        default:
            assert(!"not expect");
    }
}

static void mouse_set_orien(output_dev *dev, int axis, int32_t value)
{
    assert(IS_MOUSE(dev));
    hid_data_desc *desc = &dev->mouse.orien;
    char *buf = GET_MOUSE_ORIEN_BUFFER(dev);
    int value_bytes = desc->size / 8;
    // printf("!! axis=%d, value=%d, value_bytes=%d\n", axis, value, value_bytes);
    switch (value_bytes) {
        case 1:
            ((int8_t *)buf)[axis] = (int8_t)value;
            break;
        case 2:
            ((int16_t *)buf)[axis] = cpu_to_le16((int16_t)value);
            break;
        case 4:
            ((int32_t *)buf)[axis] = cpu_to_le32((int32_t)value);
            break;
        default:
            assert(!"not expect");
    }
}

static uint32_t mouse_get_wheel(output_dev *dev)
{
    assert(IS_MOUSE(dev));
    hid_data_desc *desc = &dev->mouse.wheel;
    char *buf = GET_MOUSE_WHEEL_BUFFER(dev);
    int value_bytes = desc->size / 8;

    switch (value_bytes) {
        case 1:
            return *(int8_t *)buf;
        case 2:
            return le16_to_cpu(*(int16_t *)buf);
        case 4:
            return le32_to_cpu(*(int32_t *)buf);
        default:
            assert(!"not expect");
    }
}

static void mouse_set_wheel(output_dev *dev, uint32_t value)
{
    assert(IS_MOUSE(dev));
    hid_data_desc *desc = &dev->mouse.wheel;
    char *buf = GET_MOUSE_WHEEL_BUFFER(dev);
    int value_bytes = desc->size / 8;

    switch (value_bytes) {
        case 1:
            *(int8_t *)buf = (int8_t)value;
            break;
        case 2:
            *(int16_t *)buf = cpu_to_le16((int16_t)value);
            break;
        case 4:
            *(int32_t *)buf = cpu_to_le32((int32_t)value);
            break;
        default:
            assert(!"not expect");
    }
}

int output_mouse_move(int dx, int dy)
{
    mouse_set_orien(&s_mouse, AXIS_X, dx);
    mouse_set_orien(&s_mouse, AXIS_Y, dy);
    if (send_report(&s_mouse) < 0) {
        fprintf(stderr, "send report falied\n");
        return -1;
    }
    return 0;
}

int output_mouse_btn(int btn, output_keystate state)
{
    // TODO
    return 0;
}

int output_mouse_wheel(int wheel)
{
    // TODO
    return 0;
}

static void close_output_dev(output_dev *dev)
{
    if (dev->fd >= 0) {
        close(dev->fd);
        dev->fd = -1;
    }
    if (dev->report_buffer) {
        free(dev->report_buffer);
        dev->report_buffer = NULL;
    }
}

void output_close(void)
{
    close_output_dev(&s_mouse);
    close_output_dev(&s_kbd);
}

static int open_output_dev(output_dev *dev)
{
    char *buf;
    int fd = open(dev->path, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "open failed, path=%s, errno=%d\n",
            dev->path, errno);
        return -1;
    }

    assert(dev->report_length > 0);
    buf = (char *)malloc(dev->report_length);
    if (!buf) {
        fprintf(stderr, "allocate report_buffer failed, len=%d\n",
            dev->report_length);
        close(fd);
        return -1;
    }
    memset(buf, 0, dev->report_length);

    dev->fd = fd;
    dev->report_buffer = buf;
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

// #define OUTPUT_C_TEST
#ifdef OUTPUT_C_TEST
int main(void)
{
    int ret = output_open();
    if (ret < 0) {
        fprintf(stderr, "output open failed, ret=%d\n", ret);
        return -1;
    }

    printf("move 100, 0\n");
    output_mouse_move(100, 0);
    sleep(2);
    printf("move -100, 0\n");
    output_mouse_move(-100, 0);

    output_close();
    return 0;
}
#endif /* OUTPUT_C_TEST */
