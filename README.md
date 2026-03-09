# Arduino Desk Clock (ESP32-S3)

This repository contains the firmware for a custom desk clock built around an ESP32-S3 microcontroller. The project drives four small GC9A01A SPI displays and interfaces with various sensors and peripherals to show local/UTC times, environmental data, power measurements, and weather information. Additional features include buzzer alerts, a screensaver, and optional Wi‑Fi/NTP and OpenWeather weather updates.

---

## ⚙️ Features

- **Dual clock displays**: Local time (IST by default) and a second timezone (UTC/EST) via analog & digital clock faces
- **Sensor readouts**: Temperature & humidity from AHT10
- **Power monitoring**: Voltage, current & power from INA219
- **RTC**: DS3231 precision real-time clock with backup battery support
- **Screensaver animations**: Several graphical patterns on a dedicated display
- **Buzzer controller**: Patterns, alarms, melodies with non-blocking playback
- **Optional Wi‑Fi**: Connect to a network for NTP time sync
- **OpenWeather integration**: Fetch and display current weather
- **Modular library design**: Each peripheral/functionality encapsulated in its own library

---

## 🔌 Hardware Requirements

| Component | Notes |
|-----------|-------|
| ESP32-S3 Dev Board | Main MCU; configured for SPI displays and I2C sensors |
| 4× GC9A01A SPI TFT displays | 240×240 pixels; each controlled by CS/DC/RST pins defined in `Config.h` |
| DS3231 RTC module (ZS-042) | I2C address 0x68; requires rechargeable LIR2032 battery |
| AHT10 humidity/temperature sensor | I2C address 0x38 |
| INA219 power monitor | I2C address 0x40‑0x45 (configurable via A0/A1) |
| Optional peripherals | Buzzer, Wi‑Fi antenna (onboard), power supply for sensors/displays |

### Wiring Summary

- **SPI bus**: `SCLK` = GPIO 12, `MOSI` = GPIO 11. Each display has its own `CS`, `DC`, `RST` lines as defined in `Config.h`.
- **I2C bus**: `SDA` = GPIO 9, `SCL` = GPIO 8. Shared by RTC, AHT10, INA219 (and EEPROM if present).
- **Buzzer**: `BUZZER_PIN` = GPIO 7 (can be adjusted).
- **Power**: 3.3 V supply common to ESP32, displays, and sensors; ground shared.

(See `Config.h` for pin definitions and device addresses.)

### RTC Battery Warning

The DS3231 ZS-042 module includes a charging circuit. **Use only a rechargeable LIR2032 battery**. Do **not** install a non‑rechargeable CR2032 unless the charging circuit is disabled by removing the diode or resistor. See `lib/RTCManager/README.md` for detailed instructions.

---

## 📂 Software Architecture

The firmware follows a modular design with custom libraries located under `lib/`. Key modules:

| Library | Purpose |
|---------|---------|
| `DisplayManager` | Initializes SPI bus and manages four `Adafruit_GC9A01A` display objects. |
| `ClockDisplay` | Draws analog/digital clock faces in various styles and updates hands. |
| `SensorManager` | Reads temperature & humidity from AHT10. |
| `PowerMonitor` | Interfaces with INA219 to measure voltage/current/power. |
| `RTCManager` | Wraps `RTClib` DS3231 functionality, timezone handling, and compile‑time/NTP sync. |
| `BuzzerController` | Non‑blocking buzzer patterns, melodies, tone generation. |
| `Screensaver` | Multiple animations for the fourth display. |
| `WiFiManager` | Simplified Wi‑Fi connection and NTP synchronization. |
| `WeatherClient` | Fetches weather data from OpenWeather using HTTP + ArduinoJson. |

High‑level control is implemented in `src/AppController.*`. The `main.cpp` simply initializes the controller and enters a loop calling `app.update()`.

### Application Flow

1. `AppController::begin()` initializes displays, clocks, sensors, power monitor, weather client, RTC, Wi‑Fi (optional), buzzer, and timezone.
2. During each `update()` call:
   - Sensor and power readings are refreshed every second. Temperature alerts trigger buzzer beeps.
   - Displays are updated every 100 ms: clocks advance with RTC, weather is drawn on the screensaver display.
   - Buzzer state machine advances.

Utilities such as `setTimeFromCompileTime()` and `setTimeFromEpoch()` provide easy RTC setup.

### Configuration

All pins, intervals, device addresses, Wi‑Fi credentials, and OpenWeather settings are defined in `include/Config.h`. Modify this file before building to adapt the project to your hardware or location.

Optional features:

- Define `WIFI_SSID`/`WIFI_PASSWORD` to enable Wi‑Fi/NTP sync.
- Define `OPENWEATHER_API_KEY`, `OPENWEATHER_LAT`, and `OPENWEATHER_LON` to enable weather updates.


---

## 🚀 Building & Uploading

This is a PlatformIO project. From the workspace root, use the PlatformIO extension in VS Code or run:

```sh
platformio run --target upload
```

Ensure the correct board is selected in `platformio.ini` (ESP32‑S3). Serial monitor at 115200 baud can be opened with:

```sh
platformio device monitor --baud 115200
```

The device will print initialization messages and any debug output from the modules.

---

## 🛠️ Troubleshooting

- **Displays not working**: Verify SPI wiring and CS/DC/RST pin assignments in `Config.h`.
- **RTC not detected / time resets**: Check I2C wiring, battery installation/charge, and module address. Use I2C scanner.
- **Sensors return zero or invalid values**: Confirm AHT10 power and I2C address; call `sensorManager.begin()` status in serial.
- **Wi‑Fi/NTP issues**: Ensure correct credentials and network range; check `WiFiManager` serial logs.
- **Weather fetch failures**: Verify API key and coordinates; inspect HTTP response in serial output.

Refer to individual library README or source comments for more detailed debugging tips.

---

## 📁 Project Structure

```
platformio.ini                # PlatformIO configuration
include/                     # Shared headers and configuration
  Config.h
lib/                         # Custom libraries grouped by functionality
  BuzzerController/
  ClockDisplay/
  DisplayManager/
  PowerMonitor/
  RTCManager/                 # Includes detailed documentation
  Screensaver/
  SensorManager/
  WeatherClient/
  WiFiManager/
src/                         # Application code
  AppController.cpp/h
  main.cpp
test/                        # Placeholder for unit tests
```

---

## 📝 Notes & Future Work

- Add UI controls (buttons, rotary encoder) to change timezones or switch animations.
- Implement data logging or remote telemetry over Wi‑Fi.
- Enhance screensaver with additional graphics or weather-based themes.
- Consolidate libraries and remove global `Config.h` dependency.

---

For questions or contributions, open an issue or pull request in the repository.

---

*Last updated: March 9, 2026*