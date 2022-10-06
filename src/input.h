#include "hid.h"

typedef void (*input_event_cb)(void *user_data, hid_dev *dev, int has_data);


int input_open(void);
void input_close(void);

void input_set_handle(input_event_cb cb, void *user_data);
int input_event_loop(void);
