#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

#include <sys/types.h> /* open */
#include <sys/stat.h> /* open */
#include <fcntl.h> /* open */
#include <unistd.h> /* close */

#include "input.h"

#define SEAT_ID "seat0"

static int open_restricted(const char *path, int flags, void *user_data)
{
    int fd = open(path, flags);
    printf("open_path=%s\n", path);
    return fd < 0 ? -errno : fd;
}

static void close_restricted(int fd, void *user_data)
{
    close(fd);
}

static const struct libinput_interface s_interface = {
    .open_restricted = open_restricted,
    .close_restricted = close_restricted,
};

int input_event_handle_loop(input_event_handlers handlers)
{
    struct udev *udev = udev_new();
    struct libinput *li;
    struct libinput_event *event;
    int ret = -1;

    li = libinput_udev_create_context(&s_interface, NULL, udev);
    if (!li) {
        fprintf(stderr, "libinput_udev_create_context failed\n");
        udev_unref(udev);
        return -1;
    }
    ret = libinput_udev_assign_seat(li, SEAT_ID);
    if (ret < 0) {
        fprintf(stderr, "libinput_udev_assign_seat(id=%s) failed, ret=%d\n", SEAT_ID, ret);
        return -1;
    }
    
    while (1) {
        enum libinput_event_type type = LIBINPUT_EVENT_NONE;
        libinput_dispatch(li);
        event = libinput_get_event(li);
        if (!event) {
            continue;
        }
        type = libinput_event_get_type(event);
        
        if (type == LIBINPUT_EVENT_POINTER_MOTION ||
                type == LIBINPUT_EVENT_POINTER_BUTTON) {
            struct libinput_event_pointer *ev = libinput_event_get_pointer_event(event);
            if (handlers.mouse_ev_cb) {
                handlers.mouse_ev_cb(ev, type);
            }
        }
        if (type == LIBINPUT_EVENT_KEYBOARD_KEY) {
            struct libinput_event_keyboard *ev = libinput_event_get_keyboard_event(event);
            if (handlers.keyboard_ev_cb) {
                handlers.keyboard_ev_cb(ev, type);
            }
        }

        libinput_event_destroy(event);
    }
 
    libinput_unref(li);
    udev_unref(udev);
    return 0;
}