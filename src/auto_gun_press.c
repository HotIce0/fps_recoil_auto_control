#include "log.h"
#include "agp_gen.h"
#include "auto_gun_press.h"

static AGPContext *s_ctx;
static int s_passthrough = 1;
static int s_collect_idx = 0;

void agp_exit(void)
{
    agp_close(s_ctx);
}

int agp_init(void)
{
    return agp_open(&s_ctx);
}

int agp_kbd_event_callback(void *user_data, hid_dev *dev, int has_data)
{
    log_debug("");
    if (!has_data) {
        return 0;
    }
    //  enable disable [CAPS]
    if (hid_kbd_is_key_press(dev, HID_KBD_CAPS)) {
        s_passthrough = !s_passthrough;
        if (s_passthrough) {
            agp_restart(s_ctx);
        }
        log_info("passthrough=%d", s_passthrough);
    }
    // switch gun [DEL]
    if (hid_kbd_is_key_press(dev, HID_KBD_DEL)) {
        s_collect_idx++;
        if (s_collect_idx >= AGP_COLLECT_IDX_MAX) {
            s_collect_idx = 0;
        }
        agp_set_collect(s_ctx, s_collect_idx);
        log_info("switch agp data: %s", agp_collect_str(s_collect_idx));
    }
    // change coefficent
    if (hid_kbd_is_key_press(dev, HID_KBD_PAGEUP)) {
        agp_coefficient_change(s_ctx, 1);
    }
    if (hid_kbd_is_key_press(dev, HID_KBD_PAGEDOWN)) {
        agp_coefficient_change(s_ctx, 0);
    }
    // change sensitive
    if (hid_kbd_is_key_press(dev, HID_KBD_HOME)) {
        agp_sensitive_change(s_ctx, 1);
    }
    if (hid_kbd_is_key_press(dev, HID_KBD_END)) {
        agp_sensitive_change(s_ctx, 0);
    }
    return 1;
}

int agp_mouse_event_callback(void *user_data, hid_dev *dev, int has_data)
{
    log_debug("");
    if (s_passthrough) {
        goto out;
    }

    if (hid_mouse_get_button(dev, HID_MOUSE_BUTTON_LEFT)) {
        AGPData data = {0};
        int ret;

        ret = agp_get_data(s_ctx, &data);
        if (ret < 0) {
            // log_err("agp get data failed, ret=%d", ret);
            goto out;
        }
        if (has_data) {
            data.x += hid_mouse_get_orien(dev, HID_MOUSE_AXIS_X);
            data.y += hid_mouse_get_orien(dev, HID_MOUSE_AXIS_Y);
        }
        
        hid_mouse_set_orien(dev, HID_MOUSE_AXIS_X, data.x);
        hid_mouse_set_orien(dev, HID_MOUSE_AXIS_Y, data.y);
        return 1;
    } else {
        agp_restart(s_ctx);
    }

out:
    if (has_data) {
        return 1;
    }
    return 0;
}