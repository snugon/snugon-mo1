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

#pragma once

//////////////////////////////////////////////////////////////////////////////
// Configurations

#ifndef snugon_mo1_CPI_DEFAULT
#    define snugon_mo1_CPI_DEFAULT 3000
#endif

// 4: 1/8 (1/2^(n-1))
#ifndef snugon_mo1_SCROLL_DIV_DEFAULT
#    define snugon_mo1_SCROLL_DIV_DEFAULT 7
#endif

// mouse report rate:125(8)、250(4)、500(2)、1000(1)　report rate
#ifndef snugon_mo1_REPORTMOUSE_INTERVAL
#    define snugon_mo1_REPORTMOUSE_INTERVAL { 1, 2, 4, 8 }
#endif

#ifndef snugon_mo1_SCROLL_INHIVITOR
#    define snugon_mo1_SCROLL_INHIVITOR 50
#endif

#ifndef snugon_mo1_SCROLLSNAP_ENABLE
#    define snugon_mo1_SCROLLSNAP_ENABLE 1
#endif

#ifndef snugon_mo1_SCROLLSNAP_RESET_TIMER
#    define snugon_mo1_SCROLLSNAP_RESET_TIMER 100
#endif

/*********************scroll horizontal threshold***************************/
// Threshold to introduce a delay at the start of horizontal scrolling when in mouse scroll mode.
#ifndef snugon_mo1_SCROLLSNAP_TENSION_THRESHOLD
// #    define snugon_mo1_SCROLLSNAP_TENSION_THRESHOLD 12
#    define snugon_mo1_SCROLLSNAP_TENSION_THRESHOLD 0
#endif
/*********************scroll horizontal threshold***************************/

// report rate
extern uint16_t report_array[];
#define REPORTMOUSE_SIZE (sizeof(report_array) / sizeof(uint16_t))

//////////////////////////////////////////////////////////////////////////////
// Constants

#define snugon_mo1_TX_GETINFO_INTERVAL 500
#define snugon_mo1_TX_GETINFO_MAXTRY 10
#define snugon_mo1_TX_GETMOTION_INTERVAL 4

//////////////////////////////////////////////////////////////////////////////
// Types

// user keys
enum snugon_mo1_keycodes {
    
    // reset & save
    KBC_SAVE = QK_KB_0, // snugon_mo1 configuration: save to EEPROM
    
    // type out
    TYPE_OUT = QK_KB_1,

    // cpi
    CPI_I100 = QK_KB_2, // CPI +100 CPI
    CPI_D100 = QK_KB_3, // CPI -100 CPI
    CPI_I1K = QK_KB_4, // CPI +1000 CPI
    CPI_D1K = QK_KB_5, // CPI -1000 CPI

    // wheel
    SCRL_TO = QK_KB_6, // Toggle scroll mode
    SCRL_MO = QK_KB_7, // Momentary scroll mode
    SCRL_DVD = QK_KB_8, // Decrement scroll divider (faster)
    SCRL_DVI = QK_KB_9, // Increment scroll divider (slower)
    
    /*********************scroll horizontal threshold***************************/
    // Threshold to introduce a delay at the start of horizontal scrolling
    SCRL_THI = QK_KB_10,
    SCRL_THD = QK_KB_11,
    /*********************scroll horizontal threshold***************************/

    // scroll invert
    SCRL_INV = QK_KB_12,

    // no move mode
    NO_MOVE = QK_KB_13, // disable cursor movement

    // report rate
    RPRT_RTI = QK_KB_14, // Increase report rate
    RPRT_RTD = QK_KB_15, // Decrease report rate 

    KBC_RST = QK_KB_16, // snugon_mo1 configuration: reset to default

    // reset to the bootloader
    BOOT_LDR = QK_KB_17,   

    snugon_mo1_SAFE_RANGE = QK_USER_0
};


typedef union {
    uint32_t raw;
    struct {
        uint8_t cpi : 7;    // cpi 7bit
        uint8_t sdiv : 3;   // scroll divider 3bit

        /*********************scroll horizontal threshold***************************/
        uint8_t sthres : 7;   // scroll divider 7bit
        /*********************scroll horizontal threshold***************************/

        /*********************scroll invert*************************/
        bool inv : 1;   // scroll invert flag 1bit
        /*********************scroll invert*************************/

        /********************report rate****************************/        
        uint8_t rptrt : 3;   // report rate 3bit
        /********************report rate****************************/
    };
} snugon_mo1_config_t;

typedef struct {
    int16_t x;
    int16_t y;
} snugon_mo1_motion_t;

typedef uint8_t snugon_mo1_cpi_t;

typedef struct {
    bool sensor_ok;

    snugon_mo1_motion_t sensor_motion;

    uint8_t cpi_value;
    bool    cpi_changed;

    bool     scroll_mode;
    uint32_t scroll_mode_changed;
    uint8_t  scroll_div;

    /*********************scroll horizontal threshold***************************/
    uint8_t  scroll_thres;
    /*********************scroll horizontal threshold***************************/
    
    /*****************scroll invert*********************************/
    bool     scroll_inv;
    uint32_t scroll_inv_changed;
    /*****************scroll invert*********************************/

    /********************report rate********************************/
    uint8_t  report_rate;
    /********************report rate********************************/

    uint32_t scroll_snap_last;
    int8_t   scroll_snap_tension_h;

    uint16_t       last_kc;
    keypos_t       last_pos;
    report_mouse_t last_mouse;
} snugon_mo1_t;

typedef enum {
    snugon_mo1_ADJUST_PENDING   = 0,
    snugon_mo1_ADJUST_PRIMARY   = 1,
    snugon_mo1_ADJUST_SECONDARY = 2,
} snugon_mo1_adjust_t;

//////////////////////////////////////////////////////////////////////////////
// Exported values (touch carefully)

extern snugon_mo1_t snugon_mo1;

//////////////////////////////////////////////////////////////////////////////
// Public API functions

/// snugon_mo1_get_scroll_mode gets current scroll mode.
bool snugon_mo1_get_scroll_mode(void);

/// snugon_mo1_set_scroll_mode modify scroll mode.
void snugon_mo1_set_scroll_mode(bool mode);

/*****************scroll invert*********************************/
bool snugon_mo1_get_scroll_inv(void);
void snugon_mo1_set_scroll_inv(bool mode);
/*****************scroll invert*********************************/

/*****************no move mode**********************************/
void no_move_mode_control(bool mode);
bool no_move_mode;
/*****************no move mode**********************************/

uint8_t snugon_mo1_get_scroll_div(void);

void snugon_mo1_set_scroll_div(uint8_t div);

/*********************scroll horizontal threshold***************************/
uint8_t snugon_mo1_get_scroll_thres(void);
void snugon_mo1_set_scroll_thres(uint8_t thres);
/*********************scroll horizontal threshold***************************/

/********************report rate********************************/
uint8_t snugon_mo1_get_report_rate(void);
void snugon_mo1_set_report_rate(uint8_t rate);
/********************report rate********************************/

uint8_t snugon_mo1_get_cpi(void);

void snugon_mo1_set_cpi(uint8_t cpi);
