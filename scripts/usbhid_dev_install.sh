#!/bin/bash

cd /sys/kernel/config/usb_gadget/ || exit -1
mkdir -p g1
cd g1

# device desc
echo 0x046d > idVendor # Logitech
echo 0xc092 > idProduct # G102 LIGHTSYNC Gaming Mouse
echo 0x5200 > bcdDevice # v2.00
echo 0x0200 > bcdUSB # USB2
echo 0x00 > bDeviceClass
echo 0x00 > bDeviceProtocol

# strings
mkdir -p strings/0x409
echo "205E31735242" > strings/0x409/serialnumber
echo "Logitech" > strings/0x409/manufacturer
echo "G102 LIGHTSYNC Gaming Mouse" > strings/0x409/product

# configrations
mkdir -p configs/c.1
mkdir -p configs/c.1/strings/0x409
echo "U152.00_B0010           " > configs/c.1/strings/0x409/configuration
echo 250 > configs/c.1/MaxPower

# function1 mouse
mkdir -p functions/hid.usb0
echo 0x02 > functions/hid.usb0/protocol # mouse
echo 0x01 > functions/hid.usb0/subclass # boot interface
echo 7 > functions/hid.usb0/report_length
echo -ne \
\\x05\\x01\\x09\\x02\\xa1\\x01\\x09\\x01\\xa1\\x00\\x05\\x09\\x19\\x01\\x29\\x10\\x15\\x00\\x25\\x01\\x95\\x10\\x75\\x01\\x81\\x02\\x05\\x01\\x16\\x01\\x80\\x26\\xff\\x7f\\x75\\x10\\x95\\x02\\x09\\x30\\x09\\x31\\x81\\x06\\x15\\x81\\x25\\x7f\\x75\\x08\\x95\\x01\\x09\\x38\\x81\\x06\\xc0\\xc0 \
 > functions/hid.usb0/report_desc
ln -s functions/hid.usb0 configs/c.1/

# function2 keyboard
mkdir -p functions/hid.usb1
echo 0x01 > functions/hid.usb1/protocol # keyboard
echo 0x01 > functions/hid.usb1/subclass # boot interface
echo 8 > functions/hid.usb1/report_length
echo -ne \\x05\\x01\\x09\\x06\\xa1\\x01\\x05\\x07\\x19\\xe0\\x29\\xe7\\x15\\x00\\x25\\x01\\x75\\x01\\x95\\x08\\x81\\x02\\x95\\x01\\x75\\x08\\x81\\x01\\x95\\x05\\x75\\x01\\x05\\x08\\x19\\x01\\x29\\x05\\x91\\x02\\x95\\x01\\x75\\x03\\x91\\x01\\x95\\x06\\x75\\x08\\x15\\x00\\x26\\xff\\x00\\x05\\x07\\x19\\x00\\x2a\\xff\\x00\\x81\\x00\\x05\\x0c\\x09\\x00\\x15\\x80\\x25\\x7f\\x95\\x40\\x75\\x08\\xb1\\x02\\xc0 \
 > functions/hid.usb1/report_desc
ln -s functions/hid.usb1 configs/c.1/

ls /sys/class/udc > UDC
