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

#include QMK_KEYBOARD_H

#include "lib/snugon_mo1/snugon_mo1.h"

//////////////////////////////////////////////////////////////////////////////

// clang-format off
matrix_row_t matrix_mask[MATRIX_ROWS] = {
    // lows:1 lols:12
    0b111111111111,
};
// clang-format on
