# How to Set RTC Time

## Method 1: Hard-code in AppController.cpp (Simplest)

Edit `src/AppController.cpp` around line 44:

```cpp
rtcManager.setTime(2024, 11, 22, 14, 30, 45);
// Format: year, month, day, hour, minute, second
// Example above: November 22, 2024, 2:30:45 PM
```

**Steps:**
1. Open `src/AppController.cpp`
2. Find line 44: `rtcManager.setTime(2024, 11, 22, 14, 30, 45);`
3. Change to your current date/time
4. Upload code
5. Time is set automatically on startup

**To force set time even if RTC has power:**
Uncomment line 48:
```cpp
rtcManager.setTime(2024, 11, 22, 14, 30, 45);
```

---

## Method 2: Serial Command Interface (Recommended)

Add this to your code for interactive time setting via Serial Monitor.

### Add to AppController.h:
```cpp
void handleSerialCommands(); // Add this to public section
```

### Add to AppController.cpp:
```cpp
void AppController::handleSerialCommands() {
    if(Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        if(cmd.startsWith("SETTIME ")) {
            // Format: SETTIME 2024,11,22,14,30,45
            cmd = cmd.substring(8); // Remove "SETTIME "
            int year, month, day, hour, minute, second;
            
            if(sscanf(cmd.c_str(), "%d,%d,%d,%d,%d,%d", 
                      &year, &month, &day, &hour, &minute, &second) == 6) {
                rtcManager.setTime(year, month, day, hour, minute, second);
                Serial.print("Time set to: ");
                Serial.print(year); Serial.print("-");
                Serial.print(month); Serial.print("-");
                Serial.print(day); Serial.print(" ");
                Serial.print(hour); Serial.print(":");
                Serial.print(minute); Serial.print(":");
                Serial.println(second);
            } else {
                Serial.println("Error: Use format SETTIME 2024,11,22,14,30,45");
            }
        } else if(cmd == "GETTIME") {
            TimeData local = rtcManager.getLocalTime();
            Serial.print("Local Time: ");
            Serial.print(local.year); Serial.print("-");
            Serial.print(local.month); Serial.print("-");
            Serial.print(local.day); Serial.print(" ");
            Serial.print(local.hour); Serial.print(":");
            Serial.print(local.minute); Serial.print(":");
            Serial.println(local.second);
        }
    }
}
```

### Add to AppController::update():
```cpp
void AppController::update() {
    if(!initialized) return;
    
    handleSerialCommands(); // Add this line
    updateSensors();
    updateDisplays();
    buzzer.update();
}
```

**Usage via Serial Monitor:**
- Send: `SETTIME 2024,11,22,14,30,45` to set time
- Send: `GETTIME` to read current time

---

## Method 3: Set Timezone

In `AppController.cpp` line 51, set your timezone:

```cpp
// Examples:
rtcManager.setTimezone(5);   // +5 hours (IST - India)
rtcManager.setTimezone(-5);   // -5 hours (EST - US East)
rtcManager.setTimezone(0);    // UTC
rtcManager.setTimezone(1);    // +1 hour (CET - Central Europe)
```

**Note:** Timezone offset is in hours. For half-hour offsets (like IST +5:30), you'll need to manually adjust.

---

## Quick Reference

### setTime() Format:
```cpp
rtcManager.setTime(year, month, day, hour, minute, second);
```

### Examples:
```cpp
// November 22, 2024, 2:30:45 PM
rtcManager.setTime(2024, 11, 22, 14, 30, 45);

// December 25, 2024, 12:00:00 AM (midnight)
rtcManager.setTime(2024, 12, 25, 0, 0, 0);

// January 1, 2025, 11:59:59 PM
rtcManager.setTime(2025, 1, 1, 23, 59, 59);
```

### Reading Time:
```cpp
TimeData local = rtcManager.getLocalTime();
TimeData utc = rtcManager.getUTCTime();
```

---

## Troubleshooting

**Time not updating?**
- Make sure you're calling `setTime()` after `begin()`
- Check Serial Monitor for "RTC lost power" message
- Verify RTC is connected (I2C address 0x68)

**Wrong timezone?**
- Adjust `setTimezone()` value in AppController.cpp
- Remember: offset is in hours from UTC

**Time resets on power off?**
- Check if battery is installed and working
- Battery should keep time for months if working

