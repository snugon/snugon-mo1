/*
Copyright 2025 Shota Nishide

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "quantum.h"
#include "snugon_mo1.h"
#include "drivers/pmw3360/pmw3360.h"

/********************type out*********************************/
#include <stdio.h>
#include <string.h>

char cpi_str[10];
char rprt_str[20];
char scrl_div_str[20];
char scrl_inv_str[25];

/***********scroll horizontal threshold**********/
char scrl_thres_str[35];
/***********scroll horizontal threshold**********/

/********************type out*********************************/

const uint8_t CPI_DEFAULT    = snugon_mo1_CPI_DEFAULT / 100;
const uint8_t CPI_MAX        = pmw3360_MAXCPI + 1;
const uint8_t SCROLL_DIV_MAX = 7;

/*********************scroll horizontal threshold***************************/
const uint8_t SCROLL_thres_MAX = 50;
/*********************scroll horizontal threshold***************************/

/********************report rate******************************/
uint16_t report_array[] = snugon_mo1_REPORTMOUSE_INTERVAL;
/********************report rate******************************/

snugon_mo1_t snugon_mo1 = {
    .sensor_ok      = false,
    .sensor_motion = {0},

    .cpi_value   = 0,
    .cpi_changed = false,

    .scroll_mode = false,
    .scroll_div  = 0,

    /*********************scroll horizontal threshold***************************/
    .scroll_thres  = 0,
    /*********************scroll horizontal threshold***************************/

    /********************scroll invert************************/
    .scroll_inv = true,
    /********************scroll invert************************/

    /********************report rate**************************/
    .report_rate  = 3,
    /********************report rate**************************/
};

//////////////////////////////////////////////////////////////////////////////
// Hook points

__attribute__((weak)) void snugon_mo1_on_adjust_layout(snugon_mo1_adjust_t v) {}

//////////////////////////////////////////////////////////////////////////////
// Static utilities

// add16 adds two int16_t with clipping.
static int16_t add16(int16_t a, int16_t b) {
    int16_t r = a + b;
    if (a >= 0 && b >= 0 && r < 0) {
        r = 32767;
    } else if (a < 0 && b < 0 && r >= 0) {
        r = -32768;
    }
    return r;
}

// clip2int8 clips an integer fit into int8_t.
static inline int8_t clip2int8(int16_t v) {
    return (v) < -127 ? -127 : (v) > 127 ? 127 : (int8_t)v;
}

static void add_cpi(int8_t delta) {
    int16_t v = snugon_mo1_get_cpi() + delta;
    snugon_mo1_set_cpi(v < 1 ? 1 : v);
}

static void add_scroll_div(int8_t delta) {
    int8_t v = snugon_mo1_get_scroll_div() + delta;
    snugon_mo1_set_scroll_div(v < 1 ? 1 : v);
}

/*********************scroll horizontal threshold***************************/
static void add_scroll_thres(int8_t delta) {
    int8_t v = snugon_mo1_get_scroll_thres() + delta;
    snugon_mo1_set_scroll_thres(v < 0 ? 0 : v);
}
/*********************scroll horizontal threshold***************************/

/********************report rate******************************/
static void add_report_rate(int8_t delta) {
    int8_t v = snugon_mo1_get_report_rate() + delta;    
    snugon_mo1_set_report_rate(v < 0 ? 0 : v);
}
/********************report rate******************************/

/********************type out*********************************/
static void type_out(void) {    
    // convert to char
    sprintf(cpi_str, "CPI %u", snugon_mo1_get_cpi() * 100);                                                   // CPI
    sprintf(rprt_str, " / report rate %u", 1000 / report_array[snugon_mo1_get_report_rate()]);                // report rate
    sprintf(scrl_div_str, " / scroll divider %u", snugon_mo1_get_scroll_div());                               // scroll divider
    sprintf(scrl_inv_str, " / scroll invert %s", snugon_mo1_get_scroll_inv() ? "Yes" : " No");                // scroll invert

    /*********************scroll horizontal threshold***************************/
    sprintf(scrl_thres_str, " / scroll horizontal threshold %u", snugon_mo1_get_scroll_thres());              // scroll horizontal threshold
    /*********************scroll horizontal threshold***************************/

    // send string
    send_string_with_delay(cpi_str, 10);                                                                     // CPI
    send_string_with_delay(rprt_str, 10);                                                                    // report rate
    send_string_with_delay(scrl_div_str, 10);                                                                // scroll divider
    send_string_with_delay(scrl_inv_str, 10);                                                                // scroll invert

    /***********scroll horizontal threshold**********/
    send_string_with_delay(scrl_thres_str, 10);                                                                // scroll horizontal threshold
    /***********scroll horizontal threshold**********/

    // delay
    send_string(SS_DELAY(2500));                                                                            // delay(ms)
    // backspace
    int total_length = strlen(cpi_str) + strlen(rprt_str) + strlen(scrl_div_str) + strlen(scrl_inv_str) + strlen(scrl_thres_str);    // string length
    for(int i = 0; i < total_length; i++) {
        send_string("\b");                                                                                  // backspace
    }
}
/********************type out*********************************/




//////////////////////////////////////////////////////////////////////////////
// Pointing device driver

void pointing_device_driver_init(void) {
snugon_mo1.sensor_ok = pmw3360_init();
if (snugon_mo1.sensor_ok) {
    pmw3360_cpi_set(CPI_DEFAULT - 1);
    pmw3360_reg_write(pmw3360_Motion_Burst, 0);
}
}

uint16_t pointing_device_driver_get_cpi(void) {
    return snugon_mo1_get_cpi();
}

void pointing_device_driver_set_cpi(uint16_t cpi) {
    snugon_mo1_set_cpi(cpi);
}

// Modify here when changing the cursor movement along the X and Y axes. Example: r->x = -clip2int8(m->y)
static void motion_to_mouse_move(snugon_mo1_motion_t *m, report_mouse_t *r) {
    /*****************no move mode*************************/
    if (!no_move_mode) {
    /*****************no move mode*************************/
        r->x = clip2int8(m->x);
        r->y = clip2int8(m->y);
        // clear motion
        m->x = 0;
        m->y = 0;
    /*****************no move mode*************************/
    } else {   
        m->x = 0;
        m->y = 0;
    }
    /*****************no move mode*************************/
}

static void motion_to_mouse_scroll(snugon_mo1_motion_t *m, report_mouse_t *r) {    
    uint8_t div = snugon_mo1_get_scroll_div() - 1;
    int16_t x   = m->x >> div;
    m->x -= x << div;
    int16_t y = m->y >> div;
    m->y -= y << div;

// Modify here when changing the movement of the scroll (pan function) along the X and Y axes. Example: r->h = -clip2int8(y)
/*****************scroll invert****************************/
if (snugon_mo1.scroll_inv) {
    r->h = clip2int8(x);
    r->v = -clip2int8(y);
} else {
    r->h = -clip2int8(x);
    r->v = clip2int8(y);
}
/*****************scroll invert****************************/

#if snugon_mo1_SCROLLSNAP_ENABLE
    // scroll snap.
    uint32_t now = timer_read32();
    if (r->h != 0 || r->v != 0) {
        snugon_mo1.scroll_snap_last = now;
    } else if (TIMER_DIFF_32(now, snugon_mo1.scroll_snap_last) >= snugon_mo1_SCROLLSNAP_RESET_TIMER) {
        snugon_mo1.scroll_snap_tension_h = 0;
    }
    /*********************scroll horizontal threshold***************************************/
    // if (abs(snugon_mo1.scroll_snap_tension_h) < snugon_mo1_SCROLLSNAP_TENSION_THRESHOLD) {
    if (abs(snugon_mo1.scroll_snap_tension_h) < snugon_mo1.scroll_thres) {
    /*********************scroll horizontal threshold***************************************/
        snugon_mo1.scroll_snap_tension_h += y;
        r->h = 0;
    }
#endif
}

static void motion_to_mouse(snugon_mo1_motion_t *m, report_mouse_t *r, bool as_scroll) {
    if (as_scroll) {
        motion_to_mouse_scroll(m, r);
    } else {        
        motion_to_mouse_move(m, r);
    }
}

// report rate
static inline bool should_report(void) {
    uint32_t now = timer_read32();
if (report_array[snugon_mo1.report_rate] > 0) {    
    static uint32_t last = 0;    
    if (TIMER_DIFF_32(now, last) < report_array[snugon_mo1.report_rate]) {    
        return false;
    }
    last = now;
}

#if defined(snugon_mo1_SCROLL_INHIVITOR) && snugon_mo1_SCROLL_INHIVITOR > 0
    if (TIMER_DIFF_32(now, snugon_mo1.scroll_mode_changed) < snugon_mo1_SCROLL_INHIVITOR) {
        snugon_mo1.sensor_motion.x = 0;
        snugon_mo1.sensor_motion.y = 0;
    }
#endif
    return true;
}

report_mouse_t pointing_device_driver_get_report(report_mouse_t rep) {
    // fetch from optical sensor.
    if (snugon_mo1.sensor_ok) {
        pmw3360_motion_t d = {0};
        if (pmw3360_motion_burst(&d)) { 
            ATOMIC_BLOCK_FORCEON {
                snugon_mo1.sensor_motion.x = add16(snugon_mo1.sensor_motion.x, d.x);
                snugon_mo1.sensor_motion.y = add16(snugon_mo1.sensor_motion.y, d.y);
            }
        }
    }
    // report mouse event, if keyboard is primary.
    if (is_keyboard_master() && should_report()) {
        // modify mouse report by PMW3360 motion.        
        motion_to_mouse(&snugon_mo1.sensor_motion, &rep, snugon_mo1.scroll_mode);        
    }
    return rep;
}

bool snugon_mo1_get_scroll_mode(void) {
    return snugon_mo1.scroll_mode;
}

void snugon_mo1_set_scroll_mode(bool mode) {
    if (mode != snugon_mo1.scroll_mode) {
        snugon_mo1.scroll_mode_changed = timer_read32();
    }
    snugon_mo1.scroll_mode = mode;
}

/*****************scroll invert*********************************************/
bool snugon_mo1_get_scroll_inv(void) {
    return snugon_mo1.scroll_inv;
}

void snugon_mo1_set_scroll_inv(bool mode) {
    if (mode != snugon_mo1.scroll_inv) {
        snugon_mo1.scroll_inv_changed = timer_read32();
    }
    snugon_mo1.scroll_inv = mode;
}
/*****************scroll invert*********************************************/

/*****************no move mode**********************************************/
void no_move_mode_control(bool mode) {
    no_move_mode = mode;
}
/*****************no move mode**********************************************/

uint8_t snugon_mo1_get_scroll_div(void) {
    return snugon_mo1.scroll_div == 0 ? snugon_mo1_SCROLL_DIV_DEFAULT : snugon_mo1.scroll_div;
}

void snugon_mo1_set_scroll_div(uint8_t div) {
    snugon_mo1.scroll_div = div > SCROLL_DIV_MAX ? SCROLL_DIV_MAX : div;
}

/*********************scroll horizontal threshold***************************/
uint8_t snugon_mo1_get_scroll_thres(void) {
    return snugon_mo1.scroll_thres == 0 ? snugon_mo1_SCROLLSNAP_TENSION_THRESHOLD : snugon_mo1.scroll_thres;
}

void snugon_mo1_set_scroll_thres(uint8_t thres) {
    snugon_mo1.scroll_thres = thres > SCROLL_thres_MAX ? SCROLL_thres_MAX : thres;
}
/*********************scroll horizontal threshold***************************/

/***********report rate*****************************************************/
uint8_t snugon_mo1_get_report_rate(void) {    
    return snugon_mo1.report_rate <= 0 ? 0 : snugon_mo1.report_rate;
}

void snugon_mo1_set_report_rate(uint8_t rate) {    
    snugon_mo1.report_rate = rate > (REPORTMOUSE_SIZE - 1) ? (REPORTMOUSE_SIZE - 1) : rate;    
}
/***********report rate*****************************************************/

uint8_t snugon_mo1_get_cpi(void) {
    return snugon_mo1.cpi_value == 0 ? CPI_DEFAULT : snugon_mo1.cpi_value;
}

void snugon_mo1_set_cpi(uint8_t cpi) {
    if (cpi > CPI_MAX) {
        cpi = CPI_MAX;
    }
    snugon_mo1.cpi_value   = cpi;
    snugon_mo1.cpi_changed = true;
    if (snugon_mo1.sensor_ok) {
        pmw3360_cpi_set(cpi == 0 ? CPI_DEFAULT - 1 : cpi - 1);
        pmw3360_reg_write(pmw3360_Motion_Burst, 0);
    }
}

//////////////////////////////////////////////////////////////////////////////
// Keyboard hooks

void keyboard_post_init_kb(void) {
// read snugon_mo1 configuration from EEPROM
if (eeconfig_is_enabled()) {
    snugon_mo1_config_t c = {.raw = eeconfig_read_kb()};
    snugon_mo1_set_cpi(c.cpi);
    snugon_mo1_set_scroll_div(c.sdiv);

    /*********************scroll horizontal threshold***************************/
    snugon_mo1_set_scroll_thres(c.sthres);
    /*********************scroll horizontal threshold***************************/

    /*********************scroll invert*************************/
    snugon_mo1_set_scroll_inv(c.inv);
    /*********************scroll invert*************************/

    /********************report rate****************************/    
    snugon_mo1_set_report_rate(c.rptrt);
    /********************report rate****************************/
}

snugon_mo1_on_adjust_layout(snugon_mo1_ADJUST_PENDING);
keyboard_post_init_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user(keycode, record)) {
        return false;
    }

    // strip QK_MODS part.
    if (keycode >= QK_MODS && keycode <= QK_MODS_MAX) {
        keycode &= 0xff;
    }

    switch (keycode) {
#ifndef MOUSEKEY_ENABLE
        // process KC_MS_BTN1~8 by myself
        // See process_action() in quantum/action.c for details.
        case KC_MS_BTN1 ... KC_MS_BTN8: {
            extern void register_button(bool, enum mouse_buttons);
            register_button(record->event.pressed, MOUSE_BTN_MASK(keycode - KC_MS_BTN1));
            // to apply QK_MODS actions, allow to process others.
            return true;
        }
#endif

        case SCRL_MO:
            snugon_mo1_set_scroll_mode(record->event.pressed);
            return false;

        /*****************no move mode**************************************/
        case NO_MOVE:
            no_move_mode_control(record->event.pressed);
            return false;
        /*****************no move mode**************************************/
    }           

    // process events which works on pressed only.
    if (record->event.pressed) {
        switch (keycode) {
            case KBC_RST:
                snugon_mo1_set_cpi(snugon_mo1_CPI_DEFAULT / 100);
                snugon_mo1_set_scroll_div(snugon_mo1_SCROLL_DIV_DEFAULT);

                /*********************scroll horizontal threshold***************************/
                snugon_mo1_set_scroll_thres(snugon_mo1_SCROLLSNAP_TENSION_THRESHOLD);
                /*********************scroll horizontal threshold***************************/

                /*********************scroll invert*************************/
                snugon_mo1_set_scroll_inv(false);
                /*********************scroll invert*************************/

                /********************report rate****************************/
                snugon_mo1_set_report_rate(0);                                     // 1000cpi
                /********************report rate****************************/

                break;
            case KBC_SAVE: {
                snugon_mo1_config_t c = {
                    .cpi  = snugon_mo1.cpi_value,
                    .sdiv = snugon_mo1.scroll_div,

                    /*********************scroll horizontal threshold***************************/
                    .sthres = snugon_mo1.scroll_thres,
                    /*********************scroll horizontal threshold***************************/

                    /*********************scroll invert**********************/
                    .inv = snugon_mo1.scroll_inv,
                    /*********************scroll invert**********************/
                    
                    /********************report rate*************************/
                    .rptrt = snugon_mo1.report_rate,                    
                    /********************report rate*************************/
                };
                eeconfig_update_kb(c.raw);
            } break;

            case CPI_I100:
                add_cpi(1);
                break;
            case CPI_D100:
                add_cpi(-1);
                break;
            case CPI_I1K:
                add_cpi(10);
                break;
            case CPI_D1K:
                add_cpi(-10);
                break;

            case SCRL_TO:
                snugon_mo1_set_scroll_mode(!snugon_mo1.scroll_mode);
                break;
            case SCRL_DVI:
                add_scroll_div(1);
                break;
            case SCRL_DVD:
                add_scroll_div(-1);
                break;
            /*********************scroll invert***************************/
            case SCRL_INV:
                snugon_mo1_set_scroll_inv(!snugon_mo1.scroll_inv);
                break;
            /*********************scroll invert***************************/

            /********************report rate******************************/
            case RPRT_RTI:
                add_report_rate(-1);
                break;
            case RPRT_RTD:
                add_report_rate(1);
                break;
            /********************report rate******************************/

            /********************type out*********************************/
            case TYPE_OUT:
                type_out();
                break;
            /********************type out*********************************/

            /*********************scroll invert***************************/
            case BOOT_LDR:
                reset_keyboard();
                break;
            /*********************scroll invert***************************/

            /*********************scroll horizontal threshold***************************/
            case SCRL_THI:
                add_scroll_thres(1);
                break;
            case SCRL_THD:
                add_scroll_thres(-1);
                break;
            /*********************scroll horizontal threshold***************************/

            default:
                return true;
        }
        return false;
    }

    return true;
}
