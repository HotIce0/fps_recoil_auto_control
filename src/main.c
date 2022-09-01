#include <stdio.h>
#include <assert.h>

#include "input.h"
#include "output.h"


static void mouse_event_cb(struct libinput_event_pointer *event, enum libinput_event_type type)
{
    if (type == LIBINPUT_EVENT_POINTER_MOTION) {
        int dx = (int)libinput_event_pointer_get_dx_unaccelerated(event);
        int dy = (int)libinput_event_pointer_get_dy_unaccelerated(event);
        // printf("move dx=%d, dy=%d\n", dx, dy);
        output_mouse_move(dx, dy);
        // TODO fix motion leak

    } else if (type == LIBINPUT_EVENT_POINTER_BUTTON) {
        printf("btn %d, pressed=%d\n", 
            (int)libinput_event_pointer_get_button(event),
            (int)libinput_event_pointer_get_button_state(event)
        );

    } else {
        assert(!"not expect");
    }
}

static void kbd_event_cb(struct libinput_event_keyboard *event, enum libinput_event_type type)
{
    if (type != LIBINPUT_EVENT_KEYBOARD_KEY) {
        assert(!"not expect");
    }
    printf("key %d, pressed=%d\n", 
        (int)libinput_event_keyboard_get_key(event),
        (int)libinput_event_keyboard_get_key_state(event)
    );
}

static const input_event_handlers s_handlers = {
    .mouse_ev_cb = mouse_event_cb,
    .keyboard_ev_cb = kbd_event_cb
};

int main(void)
{
    int ret = -1;

    ret = output_open();
    if (ret < 0) {
        fprintf(stderr, "output_open failed, ret=%d\n", ret);
        return -1;
    }

    // TODO hotkey to break the loop
    ret = input_event_handle_loop(s_handlers);
    if (ret < 0) {
        fprintf(stderr, "input_event_handle_loop failed, ret=%d\n", ret);
        output_close();
        return -1;
    }

    output_close();
    return 0;
}