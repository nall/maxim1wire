/*
 * OneWire.h
 * maxim1wire
 *
 * Library for Maxim 1-wire based devices.
 *
 * Copyright (c) 2009, Jon Nall, All rights reserved.
 *
 * This file is part of maxim1wire.
 *
 * maxim1wire is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * maxim1wire is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with maxim1wire.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ONEWIRE__H__
#define __ONEWIRE__H__
#include <inttypes.h>
#include <avr/io.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// Redefine these as required for your AVR
#ifndef ONEWIRE_PORTIN
#error Must define ONEWIRE_PORTIN in Makefile
#endif
#ifndef ONEWIRE_PORT
#error Must define ONEWIRE_PORT in Makefile
#endif
#ifndef ONEWIRE_IO_CTL
#error Must define ONEWIRE_IO_CTL in Makefile
#endif
#ifndef ONEWIRE_PIN
#error Must define ONEWIRE_PIN in Makefile
#endif

#define CMD_ONEWIRE_SEARCH_ROM 0xF0
#define CMD_ONEWIRE_READ_ROM 0x33
#define CMD_ONEWIRE_MATCH_ROM 0x55
#define CMD_ONEWIRE_SKIP_ROM 0xCC
#define CMD_ONEWIRE_ALARM_SEARCH 0xEC

uint8_t inline onewire_select_device_and_issue_command(const uint8_t cmd, const uint8_t family_code);

// Check the specified rom code, which is a uint8_t array of size 8
// this includes:
// 1 byte family code
// 12 byte serial number
// 1 byte CRC
// Returns EXIT_SUCCESS or EXIT_FAILURE
uint8_t onewire_check_rom_crc(const uint8_t* romcode, const uint8_t family_code);

// Initialize the 1-wire lib
void onewire_init();

// Reset the 1-wire bus, returning 1 if a device is present or 0 if not
uint8_t onewire_reset();

// Read a single byte from the currently addressed device
uint8_t onewire_readbyte();

// Write a single byte to the currently addressed device
void onewire_writebyte(const uint8_t data);

#endif // __ONEWIRE__H__
