#include <libinput.h>

typedef struct input_event_handlers {
    /**
     * @brief 
     * @param type options, LIBINPUT_EVENT_POINTER_MOTION
     *                      LIBINPUT_EVENT_POINTER_BUTTON
     */
    void (*mouse_ev_cb)(struct libinput_event_pointer *event, enum libinput_event_type type);

    /**
     * @brief 
     * @param type options, LIBINPUT_EVENT_KEYBOARD_KEY
     */
    void (*keyboard_ev_cb)(struct libinput_event_keyboard *event, enum libinput_event_type type);
} input_event_handlers;

int input_event_handle_loop(input_event_handlers handlers);