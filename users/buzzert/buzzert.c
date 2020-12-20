#include "buzzert.h"

static const uint16_t tap_time_ms = 300;

typedef struct th_record_ {
    uint16_t trigger_key;

    bool     is_valid;
    uint16_t begin_time;
} th_record_t; 

#define MAX_TH_RECORDS 8
static th_record_t th_records[MAX_TH_RECORDS] = { 0 };
static uint8_t th_records_end = 0;

static th_record_t* record_for_trigger_key(uint16_t trigger)
{
    th_record_t *record = NULL;

    // Search for existing record
    for (uint8_t i = 0; i < th_records_end; i++) {
        if (th_records[i].trigger_key == trigger) {
            record = &th_records[i];
            break;
        }
    }

    // Insert new record
    if (record == NULL) {
        th_records[th_records_end] = (th_record_t) {
            .trigger_key = trigger,
            .is_valid = false,
            .begin_time = 0,
        };
        th_records_end++;
    }

    return record;
}

void handle_tap_or_hold_multi(
        uint16_t keycode_in, keyrecord_t *record, 
        uint16_t trigger_key,
        uint16_t key_on_pressed, uint16_t *keys_on_hold, uint8_t num_keys_on_hold)
{
    th_record_t *th_record = record_for_trigger_key(trigger_key);
    if (keycode_in == trigger_key) {
        if (record->event.pressed) {
            // Pressed 
            
            // Send keys to hold
            for (uint8_t i = 0; i < num_keys_on_hold; i++) {
                uint16_t key = keys_on_hold[i];
                register_code(key);
            }

            th_record->begin_time = timer_read();
            th_record->is_valid   = true;
        } else {
            // Released: release held keys
            for (uint8_t i = 0; i < num_keys_on_hold; i++) {
                uint16_t key = keys_on_hold[i];
                unregister_code(key); 
            }

            // If time pressed and released is short enough, send key_on_pressed
            uint16_t end = timer_read();
            if ( th_record->is_valid && (end - th_record->begin_time) < tap_time_ms ) {
                tap_code(key_on_pressed);
            }
        }
    } else {
        th_record->is_valid = false;
    }
}

void handle_tap_or_hold(
        uint16_t keycode_in, keyrecord_t *record, 
        uint16_t trigger_key,
        uint16_t key_on_pressed, uint16_t key_on_hold)
{
    uint16_t keys_on_hold[] = { key_on_hold };
    handle_tap_or_hold_multi(keycode_in, record, trigger_key, key_on_pressed, keys_on_hold, 1);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record)
{
    // On KB_CAPSESC: ctrl when held, esc when tapped.
    handle_tap_or_hold(keycode, record, KB_CAPSESC, KC_ESC, KC_LCTL);

    // Tab as hyper. Need to do something different for this one though. 
    // Holding tab should cause a bit of a delay before registering all the modifier keys. Otherwise, this would
    // have the result of cancelling other modifiers already held (such as GUI if you're trying to GUI+Tab)
    //
    // uint16_t HYPER[] = { KC_LCTL, KC_LSFT, KC_LALT, KC_LGUI }; 
    // handle_tap_or_hold_multi(keycode, record, KB_TABHYPR, KC_TAB, HYPER, sizeof(HYPER) / sizeof(HYPER[0]));

    return true;
}
