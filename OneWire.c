/*
 * OneWire.c
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

#include "OneWire.h"
#include <util/crc16.h>
#include <util/delay.h>

#define MIN_RESET_PULSE 500         // 480 usecs (minimum)
#define MIN_PRESENCE_RESP_DELAY 15  // 15-60 usecs
#define MIN_PRESENCE_PULSE 100      // 60-240 usecs

#define MIN_READ_DRIVE_DELAY 1     // 1 usec
#define MAX_READ_RESP_DELAY 5     // toward the end of the 15us window

#define MIN_WRITE1_DRIVE_DELAY 5     // 15 usecs
#define MIN_RWSLOT_MIN_SLOT_LENGTH 75 // 60 usecs
#define MIN_RWSLOT_RECOVERY_DELAY 5   // 1 usec

static void bus_drive0()
{
    // Set the pin to an output
    ONEWIRE_IO_CTL |= 1 << ONEWIRE_PIN;
}

static void bus_release()
{
    // Tri-state the pin by setting {DDRn, PORTn} = 2'b00
    // Don't explicitly clear PORTn since we do that in onewire_init()
    ONEWIRE_IO_CTL &= ~(1 << ONEWIRE_PIN);
}

static uint8_t bus_latch()
{
    // Read the current pin state 
    return ((ONEWIRE_PORTIN >> ONEWIRE_PIN) & 1);
}

static void write_slot(const uint8_t value)
{
    bus_drive0();

    if(value == 0)
    {
        _delay_us(MIN_RWSLOT_MIN_SLOT_LENGTH);
    }
    else
    {
        _delay_us(MIN_WRITE1_DRIVE_DELAY);
    }


    bus_release();

    // Extraneous for write1 case, but harmless
    _delay_us(MIN_RWSLOT_MIN_SLOT_LENGTH + MIN_RWSLOT_RECOVERY_DELAY);
}

static uint8_t read_slot()
{
    bus_drive0();
    _delay_us(MIN_READ_DRIVE_DELAY);
    bus_release();
    _delay_us(MAX_READ_RESP_DELAY);

    const uint8_t value = bus_latch();

    // This delay is longer than required, but that's fine
    _delay_us(MIN_RWSLOT_MIN_SLOT_LENGTH + MIN_RWSLOT_RECOVERY_DELAY);

    return value;
}

uint8_t onewire_reset()
{
    // Make the pin an output and drive low for MIN_RESET_PULSE
    bus_drive0();
    _delay_us(MIN_RESET_PULSE);

    bus_release();
    // Stop driving the pin and let the DS180B20 do its thing
    _delay_us(MIN_PRESENCE_RESP_DELAY + MIN_PRESENCE_PULSE);

    const uint8_t presence_value = bus_latch();

    // Presence lasts minimum 480us -- this is overkill, that fine.
    _delay_us(MIN_RESET_PULSE);
    return (presence_value == 0) ? 1 : 0;
}

uint8_t onewire_readbyte()
{
    uint8_t value = 0;
    for(uint8_t bit = 0; bit < 8; ++bit)
    {
        // Values come in LSB first
        value |= (read_slot() << bit);
    }

    return value;
}

void onewire_writebyte(const uint8_t data)
{
    for(uint8_t bit = 0; bit < 8; ++bit)
    {
        // Values are written LSB first
        write_slot((data >> bit) & 1);
    }
}

void onewire_init()
{
    // We never drive a 1, so clear the bit in init
    ONEWIRE_PORT &= ~(1 << ONEWIRE_PIN);
}

uint8_t onewire_check_rom_crc(const uint8_t* romcode, const uint8_t family_code)
{
    return EXIT_SUCCESS;
    /* ROM CRC
     * 64 bit value
     * 63:56 : 8-bit CRC
     * 55:8  : 48-bit serial number
     * 7:0   : 8-bit family code
     */

    if(romcode[0] != family_code)
    {
        if(family_code != 0xFF)
        {
            return EXIT_FAILURE;
        }
    }

    uint16_t crc = 0;
    for(uint8_t i = 0; i < 8; ++i)
    {
        crc = _crc_ibutton_update(crc, romcode[i]);
    }

    return (crc == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

uint8_t onewire_select_device_and_issue_command(const uint8_t cmd, const uint8_t family_code)
{
    uint8_t dev_present = onewire_reset();
    if(dev_present == 0)
    {
        return EXIT_FAILURE;
    }

    onewire_writebyte(CMD_ONEWIRE_READ_ROM);

    uint8_t rom_code[8];
    for(uint8_t i = 0; i < 8; i++)
    {
        rom_code[i] = onewire_readbyte();
    }

    if(onewire_check_rom_crc(&(rom_code[0]), family_code) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    onewire_writebyte(cmd);

    return EXIT_SUCCESS;
}

