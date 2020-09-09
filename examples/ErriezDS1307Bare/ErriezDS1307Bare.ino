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
 * \brief DS1307 I2C RTC bare example without a library
 * \details
 *      Source: https://github.com/Erriez/ErriezArduinoExamples
 */

#include <Wire.h>

// DS1307 RTC I2C address
#define DS1307_I2C_ADDRESS    (0xD0 >> 1)

// Number of characters short string
#define DATE_STRING_SHORT           3

// Month names in flash
const char monthNames_P[] PROGMEM = "JanFebMarAprMayJunJulAugSepOctNovDec";

// Day of the week names in flash
const char dayNames_P[] PROGMEM= "SunMonTueWedThuFriSat";

// Numb buffer to print month / day of the week names
char nameBuffer[DATE_STRING_SHORT + 1];


char *getDayStr(uint8_t wday)
{
    nameBuffer[0] = '\0';
    if (wday <= 6) {
        strncpy_P(nameBuffer, &(dayNames_P[wday * DATE_STRING_SHORT]), DATE_STRING_SHORT);
        nameBuffer[DATE_STRING_SHORT] = '\0';
    }
    return nameBuffer;
}

char *getMonthStr(uint8_t mon)
{
    nameBuffer[0] = '\0';
    if (mon <= 11) {
        strncpy_P(nameBuffer, &(monthNames_P[mon * DATE_STRING_SHORT]), DATE_STRING_SHORT);
        nameBuffer[DATE_STRING_SHORT] = '\0';
    }
    return nameBuffer;
}

uint8_t readRegister(uint8_t reg)
{
    uint8_t value;

    readBuffer(reg, &value, sizeof(value));

    return value;
}

bool writeRegister(uint8_t reg, uint8_t value)
{
    return writeBuffer(reg, &value, 1);
}

bool readBuffer(uint8_t reg, void *bufferRead, uint8_t readLen)
{
    Wire.beginTransmission((uint8_t)DS1307_I2C_ADDRESS);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) {
        memset(bufferRead, 0, readLen);
        return false;
    }
    Wire.requestFrom((uint8_t)DS1307_I2C_ADDRESS, readLen);
    for (uint8_t i = 0; i < readLen; i++) {
        ((uint8_t *)bufferRead)[i] = (uint8_t)Wire.read();
    }
    return true;
}

bool writeBuffer(uint8_t reg, const void *bufferWrite, uint8_t writeLen)
{
    Wire.beginTransmission(DS1307_I2C_ADDRESS);
    Wire.write(reg);
    for (uint8_t i = 0; i < writeLen; i++) {
        Wire.write(((uint8_t *)bufferWrite)[i]);
    }
    if (Wire.endTransmission() != 0) {
        return false;
    }
    return true;
}

uint8_t bcdToDec(uint8_t bcd)
{
    return (uint8_t)(10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}

uint8_t decToBcd(uint8_t dec)
{
    return (uint8_t)(((dec / 10) << 4) | (dec % 10));
}

void dumpRegisters()
{
    uint8_t buf[8];

    // Read all registers
    if (!readBuffer(0, buf, sizeof(buf))) {
        Serial.println(F("Read buffers failed"));
        return;
    }

    // Print all registers
    Serial.println(F("Registers: "));
    for (uint8_t i = 0; i < sizeof(buf); i++) {
        Serial.print(F("   0x0"));
        Serial.print(i);
        Serial.print(F(": "));
        if (buf[i] < 0x10) {
            Serial.print(F("0"));
        }
        Serial.println(buf[i], HEX);
    }
}

void setDateTime(uint8_t hour, uint8_t min, uint8_t sec,
                 uint8_t mday, uint8_t mon, uint16_t year,
                 uint8_t wday)
{
    uint8_t buffer[7];

    Serial.print(F("Set date/time..."));

    // Encode date time from decimal to BCD
    buffer[0] = decToBcd(sec) & 0x7F; // CH bit cleared in seconds register
    buffer[1] = decToBcd(min) & 0x7F;
    buffer[2] = decToBcd(hour) & 0x3F;
    buffer[3] = decToBcd(wday + 1) & 0x07;
    buffer[4] = decToBcd(mday) & 0x3F;
    buffer[5] = decToBcd(mon + 1) & 0x1F;
    buffer[6] = decToBcd(year % 100);

    // Write BCD encoded buffer to RTC registers
    if (!writeBuffer(0x00, buffer, sizeof(buffer))) {
        Serial.println(F("Failed"));
    } else {
        Serial.println(F("OK"));
    }
}

bool getDateTime(uint8_t *hour, uint8_t *min, uint8_t *sec,
                 uint8_t *mday, uint8_t *mon, uint16_t *year,
                 uint8_t *wday)
{
    uint8_t buffer[7];

    // Read time registers
    if (!readBuffer(0x00, buffer, sizeof(buffer))) {
        Serial.println(F("Get date/time failed"));
        return false;
    }

    // BCD decode time registers
    *sec = bcdToDec(buffer[0] & 0x7F);
    *min = bcdToDec(buffer[1] & 0x7F);
    *hour = bcdToDec(buffer[2] & 0x3F);
    *wday = bcdToDec(buffer[3] & 0x07);
    *mday = bcdToDec(buffer[4] & 0x3F);
    *mon = bcdToDec(buffer[5] & 0x1f);
    *year = bcdToDec(buffer[6]) + 100; // 2000-1900

    // Month: 0..11
    if (*mon) {
        (*mon)--;
    }

    // Day of the week: 0=Sunday
    if (*wday) {
        (*wday)--;
    }

    // Check buffer for valid data
    if ((*sec > 59) || (*min > 59) || (*hour > 23) ||
        (*mday < 1) || (*mday > 31) || (*mon > 11) || (*year > 199) ||
        (*wday > 6))
    {
        return false;
    }

    return true;
}

void printDateTime()
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t mday;
    uint8_t mon;
    uint16_t year;
    uint8_t wday;

    // Read date/time
    if (!getDateTime(&hour, &min, &sec, &mday, &mon, &year, &wday)) {
        Serial.println(F("Read date/time failed"));
        return;
    }

    // Print date
    Serial.print(getDayStr(wday));
    Serial.print(F(" "));
    Serial.print(getMonthStr(mon));
    Serial.print(F(" "));
    Serial.print(mday);
    Serial.print(F(" "));
    Serial.print(hour);
    Serial.print(F(":"));
    if (min < 10) {
        Serial.print(F("0"));
    }
    Serial.print(min);
    Serial.print(F(":"));
    if (sec < 10) {
        Serial.print(F("0"));
    }
    Serial.print(sec);
    Serial.print(F(" "));
    Serial.println(year + 1900);
}

void setup()
{
    // Initialize serial
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez DS1307 bare example"));

    // Initialize Wire
    Wire.begin();
    Wire.setClock(100000);

    // Dump registers
    dumpRegisters();

#if 0
    // Clear CH (Clock Halt) bit to start RTC clock
    uint8_t secondsRegister = readRegister(0x00);
    if (secondsRegister & 0x80) {
        Serial.println(F("Starting RTC clock"));
        secondsRegister &= 0x7F;
        writeRegister(0x00, secondsRegister);
    }
#else
    // Set date/time: 12:34:56 31 December 2020 Sunday
    setDateTime(12, 34, 56,  31, 12-1, 2020-1900, 0);
#endif

    // Dump registers
    dumpRegisters();
}

void loop()
{
    // Print RTC time
    printDateTime();

    // Wait
    delay(1000);
}