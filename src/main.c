#include <stdio.h>
#include <assert.h>

#include "log.h"
#include "input.h"
#include "output.h"
#include "auto_gun_press.h"

void input_event_callback(void *user_data, hid_dev *dev, int has_data)
{
    int ret;
    if (dev->type == HID_DEV_MOUSE) {
        ret = agp_mouse_event_callback(user_data, dev, has_data);
    } else {
        ret = agp_kbd_event_callback(user_data, dev, has_data);
    }
    if (ret) {
        output_send_report(dev);
    }
}

int main(void)
{
    int ret = -1;

    ret = input_open();
    if (ret < 0) {
        log_err("input_open failed, ret=%d", ret);
        goto out;
    }
    ret = output_open();
    if (ret < 0) {
        log_err("output_open failed, ret=%d", ret);
        goto out;
    }
    ret = agp_init();
    if (ret < 0) {
        log_err("agp_init failed, ret=%d", ret);
        goto out;
    }

    input_set_handle(input_event_callback, NULL);
    input_event_loop();

out:
    agp_exit();
    output_close();
    input_close();
    return 0;
}