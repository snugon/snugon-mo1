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
#include "pmw3360.h"
/***************************srom cpi 12000************************************************/
#include "pmw3360_srom_0x04.h"
/***************************srom cpi 12000************************************************/

#define PMW3360_SPI_MODE 3
#define PMW3360_SPI_DIVISOR 64
#define PMW3360_CLOCKS 2000000

bool pmw3360_spi_start(void) {
    return spi_start(PMW3360_NCS_PIN, false, PMW3360_SPI_MODE, PMW3360_SPI_DIVISOR);
}

uint8_t pmw3360_reg_read(uint8_t addr) {
    pmw3360_spi_start();
    spi_write(addr & 0x7f);
    wait_us(160);
    uint8_t data = spi_read();
    spi_stop();
    wait_us(20);
    return data;
}

void pmw3360_reg_write(uint8_t addr, uint8_t data) {
    pmw3360_spi_start();
    spi_write(addr | 0x80);
    spi_write(data);
    spi_stop();
    wait_us(180);
}

uint8_t pmw3360_cpi_get(void) {
    return pmw3360_reg_read(pmw3360_Config1);
}

void pmw3360_cpi_set(uint8_t cpi) {
    if (cpi > pmw3360_MAXCPI) {
        cpi = pmw3360_MAXCPI;
    }
    pmw3360_reg_write(pmw3360_Config1, cpi);
}

static uint32_t pmw3360_timer      = 0;
static uint32_t pmw3360_scan_count = 0;
static uint32_t pmw3360_last_count = 0;

void pmw3360_scan_perf_task(void) {
    pmw3360_scan_count++;
    uint32_t now = timer_read32();
    if (TIMER_DIFF_32(now, pmw3360_timer) > 1000) { 
        #if defined(CONSOLE_ENABLE)
            dprintf("pmw3360 scan frequency: %lu\n", pmw3360_scan_count);
        #endif
        pmw3360_last_count = pmw3360_scan_count;
        pmw3360_scan_count = 0;
        pmw3360_timer      = now;
    }
}

uint32_t pmw3360_scan_rate_get(void) {
    return pmw3360_last_count;
}

bool pmw3360_motion_read(pmw3360_motion_t *d) {
#ifdef DEBUG_PMW3360_SCAN_RATE
    pmw3360_scan_perf_task();
#endif
    uint8_t mot = pmw3360_reg_read(pmw3360_Motion);
    if ((mot & 0x88) != 0x80) {
        return false;
    }
    d->x = pmw3360_reg_read(pmw3360_Delta_X_L);
    d->x |= pmw3360_reg_read(pmw3360_Delta_X_H) << 8;
    d->y = pmw3360_reg_read(pmw3360_Delta_Y_L);
    d->y |= pmw3360_reg_read(pmw3360_Delta_Y_H) << 8;
    return true;
}

bool pmw3360_motion_burst(pmw3360_motion_t *d) {
#ifdef DEBUG_PMW3360_SCAN_RATE
    pmw3360_scan_perf_task();
#endif
    pmw3360_spi_start();
    spi_write(pmw3360_Motion_Burst);
    wait_us(35);
    uint8_t mot = spi_read();
    if ((mot & 0x88) != 0x80) {
        spi_stop();
        return false;
    }
    spi_read();
    d->x = spi_read();
    d->x |= spi_read() << 8;
    d->y = spi_read();
    d->y |= spi_read() << 8;
    spi_stop();
    return true;
}

/***************************srom cpi 12000************************************************/
extern const uint16_t pmw_firmware_length;
extern const uint8_t  pmw_firmware_data[];

void pmw_spi_start(void) { spi_start(PMW3360_NCS_PIN, false, PMW3360_SPI_MODE, PMW3360_SPI_DIVISOR); }

void pmw_write(uint8_t reg_addr, uint8_t data) {
    pmw_spi_start();
    spi_write(reg_addr | 0x80);
    spi_write(data);
    spi_stop();
    wait_us(180);
}

uint8_t pmw_read(uint8_t reg_addr) {
    pmw_spi_start();
    spi_write(reg_addr & 0x7f);
    uint8_t data = spi_read();
    spi_stop();
    wait_us(20);

    return data;
}
/***************************srom cpi 12000************************************************/

bool pmw3360_init(void) {
    spi_init();
    setPinOutput(PMW3360_NCS_PIN);
    // reboot
    pmw3360_spi_start();
    pmw3360_reg_write(pmw3360_Power_Up_Reset, 0x5a);
    wait_ms(50);
    // read five registers of motion and discard those values
    pmw3360_reg_read(pmw3360_Motion);
    pmw3360_reg_read(pmw3360_Delta_X_L);
    pmw3360_reg_read(pmw3360_Delta_X_H);
    pmw3360_reg_read(pmw3360_Delta_Y_L);
    pmw3360_reg_read(pmw3360_Delta_Y_H);    
    /***************************srom cpi 12000************************************************/
    // upload firmware
    // disable rest mode
    pmw_write(pmw3360_Config2, 0x20);                                       // SROM step 2

    // enable initialisation
    pmw_write(pmw3360_SROM_Enable, 0x1d);                                   // SROM step 3

    // wait a frame
    wait_ms(10);                                                    // SROM step 4

    // start SROM download
    pmw_write(pmw3360_SROM_Enable, 0x18);                           // SROM step 5

    // write the SROM file

    pmw_spi_start();

    spi_write(pmw3360_SROM_Load_Burst | 0x80);                      // SROM step 6
    wait_us(15);

    // send all bytes of the firmware
    unsigned char c;
    for (int i = 0; i < pmw_firmware_length; i++) {
        c = (unsigned char)pgm_read_byte(pmw_firmware_data + i);
        spi_write(c);
        wait_us(15);
    }

    spi_stop();
    wait_us(180);

    // read id
    pmw_read(pmw3360_SROM_ID);                                      // SROM step 7
    /***************************srom cpi 12000************************************************/
    // configuration
    pmw3360_reg_write(pmw3360_Config2, 0x00);                       // SROM step 8
    // check product ID and revision ID
    uint8_t pid = pmw3360_reg_read(pmw3360_Product_ID);
    uint8_t rev = pmw3360_reg_read(pmw3360_Revision_ID);
    spi_stop();
    return pid == 0x42 && rev == 0x01;
}
