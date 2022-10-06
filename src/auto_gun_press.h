#ifndef __AUTO_GUN_PRESS_H__
#define __AUTO_GUN_PRESS_H__

#include "hid.h"

int agp_kbd_event_callback(void *user_data, hid_dev *dev, int has_data);
int agp_mouse_event_callback(void *user_data, hid_dev *dev, int has_data);
int agp_init(void);
void agp_exit(void);

#endif // __AUTO_GUN_PRESS_H__