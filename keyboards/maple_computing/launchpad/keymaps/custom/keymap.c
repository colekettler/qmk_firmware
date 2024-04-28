// Below layout is based upon /u/That-Canadian's planck layout
#include QMK_KEYBOARD_H

extern keymap_config_t keymap_config;

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
enum layer_names {
    _QWERTY,
    _RGB,
    _FUNC
};

// Defines for task manager and such
#define CALTDEL LCTL(LALT(KC_DEL))
#define TSKMGR LCTL(LSFT(KC_ESC))

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* Qwerty
     * ,-------------.
     * |   1  |  2   |
     * |------+------|
     * |   3  |  4   |
     * |------+------|
     * |   5  |  6   |
     * |------+------|
     * | FUNC | RGB  |
     * `-------------'
     */
    [_QWERTY] = LAYOUT(
        KC_1,      KC_2,
        KC_3,      KC_4,
        KC_5,      KC_6,
        MO(_FUNC), TG(_RGB)
      ),

    /* RGB
     * ,-------------.
     * | Mode-| Mode+|
     * |------+------|
     * | HUE- | HUE+ |
     * |------+------|
     * | SAT- | SAT+ |
     * |------+------|
     * |RGBTOG|      |
     * `-------------'
     */
    [_RGB] = LAYOUT(
        RGB_RMOD, RGB_MOD,
        RGB_HUD,  RGB_HUI,
        RGB_SAD,  RGB_SAI,
        RGB_TOG,  KC_TRNS
      ),

    /* Function
     * ,-------------.
     * |   Q  |CALDEL|
     * |------+------|
     * |   A  |TSKMGR|
     * |------+------|
     * |   Z  |  X   |
     * |------+------|
     * |      |  C   |
     * `-------------'
     */
    [_FUNC] = LAYOUT(
        KC_Q,    CALTDEL,
        KC_A,    TSKMGR,
        KC_Z,    KC_X,
        _______, KC_C
      )

};

void matrix_init_user(void) {}

// RGB timeout

static uint32_t key_timer;           // timer for last keyboard activity, use 32bit value and function to make longer idle time possible
static void refresh_rgb(void);       // refreshes the activity timer and RGB, invoke whenever any activity happens
static void check_rgb_timeout(void); // checks if enough time has passed for RGB to timeout
bool is_rgb_timeout = false;         // store if RGB has timed out or not in a boolean

void refresh_rgb(void) {
    key_timer = timer_read32(); // store time of last refresh
    if (is_rgb_timeout)
    {
        is_rgb_timeout = false;
        rgblight_wakeup();
    }
}
void check_rgb_timeout(void) {
    if (!is_rgb_timeout && timer_elapsed32(key_timer) > RGBLIGHT_TIMEOUT) // check if RGB has already timeout and if enough time has passed
    {
        rgblight_suspend();
        is_rgb_timeout = true;
    }
}
/* Then, call the above functions from QMK's built in post processing functions like so */
/* Runs at the end of each scan loop, check if RGB timeout has occured or not */
void housekeeping_task_user(void) {
#ifdef RGBLIGHT_TIMEOUT
    check_rgb_timeout();
#endif
}
/* Runs after each key press, check if activity occurred */
void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef RGBLIGHT_TIMEOUT
    if (record->event.pressed)
        refresh_rgb();
#endif
}
/* Runs after each encoder tick, check if activity occurred */
void post_encoder_update_user(uint8_t index, bool clockwise) {
#ifdef RGBLIGHT_TIMEOUT
    refresh_rgb();
#endif
}
