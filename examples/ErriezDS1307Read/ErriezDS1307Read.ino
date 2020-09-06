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
 * \brief DS1307 RTC read example for Arduino
 * \details
 *    Source:         https://github.com/Erriez/ErriezDS1307
 *    Documentation:  https://erriez.github.io/ErriezDS1307
 */

#include <Wire.h>
#include <ErriezDS1307.h> 

// Create RTC object
ErriezDS1307 ds1307;


void setup() 
{
    // Initialize serial port
    Serial.begin(115200);
    Serial.println(F("\nErriez DS1307 read example"));

    // Initialize I2C
    Wire.begin();
    Wire.setClock(100000);
    
    // Initialize RTC
    while (!ds1307.begin()) {
        Serial.println(F("Error: DS1307 not found"));
        delay(3000);
    }
    ds1307.setSquareWave(SquareWaveDisable);
}

void loop() 
{
    struct tm dt;

    // Read date/time from RTC
    ds1307.read(&dt);

    // Print date/time
    Serial.println(asctime(&dt));

    // Wait a second
    delay(1000);
}
