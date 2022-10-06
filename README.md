# fps_recoil_auto_control
> redirect the input data from raspberrypi to your computer via a usb hid device (mouse and keyboard functions). and modify the input data in the process of redirect to achieve recoil auto control.

# Requirements
- raspberrypi 4B

# First Use
### 1. setup raspi4b usb device
1. Add `dtoverlay=dwc2` to the /boot/config.txt
2. Add `modules-load=dwc2` to the end of /boot/cmdline.txt
3. If you have not already enabled ssh then create a empty file called `ssh` in /boot
4. Add `libcomposite` to /etc/modules
5. reboot your pi
6. execute script `scripts/usbhid_dev_install.sh`

### 2. resolve problem of pi4b, usb inttrupt transfer slow
1. add `usbhid.mousepoll=0` ot `/boot/cmdline.txt`
2. reboot your pi

### 3. modify `s_mouse` and `s_kbd` in input.c for your mouse and keyboard
1. 
    > modify the `struct hid_report_desc`

### 4. build
1. install dependencies 
    > sudo apt install libudev-dev libhidapi-dev

# Working principle
1. read mouse, keyboard input by hidapi (use libusb backend)
2. modify input data
    > ref: https://github.com/HotIce0/HIDDeviceDataDynamicModification
3. send hid report via usb hid device (mouse and keyboard compsite device)
    > usb hid device report format
    > 1. mouse
    > ```c
    > // PVID=046D:C092 G102 LIGHTSYNC Gaming Mouse
    > // {00 00}          {05 00} {00 00} {00}
    > // btn(bit map)     x       y       wheel
    > ```
    > 2. keyboard
    > ```c
    > // PVID=0951:16D2 HyperX Alloy FPS Pro Mechanical Gaming Keyboard
    > // {00}  {00}  {05 00 00 00 00 00}
    > // CTRL  LED   KEYCODE(6 key)
    > ```

# TODO
1. support auto fill input device report descriptor, when hidapi support get report descriptor.
2. support device hotplug callback (wait hidapi support)