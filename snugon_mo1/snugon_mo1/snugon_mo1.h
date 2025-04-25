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

#include "quantum.h"
#include "lib/snugon_mo1/snugon_mo1.h"

// clang-format off

#define LAYOUT_basic( \
    M00, M01, M02, M03, M04, M05, M06, M07, M08, M09, M10, M11 \
    ) \
    { \
        { M00,   M01,   M02,   M03,   M04,   M05,   M06,   M07,   M08,   M09,   M10,   M11 } \
    }

// clang-format on

#define LAYOUT LAYOUT_basic