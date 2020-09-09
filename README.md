# DS1307 high precision I2C RTC library for Arduino

[![Build Status](https://travis-ci.org/Erriez/ErriezDS1307.svg?branch=master)](https://travis-ci.org/Erriez/ErriezDS1307)

This is a DS1307 I2C RTC library for Arduino.

![DS1307](https://raw.githubusercontent.com/Erriez/ErriezDS1307/master/extras/DS1307.png)

## Library features

* libc `<time.h>` compatible
* Read/write date/time `struct tm`
* Set/get Unix epoch UTC `time_t`
* Set/get time (hour, min, sec)
* Set/get date and time (hour, min, sec, mday, mon, year, wday)
* Control `SQW` signal (disable / 1Hz / 4096Hz / 8192Hz / 32768Hz)
* Full RTC register access

## Hardware

Any Arduino hardware with a TWI interface and `Wire.h` support.

## Pins

| Pins board - DS1307            | VCC  | GND  |    SDA     |    SCL     |    SQW     |
| ------------------------------ | :--: | :--: | :--------: | :--------: | :--------: |
| Arduino UNO (ATMega328 boards) |  5V  | GND  |     A4     |     A5     | D2 (INT0)  |
| Arduino Mega2560               |  5V  | GND  |    D20     |    D21     | D2 (INT4)  |
| Arduino Leonardo               |  5V  | GND  |     D2     |     D3     | D7 (INT6)  |
| Arduino DUE (ATSAM3X8E)        | 3V3  | GND  |     20     |     21     |     2      |
| ESP8266                        | 3V3  | GND  | GPIO4 (D2) | GPIO5 (D1) | GPIO0 (D3) |
| ESP32                          | 3V3  | GND  |   GPIO21   |   GPIO22   |   GPIO0    |

Note: Tested ESP8266 / ESP32 boards:

* **ESP8266 boards**: ESP12F / WeMos D1 & R2 / Node MCU v2 / v3
* **ESP32 boards:** WeMos LOLIN32 / LOLIN D32

Other unlisted MCU's may work, but are not tested.

## Examples

Arduino IDE | Examples | Erriez DS1307 RTC:

* [Alarm](https://github.com/Erriez/ErriezDS1307/blob/master/examples/ErriezDS1307Alarm/ErriezDS1307Alarm.ino) Software alarm
* [Bare](https://github.com/Erriez/ErriezDS1307/blob/master/examples/ErriezDS1307Bare/ErriezDS1307Bare.ino) Bare example without a library
* [DumpRegisters](https://github.com/Erriez/ErriezDS1307/blob/master/examples/ErriezDS1307DumpRegisters/ErriezDS1307DumpRegisters.ino) Dump registers for debugging
* [SetBuildDateTime](https://github.com/Erriez/ErriezDS1307/blob/master/examples/ErriezDS1307SetBuildDateTime/ErriezDS1307SetBuildDateTime.ino) Set build date/time
* [SetGetDateTime](https://github.com/Erriez/ErriezDS1307/blob/master/examples/ErriezDS1307SetGetDateTime/ErriezDS1307SetGetDateTime.ino) Set/get date and time
* [SetGetTime](https://github.com/Erriez/ErriezDS1307/blob/master/examples/ErriezDS1307SetGetTime/ErriezDS1307SetGetTime.ino) Set/get time
* [SQWInterrupt](https://github.com/Erriez/ErriezDS1307/blob/master/examples/ErriezDS1307SQWInterrupt/ErriezDS1307SQWInterrupt.ino) 1Hz SQW interrupt pin
* [Terminal](https://github.com/Erriez/ErriezDS1307/blob/master/examples/ErriezDS1307Terminal/ErriezDS1307Terminal.ino) Serial terminal example
* [Test](https://github.com/Erriez/ErriezDS1307/blob/master/examples/ErriezDS1307Test/ErriezDS1307Test.ino) Regression test
* [WriteRead](https://github.com/Erriez/ErriezDS1307/blob/master/examples/ErriezDS1307WriteRead/ErriezDS1307WriteRead.ino) Write/read `struct tm`

## Documentation

* [Doxygen online HTML](https://erriez.github.io/ErriezDS1307)
* [Doxygen PDF](https://github.com/Erriez/ErriezDS1307/blob/master/ErriezDS1307.pdf)
* [DS1307 datasheet](https://github.com/Erriez/ErriezDS1307/blob/master/extras/DS1307.pdf)


## Usage

**Initialization**

```c++
#include <Wire.h>
#include <ErriezDS1307.h>

// Create RTC object
ErriezDS1307 rtc;

void setup()
{
    // Initialize TWI with a 100kHz (default)
    Wire.begin();
    Wire.setClock(100000);
    
    // Initialize RTC
    while (!rtc.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);
    }
}
```

**Check oscillator status at startup**

```c++
// Check oscillator status
if (!rtc.isRunning()) {
    // Error: RTC oscillator stopped. Date/time cannot be trusted. 
    // Set new date/time before reading date/time.

    // Enable oscillator
    rtc.clockEnable(true);
}
```

**Set time**

```c++
// Write time to RTC
if (!rtc.setTime(12, 0, 0)) {
    // Error: Set time failed
}
```

**Get time**

```c++
uint8_t hour;
uint8_t minute;
uint8_t second;

// Read time from RTC
if (!rtc.getTime(&hour, &minute, &second)) {
    // Error: RTC read failed
}
```

**Set date and time**

```c++
// Write RTC date/time: 13:45:09  31 December 2019  2=Tuesday
if (!rtc.setDateTime(13, 45, 9,  31, 12, 2019,  2) {
    // Error: RTC write failed
}
```

**Get date and time**

```c++
uint8_t hour;
uint8_t min;
uint8_t sec;
uint8_t mday;
uint8_t mon;
uint16_t year;
uint8_t wday;

// Read RTC date/time
if (!rtc.getDateTime(&hour, &min, &sec, &mday, &mon, &year, &wday) {
    // Error: RTC read failed
}

// hour: 0..23
// min: 0..59
// sec: 0..59
// mday: 1..31
// mon: 1..12
// year: 2000..2099
// wday: 0..6 (0=Sunday .. 6=Saturday)
```

**Write date/time struct tm**

```c++
struct tm dt;

dt.tm_hour = 12;
dt.tm_min = 34;
dt.tm_sec = 56;
dt.tm_mday = 29;
dt.tm_mon = 1; // 0=January
dt.tm_year = 2020-1900;
dt.tm_wday = 6; // 0=Sunday

if (!rtc.write(&dt)) {
    // Error: RTC Read failed
}
```

**Read date/time struct tm**

```c++
struct tm dt;

// Read RTC date/time
if (!rtc.read(&dt)) {
    // Error: RTC read failed
}
```

**Read Unix Epoch UTC**

```c++
time_t t;

// Read Unix epoch UTC from RTC
if (!rtc.getEpoch(&t)) {
    // Error: RTC read failed
}
```

**Write Unix Epoch UTC**

```c++
// Write Unix epoch UTC to RTC
if (!rtc.setEpoch(1599416430UL)) {
    // Error: Set epoch failed
}
```

**Square Wave Out (SQW)**

```c++
rtc.setSquareWave(SquareWaveDisable);	// Disable
rtc.setSquareWave(SquareWave1024Hz);	// 1024Hz
rtc.setSquareWave(SquareWave4096Hz);	// 4096Hz
rtc.setSquareWave(SquareWave8192Hz);	// 8192Hz
rtc.setSquareWave(SquareWave32768Hz);	// 32768Hz
```


## Library dependencies

* `Wire.h`
* `Terminal.ino` requires `ErriezSerialTerminal` library.


## Library installation

Please refer to the [Wiki](https://github.com/Erriez/ErriezArduinoLibraries/wiki) page.


## More Arduino Libraries from Erriez

* [Erriez Libraries](https://github.com/Erriez/ErriezArduinoLibraries)
