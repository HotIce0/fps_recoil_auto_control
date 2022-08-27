#!/bin/bash
cd /sys/kernel/config/usb_gadget/g1
echo "" > UDC

rm configs/c.1/hid.usb0
rm configs/c.1/hid.usb1
rmdir configs/c.1/strings/0x409/
rmdir configs/c.1/
rmdir functions/hid.usb0/
rmdir functions/hid.usb1/
rmdir strings/0x409/

cd ..
rmdir g1