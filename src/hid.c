#include <string.h>

#include "log.h"
#include "hid.h"

#include "bswap.h"

// #define LOG_DUMP
#ifdef LOG_DUMP
#include <stdio.h>
#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
void dump_hex(const uint8_t *ptr, uint16_t buflen)
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
#define dump_hex(_p, _len)
#endif

#define GET_MOUSE_BTN_BUF(_dev) \
    ((_dev)->report_buffer + (_dev)->mouse.btn.report_buffer_offset)
#define GET_MOUSE_ORIEN_BUF(_dev) \
    ((_dev)->report_buffer + (_dev)->mouse.orien.report_buffer_offset)
#define GET_MOUSE_WHEEL_BUF(_dev) \
    ((_dev)->report_buffer + (_dev)->mouse.wheel.report_buffer_offset)

#define GET_KBD_CTRL_BTN_BUF(_dev) \
    ((_dev)->report_buffer + (_dev)->kbd.ctrl_btn.report_buffer_offset)
#define GET_KBD_LED_BUF(_dev) \
    ((_dev)->report_buffer + (_dev)->kbd.led.report_buffer_offset)
#define GET_KBD_KEY_BUF(_dev) \
    ((_dev)->report_buffer + (_dev)->kbd.key.report_buffer_offset)

#define GET_DESC_BYTES(_desc) \
    (((_desc)->size * (_desc)->count) / 8)


int hid_kbd_copy(hid_dev *dst, hid_dev *src)
{
    if (GET_DESC_BYTES(&dst->kbd.ctrl_btn) != GET_DESC_BYTES(&src->kbd.ctrl_btn)) {
        log_warn("dst ctrl_btn desc bytes diff src, %d, %d",
                    GET_DESC_BYTES(&dst->kbd.ctrl_btn), GET_DESC_BYTES(&src->kbd.ctrl_btn));
    }
    if (GET_DESC_BYTES(&dst->kbd.key) != GET_DESC_BYTES(&src->kbd.key)) {
        log_warn("dst key desc bytes diff src, %d, %d",
                    GET_DESC_BYTES(&dst->kbd.key), GET_DESC_BYTES(&src->kbd.key));
    }
    memcpy(GET_KBD_CTRL_BTN_BUF(dst), GET_KBD_CTRL_BTN_BUF(src),
        GET_DESC_BYTES(&dst->kbd.ctrl_btn));
    memcpy(GET_KBD_KEY_BUF(dst), GET_KBD_KEY_BUF(src),
        GET_DESC_BYTES(&dst->kbd.key));
    return 0;
}

int hid_mouse_copy(hid_dev *dst, hid_dev *src)
{
    if (GET_DESC_BYTES(&dst->mouse.btn) != GET_DESC_BYTES(&src->mouse.btn)) {
        log_warn("dst btn desc bytes diff src, %d, %d",
                    GET_DESC_BYTES(&dst->mouse.btn), GET_DESC_BYTES(&src->mouse.btn));
    }
    if (GET_DESC_BYTES(&dst->mouse.orien) != GET_DESC_BYTES(&src->mouse.orien)) {
        log_warn("dst orien desc bytes diff src, %d, %d",
                    GET_DESC_BYTES(&dst->mouse.orien), GET_DESC_BYTES(&src->mouse.orien));
    }
    if (GET_DESC_BYTES(&dst->mouse.wheel) != GET_DESC_BYTES(&src->mouse.wheel)) {
        log_warn("dst wheel desc bytes diff src, %d, %d",
                    GET_DESC_BYTES(&dst->mouse.wheel), GET_DESC_BYTES(&src->mouse.wheel));
    }
    log_debug("btn_len=%d, orien_len=%d, wheel_len=%d",
        GET_DESC_BYTES(&dst->mouse.btn),
        GET_DESC_BYTES(&dst->mouse.orien),
        GET_DESC_BYTES(&dst->mouse.wheel));
    log_debug("DUMP SRC");
    dump_hex(src->report_buffer, src->report_length);

    memcpy(GET_MOUSE_BTN_BUF(dst), GET_MOUSE_BTN_BUF(src),
        GET_DESC_BYTES(&dst->mouse.btn));
    memcpy(GET_MOUSE_ORIEN_BUF(dst), GET_MOUSE_ORIEN_BUF(src),
        GET_DESC_BYTES(&dst->mouse.orien));
    memcpy(GET_MOUSE_WHEEL_BUF(dst), GET_MOUSE_WHEEL_BUF(src),
        GET_DESC_BYTES(&dst->mouse.wheel));
    
    log_debug("DUMP DST");
    dump_hex(dst->report_buffer, dst->report_length);
    return 0;
}

int hid_copy(hid_dev *dst, hid_dev *src)
{
    if (src->type != dst->type) {
        return -1;
    }
    if (src->type == HID_DEV_MOUSE) {
        return hid_mouse_copy(dst, src);
    } else if (src->type == HID_DEV_KEYBOARD) {
        return hid_kbd_copy(dst, src);
    }
    return -1;
}

int hid_mouse_get_button(hid_dev *dev, int button)
{
    char *buf = dev->report_buffer + dev->mouse.btn.report_buffer_offset;
    int byte_off = button / 8;
    int bit_off = button % 8;

    return buf[byte_off] & (0x01 << bit_off) ? 1: 0;
}

void hid_mouse_set_button(hid_dev *dev, int button, int press)
{
    char *buf = dev->report_buffer + dev->mouse.btn.report_buffer_offset;
    int byte_off = button / 8;
    int bit_off = button % 8;

    if (press) {
        buf[byte_off] = buf[byte_off] | (0x01 << bit_off);
    } else {
        buf[byte_off] = buf[byte_off] & ~(0x01 << bit_off);
    }
}

int32_t hid_mouse_get_orien(hid_dev *dev, int axis)
{
    hid_report_desc *desc = &dev->mouse.orien;
    char *buf = dev->report_buffer + dev->mouse.orien.report_buffer_offset;
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

void hid_mouse_set_orien(hid_dev *dev, int axis, int32_t value)
{
    hid_report_desc *desc = &dev->mouse.orien;
    char *buf = dev->report_buffer + dev->mouse.orien.report_buffer_offset;
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

uint32_t hid_mouse_get_wheel(hid_dev *dev)
{
    hid_report_desc *desc = &dev->mouse.wheel;
    char *buf = dev->report_buffer + dev->mouse.wheel.report_buffer_offset;
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

void hid_mouse_set_wheel(hid_dev *dev, uint32_t value)
{
    hid_report_desc *desc = &dev->mouse.wheel;
    char *buf = dev->report_buffer + dev->mouse.wheel.report_buffer_offset;
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

int hid_kbd_is_key_press(hid_dev *dev, uint32_t key_code)
{
    hid_report_desc *desc = &dev->kbd.key;
    char *buf = dev->report_buffer + dev->kbd.key.report_buffer_offset;
    int i;
    for (i = 0; i < desc->count; i++) {
        if (buf[i] == key_code) {
            return 1;
        }
    }
    return 0;
}