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
5. reboot you pi
6. execute script `scripts/usbhid_dev_install.sh`

### 2. build
1. install dependencies 
> sudo apt install libudev-dev libmtdev-dev libevdev-dev libwacom-dev libgtk-3-dev

# Working principle
1. read mouse, keyboard input by /dev/input/eventX
2. modify input data
3. send to usb hid device (mouse and keyboard)
> you need to disable the mouse and keyboard input, To avoid the mouse and keyboard act on linux input system.

> Diable mouse, keyboard input: ref https://lxadm.com/Disable_/_enable_keyboard_and_mouse_in_Linux

# hid report format
1. mouse
```c
// PVID=046D:C092 G102 LIGHTSYNC Gaming Mouse
// {00 00}          {05 00} {00 00} {00}
// btn(bit map)     x       y       wheel
```
2. keyboard
```c
// PVID=0951:16D2 HyperX Alloy FPS Pro Mechanical Gaming Keyboard
// {00}  {00}  {05 00 00 00 00 00}
// CTRL  LED   KEYCODE(6 key)
```

# ref: https://github.com/HotIce0/HIDDeviceDataDynamicModification