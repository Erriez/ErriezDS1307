/*
 * MIT License
 *
 * Copyright (c) 2020 Erriez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*!
 * \brief DS1307 RTC set date/time example for Arduino
 * \details
 *    Source:         https://github.com/Erriez/ErriezDS1307
 *    Documentation:  https://erriez.github.io/ErriezDS1307
 *
 *    Connect the SQW/OUT pin to an Arduino interrupt pin
 */

#include <Wire.h>
#include <ErriezDS1307.h>   // https://github.com/Erriez/ErriezDS1307

// Create DS1307 RTC object
ErriezDS1307 ds1307;

// Global date/time object
struct tm dt;


void rtcInit()
{
    // Initialize RTC
    while (!ds1307.begin()) {
        Serial.println(F("Error: DS1307 not found"));
        delay(3000);
    }

    // Set square wave out pin
    // SquareWaveDisable, SquareWave1Hz, SquareWave4096Hz, SquareWave8192Hz, SquareWave32768Hz
    ds1307.setSquareWave(SquareWaveDisable);
}

bool getBuildTime(const char *str)
{
    int hour;
    int minute;
    int second;

    // Convert build time macro to time
    if (sscanf(str, "%d:%d:%d", &hour, &minute, &second) != 3) {
        return false;
    }

    dt.tm_hour = hour;
    dt.tm_min = minute;
    dt.tm_sec = second;

    return true;
}

bool getBuildDate(const char *str)
{
    const char *monthName[12] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    char month[12];
    int dayMonth;
    int year;
    uint8_t monthIndex;

    // Convert build time macro to date
    if (sscanf(str, "%s %d %d", month, &dayMonth, &year) != 3) {
        return false;
    }

    // Convert month string to month number
    for (monthIndex = 0; monthIndex < 12; monthIndex++) {
        if (strcmp(month, monthName[monthIndex]) == 0) {
            break;
        }
    }
    if (monthIndex >= 12) {
        // Not found
        return false;
    }

    dt.tm_mday = dayMonth;
    dt.tm_mon = monthIndex;
    dt.tm_year = year - 1900;

    return true;
}

void rtcSetDateTime()
{
    Serial.print(F("Build date time: "));

    // Convert compile date/time to date/time string
    if (!getBuildDate(__DATE__) || !getBuildTime(__TIME__)) {
        Serial.print(F("FAILED"));
        while (1) {
            delay(1000);
        }
    }

    // Print build date/time
    Serial.println(asctime(&dt));

    // Set new date time
    Serial.print(F("Set RTC date time..."));
    if (ds1307.write(&dt)) {
        Serial.println(F("OK"));
    } else {
        Serial.println(F("FAILED"));
    }
}

void setup()
{
    // Startup delay to initialize serial port
    delay(500);

    // Initialize serial port
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS1307 RTC Set Time example\n"));

    // Initialize TWI
    Wire.begin();
    Wire.setClock(100000);

    // Initialize RTC
    rtcInit();

    // Set date/time
    rtcSetDateTime();
}

void loop()
{
    // Get date/time
    if (!ds1307.read(&dt)) {
        Serial.println(F("DS1307 read failed"));
    } else {
        Serial.println(asctime(&dt));
    }

    // Wait some time
    delay(1000);
}