#include "buzzert.h"

static bool capsesc_valid = false;
static uint16_t capsesc_begin = 0;
bool process_record_user(uint16_t keycode, keyrecord_t *record)
{
    if (keycode == KB_CAPSESC) {
        if (record->event.pressed) {
            // Pressed 
            register_code(KC_LCTL); // Send control on pressed
            capsesc_begin = timer_read();
            capsesc_valid = true;
        } else {
            // Released
            unregister_code(KC_LCTL); // Release control

            // If time pressed and released is short enough, send escape
            uint16_t capsesc_end = timer_read();
            if ( capsesc_valid && (capsesc_end - capsesc_begin) < 300 ) {
                tap_code(KC_ESC);
            }
        }
    } else {
        capsesc_valid = false;
    }

    return true;
}
