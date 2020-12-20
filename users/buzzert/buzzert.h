#ifndef USERSPACE
#define USERSPACE

#include "quantum.h"

enum my_keycodes {
    KB_CAPSESC = SAFE_RANGE,

    USER_SAFE_RANGE
};

void my_custom_function(void);

#endif
