#include <stdint.h>
#include <string.h>
#include <hidapi.h>

#include "log.h"
#include "hid.h"
#include "input.h"

#define IS_EXPECT_DEV(_info, _input) \
    (((_info)->vendor_id == (_input)->vid) && \
     ((_info)->product_id == (_input)->pid) && \
     ((_info)->interface_number == (_input)->interface_number))

typedef struct input_dev {
    hid_device *hidapi;

    hid_dev dev;
    uint16_t vid;
    uint16_t pid;
    int interface_number;
} input_dev;

input_event_cb s_cb = NULL;
void *s_user_data = NULL;

static input_dev s_mouse = {
    .hidapi = NULL,
    .vid = 0x046d,
    .pid = 0xc539,
    .interface_number = 1,
    .dev = {
        .report_length = 9,
        .type = HID_DEV_MOUSE,
        .mouse = {
            .btn = {
                .physical_minimum = 1,
                .physical_maximum = 16,
                .logical_minimum = 0,
                .logical_maximum = 1,
                .size = 1,
                .count = 16,
                .report_buffer_offset = 1
            },
            .orien = {
                .physical_minimum = 0,
                .physical_maximum = 0,
                .logical_minimum = -32767,
                .logical_maximum = 32767,
                .size = 16,
                .count = 2,
                .report_buffer_offset = 3
            },
            .wheel = {
                .physical_minimum = 0,
                .physical_maximum = 0,
                .logical_minimum = -127,
                .logical_maximum = 127,
                .size = 8,
                .count = 1,
                .report_buffer_offset = 7
            }
        }
    }
};

static input_dev s_kbd = {
    .hidapi = NULL,
    .vid = 0x0951,
    .pid = 0x16d2,
    .interface_number = 0,
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

int input_event_loop(void)
{
    input_dev *devs[2] = {&s_mouse, &s_kbd};
    int i = 0;
    
    while (1) {
        if (!devs[i]->hidapi) {
            goto next;
        }
        int ret = hid_read_timeout(devs[i]->hidapi,
            (unsigned char *)devs[i]->dev.report_buffer,
            devs[i]->dev.report_length,
            0);
        if (ret < 0) {
            log_err("hid read failed, pvid=%04x:%04x type=%d, ret=%d",
                devs[i]->vid, devs[i]->pid, devs[i]->dev.type, ret);
            return -1;
        }

        log_debug("read report idx=%d, actual_len=%d, report_length=%d",
            i, ret, devs[i]->dev.report_length);

        s_cb(s_user_data, &devs[i]->dev, !!ret);

        if (ret > 0) {
            memcpy(devs[i]->dev.report_buffer_last, 
                devs[i]->dev.report_buffer,
                devs[i]->dev.report_length);
        }

    next:
        i++;
        if (i >= 2) {
            i = 0;
        }
    }
    return 0;
}

void input_set_handle(input_event_cb cb, void *user_data)
{
    s_cb = cb;
    s_user_data = user_data;
}

int input_open(void)
{
    int ret;
    int find_cnt = 0;
    struct hid_device_info *root;
    struct hid_device_info *cur;

    ret = hid_init();
    if (ret < 0) {
        log_err("hid init failed, ret=%d", ret);
        return -1;
    }

    root = hid_enumerate(0, 0);
    if (!root) {
        log_err("hid enumerate failed, no device\n");
        hid_exit();
        return -1;
    }

    cur = root;
    while (cur) {
        log_info("path=%s, bus_type=%d, pvid=%04x:%04x, interface_number=%d, usage=%d, usage_page=%d\n",
            cur->path, cur->bus_type,
            cur->vendor_id, cur->product_id,
            cur->interface_number,
            cur->usage, cur->usage_page);
        if (!IS_EXPECT_DEV(cur, &s_mouse) &&
            !IS_EXPECT_DEV(cur, &s_kbd)) {
            goto next;
        }

        hid_device *dev = hid_open_path(cur->path);
        if (!dev) {
            log_err("hid open failed, path=%s\n", cur->path);
            return -1;
        }
        if (IS_EXPECT_DEV(cur, &s_mouse)) {
            log_err("open mouse success, path=%s\n", cur->path);
            s_mouse.hidapi = dev;
        }
        if (IS_EXPECT_DEV(cur, &s_kbd)) {
            log_err("open keyboard success, path=%s\n", cur->path);
            s_kbd.hidapi = dev;
        }
        find_cnt++;
    next:
        cur = cur->next;
    };

    hid_free_enumeration(root);

    if (find_cnt != 2) {
        hid_close(s_mouse.hidapi);
        hid_close(s_kbd.hidapi);
        s_mouse.hidapi = NULL;
        s_kbd.hidapi = NULL;
        log_err("find cnt = %d", find_cnt);
        return -1;
    }

    return 0;
}

void input_close(void)
{
    hid_close(s_mouse.hidapi);
    hid_close(s_kbd.hidapi);
    s_mouse.hidapi = NULL;
    s_kbd.hidapi = NULL;
    hid_exit();
}