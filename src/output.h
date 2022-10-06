#include "hid.h"

int output_open(void);
void output_close(void);

int output_send_report(hid_dev *dev);
