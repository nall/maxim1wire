/*
 * DS18B20.c
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

#include "DS18B20.h"
#include <util/crc16.h>

uint8_t ds18b20_check_spad_crc(uint8_t* spad)
{
    uint16_t crc = 0;
    for(uint8_t i = 0; i < SPAD_MAX_BYTES; ++i)
    {
        crc = _crc_ibutton_update(crc, spad[i]);
    }

    return (crc == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


uint8_t ds18b20_read_spad(uint8_t* spad)
{
    if(onewire_select_device_and_issue_command(CMD_18B20_READ_SPAD, DS18B20_FAMILY_CODE) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    for(uint8_t i = 0; i < SPAD_MAX_BYTES; i++)
    {
        spad[i] = onewire_readbyte();
    }

#ifdef ONEWIRE_CHECK_CRC
    return ds18b20_check_spad_crc(spad);
#else
    return EXIT_SUCCESS;
#endif // ONEWIRE_CHECK_CRC
}

uint8_t ds18b20_write_spad(uint8_t* spad)
{
    if(onewire_select_device_and_issue_command(CMD_18B20_WRITE_SPAD, DS18B20_FAMILY_CODE) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    onewire_writebyte(spad[SPAD_ALARM_HIGH]);
    onewire_writebyte(spad[SPAD_ALARM_LOW]);
    onewire_writebyte(spad[SPAD_CONFIG]);

    return EXIT_SUCCESS;
}

#if ONEWIRE_USE_MACROS == 0
uint8_t ds18b20_read_temperature(int16_t* temperature)
{
    *temperature = 0;
    if(onewire_select_device_and_issue_command(CMD_18B20_CONV_TEMP, DS18B20_FAMILY_CODE) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    while(1)
    {
        const uint8_t done = onewire_readbyte();

        // There might be a 1 bit anywhere in the output
        // so just check != 0
        if(done != 0)
        {
            break;
        }
    }

    uint8_t spad[SPAD_MAX_BYTES];
    if(ds18b20_read_spad(&(spad[0])) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    *temperature = spad[SPAD_TEMP_MSB];
    *temperature <<= 8;
    *temperature |= spad[SPAD_TEMP_LSB];

    return EXIT_SUCCESS;
}

uint8_t ds18b20_set_resolution(const uint8_t resolution, const uint8_t store_to_eeprom)
{
    uint8_t spad[SPAD_MAX_BYTES];
    if(ds18b20_read_spad(&(spad[0])) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    uint8_t config = spad[SPAD_CONFIG];
    if(((config >> 5) & 3) != resolution)
    {
        // Set resolution to desired value.
        config &= ~(0x3 << 5);
        config |= (resolution << 5);
        spad[SPAD_CONFIG] = config;
        if(ds18b20_write_spad(&(spad[0])) == EXIT_FAILURE)
        {
            return EXIT_FAILURE;
        }

        // Save these new values to EEPROM to save this step
        // next time
        if(store_to_eeprom != 0)
        {
            if(onewire_select_device_and_issue_command(CMD_18B20_COPY_SPAD, DS18B20_FAMILY_CODE) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}
#endif //ONEWIRE_USE_MACROS
