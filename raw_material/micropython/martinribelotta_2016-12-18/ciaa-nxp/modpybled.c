/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Ernesto Gigliotti <ernestogigliotti@gmail.com>
 * Copyright (c) 2015 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/runtime.h"
#include "modpyb.h"
#include "ciaanxp_mphal.h"

typedef struct _pyb_led_obj_t {
    mp_obj_base_t base;
} pyb_led_obj_t;

STATIC const pyb_led_obj_t pyb_led_obj[] = {
    {{&pyb_led_type}},
    {{&pyb_led_type}},
    {{&pyb_led_type}},
    {{&pyb_led_type}},
    {{&pyb_led_type}},
    {{&pyb_led_type}},
};

#define NUM_LED MP_ARRAY_SIZE(pyb_led_obj)
#define LED_ID(obj) ((obj) - &pyb_led_obj[0] + 1)

void pyb_led_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_led_obj_t *self = self_in;
    mp_printf(print, "LED(%u)", LED_ID(self));
}

STATIC mp_obj_t pyb_led_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t led_id = mp_obj_get_int(args[0]);
    if (!(1 <= led_id && led_id <= NUM_LED)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "LED %d does not exist", led_id));
    }
    return (mp_obj_t)&pyb_led_obj[led_id - 1];
}

mp_obj_t pyb_led_on(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    mp_hal_setLed(LED_ID(self)-1, 0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_led_on_obj, pyb_led_on);

mp_obj_t pyb_led_off(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    mp_hal_setLed(LED_ID(self)-1, 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_led_off_obj, pyb_led_off);

mp_obj_t pyb_led_toggle(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    mp_hal_toggleLed(LED_ID(self)-1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_led_toggle_obj, pyb_led_toggle);

mp_obj_t pyb_led_value(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    uint8_t val = mp_hal_testLed(LED_ID(self)-1);
    if(val==0)
        val=1;
    else
        val=0;
    return MP_OBJ_NEW_SMALL_INT(val);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_led_value_obj, pyb_led_value);

/// \method intensity([value])
/// Get or set the led intensity:
///
///   - With no argument, return led intensity (0 to 15).
///   - With `value` given, set the led intensity (0 to 15)
STATIC mp_obj_t pyb_led_intensity(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_led_obj_t *self = args[0];

    int ln = LED_ID(self);
    if(ln!=4 && ln!=5 && ln!=6)
    {
	nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "LED %d does not support intensity", ln));
	return mp_const_none;
    }

    if (n_args == 1) {
        // get intensity
        return MP_OBJ_NEW_SMALL_INT(mp_hal_getPwmRGBValue(LED_ID(self)-4));
    } else {
        // set intensity
	mp_hal_setPwmRGBValue(LED_ID(self)-4,mp_obj_get_int(args[1]));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_led_intensity_obj, 1, 2, pyb_led_intensity);


STATIC const mp_map_elem_t pyb_led_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_on), (mp_obj_t)&pyb_led_on_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_off), (mp_obj_t)&pyb_led_off_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_toggle), (mp_obj_t)&pyb_led_toggle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_intensity), (mp_obj_t)&pyb_led_intensity_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_value), (mp_obj_t)&pyb_led_value_obj },

};

STATIC MP_DEFINE_CONST_DICT(pyb_led_locals_dict, pyb_led_locals_dict_table);

const mp_obj_type_t pyb_led_type = {
    { &mp_type_type },
    .name = MP_QSTR_LED,
    .print = pyb_led_print,
    .make_new = pyb_led_make_new,
    .locals_dict = (mp_obj_t)&pyb_led_locals_dict,
};
