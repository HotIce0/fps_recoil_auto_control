#ifndef __HID_H__
#define __HID_H__

#include <stdint.h>
#include <assert.h>

#define REPORT_BUFFER_SIZE  20


#define HID_MOUSE_BUTTON_LEFT    0
#define HID_MOUSE_BUTTON_RIGHT   1
#define HID_MOUSE_BUTTON_MIDDLE  2
#define HID_MOUSE_BUTTON_EX_BASE 3

#define HID_MOUSE_AXIS_X 0
#define HID_MOUSE_AXIS_Y 1


// Keyboard page Usage ID
// ref: 10 Keyboard/Keypad Page (0x07)
//      HIDUsageTables1.22.pdf

// Control Key
// Usage: bitmap offset = (HID_KBD_LEFT_CTRL & HID_KBD_CTRL_BM_OFF_MASK)
#define HID_KBD_CTRL_BM_OFF_MASK 0x0F
#define HID_KBD_LEFT_CTRL   0xE0
#define HID_KBD_LEFT_SHIFT  0xE1
#define HID_KBD_LEFT_ALT    0xE2
#define HID_KBD_LEFT_GUI    0xE3
#define HID_KBD_RIGHT_CTRL  0xE4
#define HID_KBD_RIGHT_SHIFT 0xE5
#define HID_KBD_RIGHT_ALT   0xE6
#define HID_KBD_RIGHT_GUI   0xE7

// Normal Key
// Usage: HID_KBD_LETTER('a'), Just support lowercase.
#define HID_KBD_LETTER(x) ((uint8_t)((x) - 0x5D))
// Usage: HID_KBD_NUMBER(1)
#define HID_KBD_NUMBER(x) ((x) == 0 ? 0x27: ((x) + 0x1D))
// Usage: HID_KBD_FX(2)
#define HID_KBD_FX(x) ((uint8_t)((x) + 0x39))

#define HID_KBD_ENTER      0x28
#define HID_KBD_SPACE      0x2C
#define HID_KBD_BACKSPACE  0x2A
// -
#define HID_KBD_MINUS      0x2D
// =
#define HID_KBD_EQUAL      0x2E
#define HID_KBD_CAPS       0x39
#define HID_KBD_DEL        0x4C
#define HID_KBD_HOME       0x4A
#define HID_KBD_END        0x4D
#define HID_KBD_PAGEUP     0x4B
#define HID_KBD_PAGEDOWN   0x4E


enum hid_dev_type {
    HID_DEV_NONE,
    HID_DEV_MOUSE,
    HID_DEV_KEYBOARD,
};

enum axis_orien {
    AXIS_X = 0,
    AXIS_Y,
};

typedef struct hid_report_desc {
    int32_t physical_minimum;
	int32_t physical_maximum;
	int32_t logical_minimum;
	int32_t logical_maximum;
	uint32_t size; // bit
	uint32_t count;
    int report_buffer_offset;
} hid_report_desc;

typedef struct hid_dev {
    char report_buffer[REPORT_BUFFER_SIZE];
    char report_buffer_last[REPORT_BUFFER_SIZE];
    int report_length;
    
    int type; /* \ref hid_dev_type */
    union {
        struct {
            hid_report_desc btn;
            hid_report_desc orien;
            hid_report_desc wheel;
        } mouse;
        struct {
            hid_report_desc ctrl_btn;
            hid_report_desc led;
            hid_report_desc key;
        } kbd;
    };
} hid_dev;

int hid_mouse_get_button(hid_dev *dev, int button);
void hid_mouse_set_button(hid_dev *dev, int button, int press);
/**
 * @brief 
 * 
 * @param dev 
 * @param axis \ref axis_orien
 * @return int32_t 
 */
int32_t hid_mouse_get_orien(hid_dev *dev, int axis);
void hid_mouse_set_orien(hid_dev *dev, int axis, int32_t value);
uint32_t hid_mouse_get_wheel(hid_dev *dev);
void hid_mouse_set_wheel(hid_dev *dev, uint32_t value);

int hid_kbd_is_key_press(hid_dev *dev, uint32_t key_code);

int hid_copy(hid_dev *dst, hid_dev *src);

#endif // __HID_H__