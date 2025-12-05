# DS3231 RTC Manager

## Module Information
- **Module**: DS3231 Precision RTC (ZS-042 variant)
- **RTC Chip**: DS3231
- **EEPROM**: AT24C32 (optional, not used in current implementation)
- **I2C Address**: 0x68 (DS3231), 0x57 (AT24C32 EEPROM)
- **Battery**: Requires **rechargeable LIR2032** coin cell battery (NOT CR2032)

## Wiring

### I2C Connections (Shared Bus)
```
ESP32-S3          DS3231 ZS-042 Module
--------          --------------------
GPIO 9 (SDA)  ───> SDA
GPIO 8 (SCL)  ───> SCL
3.3V          ───> VCC
GND           ───> GND
```

### I2C Bus Devices
The I2C bus is shared with:
- **AHT10** sensor (address 0x38)
- **INA219** power monitor (address 0x40-0x45)
- **DS3231** RTC (address 0x68)

## Important Notes

### Battery Requirements
⚠️ **CRITICAL**: The ZS-042 module **requires a rechargeable LIR2032** coin cell battery.

**Important Battery Information:**
- **Required Battery Type**: Rechargeable **LIR2032** (3.6V nominal, ~40mAh)
- **DO NOT USE**: Non-rechargeable CR2032 battery (unless charging circuit is disabled)
- **Why?**: The module has a built-in charging circuit that supplies current to the battery when powered via VCC
- **Risk**: Using a non-rechargeable CR2032 can cause battery damage, leakage, or even explosion if the charging circuit attempts to charge it

**How the Battery Works:**
- When the ESP32 is powered, the charging circuit charges the LIR2032 battery
- When power is disconnected, the battery keeps the RTC running
- The RTC maintains time for weeks/months depending on battery capacity

**Installing the Battery:**
1. Insert a **LIR2032** rechargeable coin cell into the battery holder on the ZS-042 module
2. Ensure correct polarity (+ side up, typically)
3. The module will automatically charge the battery when powered
4. First charge: Leave module powered for a few hours to charge the battery

**Alternative (Advanced):**
If you must use a non-rechargeable CR2032, you need to **disable the charging circuit** by removing either:
- The 1N4148 diode, OR
- The 200Ω resistor labeled '102' on the module
*Warning: This modification voids warranty and should only be done by experienced users*

### Setting Time
You can set the time in your code:

```cpp
// In setup() or when needed
rtcManager.setTime(2024, 11, 22, 14, 30, 0); // Year, Month, Day, Hour, Minute, Second
```

### Timezone
Set your local timezone offset:

```cpp
rtcManager.setTimezone(5);  // +5 hours from UTC (e.g., IST)
// or
rtcManager.setTimezone(-5); // -5 hours from UTC (e.g., EST)
```

## Features

- **Precision**: ±2ppm accuracy from -40°C to +85°C
- **Temperature Sensor**: Built-in temperature sensor (accessible via `getTemperature()`)
- **Automatic Detection**: Code detects if RTC lost power
- **Fallback**: If RTC not found, uses millis() as fallback

## Usage Example

```cpp
#include "RTCManager.h"

RTCManager rtc;

void setup() {
    rtc.begin();
    
    // Check if RTC lost power
    if(rtc.hasLostPower()) {
        // Set current time
        rtc.setTime(2024, 11, 22, 14, 30, 0);
    }
    
    // Set timezone (e.g., +5:30 for IST)
    rtc.setTimezone(5); // Adjust for your timezone
}

void loop() {
    // Get local time
    TimeData local = rtc.getLocalTime();
    Serial.print("Local: ");
    Serial.print(local.hour);
    Serial.print(":");
    Serial.println(local.minute);
    
    // Get UTC time
    TimeData utc = rtc.getUTCTime();
    Serial.print("UTC: ");
    Serial.print(utc.hour);
    Serial.print(":");
    Serial.println(utc.minute);
    
    // Get temperature from RTC
    float temp = rtc.getTemperature();
    Serial.print("RTC Temp: ");
    Serial.println(temp);
}
```

## Troubleshooting

### RTC Not Detected
1. Check I2C connections (SDA, SCL)
2. Verify power connections (VCC, GND)
3. Run I2C scanner to check if device at 0x68 is found
4. Check for I2C address conflicts

### Time Resets on Power Off
- Check if a **LIR2032 rechargeable battery** is installed in the battery holder
- Verify battery polarity is correct
- Ensure battery is charged (leave module powered for several hours for initial charge)
- If battery is installed but time still resets, the battery may be dead or not making contact
- Without a battery, you need to set time after each power-on

### Wrong Time
- Verify timezone offset is correct
- Ensure time was set correctly
- Check if RTC lost power flag is set

