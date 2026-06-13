# Arduino Alarm Clock

This project is an Arduino UNO alarm clock implemented with PlatformIO. It uses an LCD display, a DS1302 real-time clock module, a DHT11 temperature and humidity sensor, four buttons, a buzzer, and status LEDs.

The alarm clock shows the current time, displays date and environment values, allows time/date editing with buttons, saves the alarm setting in EEPROM, and rings the buzzer when the alarm time is reached.

## Hardware

- Arduino UNO
- I2C LCD 16x2 display
- DS1302 RTC module
- DHT11 temperature and humidity sensor
- 4 push buttons
- Buzzer
- Green status LED
- Red alarm LED

## Pin Configuration

The project pin configuration is stored in `include/config.h`.

| Component | Pin |
|---|---|
| RTC DAT | 6 |
| RTC CLK | 7 |
| RTC RST | 5 |
| Buzzer | 3 |
| DHT11 sensor | 8 |
| Button 1 | 10 |
| Button 2 | 11 |
| Button 3 | 12 |
| Button 4 | 13 |
| Green LED | 14 / A0 |
| Red LED | 15 / A1 |
| LCD SDA | A4 |
| LCD SCL | A5 |

## Project Structure

```txt
Arduino/
|-- include/
|   |-- config.h
|   |-- context.h
|   |-- lcd_wrapper.h
|   |-- rtc_wrapper.h
|   |-- screens.h
|   `-- sensors.h
|-- lib/
|   `-- helpers/
|       |-- helpers.cpp
|       `-- helpers.h
|-- src/
|   |-- screens/
|   |   |-- alarm.cpp
|   |   |-- clock.cpp
|   |   `-- init.cpp
|   |-- lcd_wrapper.cpp
|   |-- main.cpp
|   |-- rtc_wrapper.cpp
|   `-- sensors.cpp
`-- platformio.ini
```

## Module Overview

### `src/main.cpp`

Contains the main program loop. The program works as a simple state machine. The current screen is stored in the `screen` variable, and the shared application state is stored in `struct context`.

`main.cpp` does not implement the alarm logic itself. It only calls the active screen function and stores the screen returned by that function.

### `include/context.h`

Defines `struct context`, which stores values that change while the program is running:

- last button states
- selected edit field
- editable time and date values
- alarm time
- alarm enabled/ringing state
- snooze state

This keeps the application state in one place instead of using global state in screen files.

### `src/screens/init.cpp`

Initializes the project after Arduino starts:

- starts Arduino and Serial communication
- initializes context values
- configures button, buzzer, and LED pins
- initializes LCD, RTC, sensors, and alarm state
- displays a short startup message
- switches to the clock screen

### `src/screens/clock.cpp`

Implements the main clock-related screens:

- clock screen
- environment screen
- date screen
- time/date setting screen

The clock screen displays the current time and alarm status. It also checks button input and switches to other screens.

Button actions on the clock screen:

| Button | Action |
|---|---|
| B1 | Show temperature and humidity |
| B2 | Show date |
| B3 | Set time/date |
| B4 | Set alarm |

The time/date setting screen allows editing hour, minute, day, month, and year.

### `src/screens/alarm.cpp`

Implements alarm behavior:

- loads alarm settings from EEPROM
- saves alarm settings to EEPROM
- checks if the current RTC time matches the alarm time
- starts the buzzer when the alarm rings
- handles snooze
- updates status LEDs
- provides the alarm setting screen

Alarm setting fields:

- hour
- minute
- alarm ON/OFF

When the alarm rings:

| Button | Action |
|---|---|
| B1 | Stop alarm |
| B2 | Snooze for 5 minutes |
| B3 | Stop alarm |
| B4 | Stop alarm |

### `src/rtc_wrapper.cpp`

Wraps the DS1302 RTC module. This module is responsible for:

- initializing the RTC
- reading current date and time
- setting date and time
- formatting time and date strings for the LCD

The RTC object is created in this module because the RTC library needs an object to communicate with the hardware.

### `src/lcd_wrapper.cpp`

Wraps the LCD display functions. This module is responsible for:

- initializing the LCD
- clearing the display
- setting the cursor
- printing text
- turning the backlight on or off
- printing two LCD lines at once

The LCD object is created in this module because the LCD library needs an object to communicate with the display.

### `src/sensors.cpp`

Handles the DHT11 sensor:

- reads temperature
- reads humidity
- formats both values for the LCD

The environment screen uses this module to display temperature and humidity.

## Controls

The project uses four buttons. Buttons are configured with `INPUT_PULLUP`, so a pressed button is read as `LOW`.

General navigation:

- B1 usually goes back or opens the environment screen from the main clock screen.
- B2 usually changes the selected edit field.
- B3 increases the selected value.
- B4 decreases the selected value or opens alarm settings from the clock screen.

## Alarm Storage

The alarm hour, minute, and enabled state are saved in EEPROM. This means the alarm setting remains saved even after the Arduino is disconnected from power.

The current clock time is stored by the RTC module. For the RTC to keep time after power loss, the RTC module must have a working backup battery.

## Build and Upload

This project is built with PlatformIO.

Build:

```bash
platformio run
```

Upload to Arduino UNO:

```bash
platformio run --target upload
```

## Libraries

The required libraries are configured in `platformio.ini`:

- `makuna/RTC`
- `adafruit/DHT sensor library`
- `adafruit/Adafruit Unified Sensor`
- `robtillaart/I2C_LCD`

The build flags also enable floating-point formatting support for formatted output.

## Basic Functionality

- Displays the current time.
- Displays date.
- Displays temperature and humidity.
- Allows time and date setting with buttons.
- Allows alarm time setting with buttons.
- Saves alarm settings after power loss.
- Rings using a buzzer.
- Supports 5-minute snooze.
- Shows alarm status with LEDs.
