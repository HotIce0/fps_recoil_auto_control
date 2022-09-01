
typedef enum output_keystate {
    OUTPUT_KEYSTATE_RELEASE = 0,
    OUTPUT_KEYSTATE_PRESSED = 1,
} output_keystate;

int output_open(void);
void output_close(void);

int output_mouse_move(int dx, int dy);
int output_mouse_btn(int btn, output_keystate state);
int output_mouse_wheel(int wheel);