/*
 * DS18B20.h
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

#ifndef __DS18B20__H__
#define __DS18B20__H__
#include <avr/io.h>
#include "OneWire.h"

#define DS18B20_PORTIN ONEWIRE_PORTIN
#define DS18B20_PORT ONEWIRE_PORT
#define DS18B20_IO_CTL ONEWIRE_IO_CTL
#define DS18B20_PIN ONEWIRE_PIN

#define SPAD_TEMP_LSB   0
#define SPAD_TEMP_MSB   1
#define SPAD_ALARM_HIGH 2
#define SPAD_ALARM_LOW  3
#define SPAD_CONFIG     4
#define SPAD_RESERVED1  5
#define SPAD_RESERVED2  6
#define SPAD_RESERVED3  7
#define SPAD_CRC        8
#define SPAD_MAX_BYTES  9

#define CMD_18B20_CONV_TEMP 0x44
#define CMD_18B20_WRITE_SPAD 0x4E
#define CMD_18B20_READ_SPAD 0xBE
#define CMD_18B20_COPY_SPAD 0x48
#define CMD_18B20_RECALL_E 0xB8
#define CMD_18B20_READ_POWER_SUPPLY 0xB4

#define DS18B20_FAMILY_CODE 0x28

#define DS18B20_9BIT_RESOLUTION  0x0
#define DS18B20_10BIT_RESOLUTION 0x1
#define DS18B20_11BIT_RESOLUTION 0x2
#define DS18B20_12BIT_RESOLUTION 0x3


uint8_t ds18b20_check_spad_crc(uint8_t* spad);
uint8_t ds18b20_read_spad(uint8_t* spad);
uint8_t ds18b20_write_spad(uint8_t* spad);
uint8_t ds18b20_read_temperature(int16_t* temperature);
uint8_t ds18b20_set_resolution(const uint8_t resolution, const uint8_t store_to_eeprom);

#endif // __DS18B20__H__
