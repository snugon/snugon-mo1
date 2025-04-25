# MCU name
MCU = RP2040

# Bootloader selection
BOOTLOADER = rp2040

# Link Time Optimization required for size.
LTO_ENABLE = yes

# Build Options
BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
NKRO_ENABLE = yes           # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
AUDIO_ENABLE = no           # Audio output

SPLIT_KEYBOARD = no

ENCODER_ENABLE = yes


RGBLIGHT_ENABLE = no

RGB_MATRIX_ENABLE = no

SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend

OLED_ENABLE = no



# Disable other features to squeeze firmware size
SPACE_CADET_ENABLE = no
MAGIC_ENABLE = no

# type out cpi, report rate, scroll divider
SEND_STRING_ENABLE = yes

# Include common library
SRC += lib/snugon_mo1/snugon_mo1.c

# PMW3360
MOUSEKEY_ENABLE = yes

POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = custom
SRC += drivers/pmw3360/pmw3360.c
QUANTUM_LIB_SRC += spi_master.c # Optical sensor use SPI to communicate