#pragma once

#define MATRIX_ROWS         1
#define MATRIX_COLS         12
#define MATRIX_ROW_PINS     { }
#define MATRIX_COL_PINS     { GP17, GP7, GP6, GP5, GP4, GP3, GP2, GP29, GP28, GP27, GP26, GP25 }
#define MATRIX_MASKED
#define DEBOUNCE            5
#define DIODE_DIRECTION     COL2ROW

// Layers
    #define DYNAMIC_KEYMAP_LAYER_COUNT 32

    #if !defined(LAYER_STATE_8BIT) && !defined(LAYER_STATE_16BIT) && !defined(LAYER_STATE_32BIT)
    #    define LAYER_STATE_32BIT
    #endif

// To squeeze firmware size
    #undef LOCKING_SUPPORT_ENABLE
    #undef LOCKING_RESYNC_ENABLE

//PMW3360
    #define SPI_SCK_PIN GP18
    #define SPI_MOSI_PIN GP19
    #define SPI_MISO_PIN GP20
    #define PMW3360_NCS_PIN GP21 // SPI CS pin.