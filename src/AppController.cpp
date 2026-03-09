#include "AppController.h"
#include "WiFiManager.h"
#include "../lib/WeatherClient/src/WeatherClient.h"

// When true, keeps the original behavior of clearing full screens
// on each update (more flicker). When false, uses partial redraws
// similar to the Focus Table Clock test harness.
static const bool USE_BASELINE_FULL_CLEAR = false;

AppController::AppController() 
    : localClock(nullptr),
      utcClock(nullptr),
      screensaver(nullptr),
    initialized(false),
    lastSensorUpdate(0),
    lastDisplayUpdate(0),
    tempAlertActive(false) {
}

bool AppController::begin() {
    // Initialize display manager
    if(!displayManager.begin()) {
        return false;
    }
    
    // Initialize clock displays
    static ClockDisplay localClockInstance(&displayManager.getDisplay(DISP_LOCAL_TIME), ClockStyle::CLASSIC);
    static ClockDisplay utcClockInstance(&displayManager.getDisplay(DISP_UTC_TIME), ClockStyle::MINIMALIST);
    localClock = &localClockInstance;
    utcClock = &utcClockInstance;
    localClock->begin();
    // Force Display 1 background to black (remove blue background)
    localClock->setBackgroundColor(0x0000);
    utcClock->begin();
    // Move digital time up for both clocks (smaller y moves it upward)
    localClock->setDigitalYOffset(150);
    utcClock->setDigitalYOffset(150);
    
    // Initialize screensaver
    static Screensaver screensaverInstance(&displayManager.getDisplay(DISP_SCREENSAVER));
    screensaver = &screensaverInstance;
    screensaver->begin();
    
    // Initialize sensors
    sensorManager.begin();
    powerMonitor.begin();
    // Initialize weather client
    WeatherClient.begin();
    
    // Initialize RTC (DS3231 ZS-042)
    rtcManager.begin();
    
    // Check if RTC lost power (battery may be missing, not charged, or dead)
    if(rtcManager.hasLostPower()) {
        Serial.println("WARNING: RTC lost power - setting time from compile time!");
        // Automatically set time from compilation date/time
        rtcManager.setTimeFromCompileTime();
        Serial.println("RTC time set from compilation time!");
    } else {
        // Optional: Uncomment to always set time from compile time on each upload
        // rtcManager.setTimeFromCompileTime();
        // Serial.println("RTC time updated from compilation time!");
    }

    // Attempt to sync time over NTP if Wi‑Fi credentials are provided
#if defined(WIFI_SSID)
    Serial.println("Attempting Wi-Fi for NTP sync...");
    if(WiFiManager.begin(WIFI_SSID, WIFI_PASSWORD, 15000)) {
        Serial.println("Wi-Fi connected. Syncing time (Asia/Kolkata)...");
        if(WiFiManager.syncTime("Asia/Kolkata", 10000)) {
            time_t epoch = WiFiManager.getEpoch();
            if(epoch != 0) {
                rtcManager.setTimeFromEpoch(epoch);
                Serial.println("RTC updated from NTP epoch (UTC).");
            } else {
                Serial.println("NTP returned invalid epoch.");
            }
        } else {
            Serial.println("Failed to obtain time from NTP.");
        }
    } else {
        Serial.println("Wi-Fi connect failed or timed out.");
    }
#else
    Serial.println("WIFI_SSID not defined - skipping Wi-Fi/NTP sync.");
#endif
    
    // Always set timezone to IST for local clock display
    // Display 1 (Local Time): IST = UTC+5:30
    rtcManager.setTimezone(5, 30);
    Serial.println("Timezone set to IST (+5:30).");
    
    // Initialize buzzer
    buzzer.begin();
    // Play a short non-blocking welcome jingle (Jingle-like motif)
    Serial.println("Buzzer: startup welcome jingle");
    static const int jingleNotes[] = {
        330, 330, 330, 0,
        330, 330, 330, 0,
        330, 392, 261, 294, 330
    };
    static const int jingleDur[] = {
        150, 150, 300, 150,
        150, 150, 300, 150,
        150, 150, 150, 150, 300
    };
    buzzer.playMelody(jingleNotes, jingleDur, sizeof(jingleNotes)/sizeof(jingleNotes[0]), true);
    
    initialized = true;
    return true;
}

void AppController::drawSensorData(float temp, float hum, float voltage, float current) {
    Adafruit_GC9A01A& disp = displayManager.getDisplay(DISP_SENSOR_DATA);

    static bool layoutInitialized = false;

    // Baseline behavior: keep full-screen clear on every update
    if (USE_BASELINE_FULL_CLEAR) {
        // Clear screen
        disp.fillScreen(0x0000);

        // Draw temperature (moved right to avoid clipping)
        disp.setTextColor(0xFFE0);
        disp.setTextSize(2);
        disp.setCursor(40, 40);
        disp.print("Temp:");
        disp.setTextSize(3);
        disp.setCursor(40, 70);
        disp.print(temp, 1);
        disp.print("C");

        // Draw humidity (aligned with temperature column) - lower slightly
        disp.setTextColor(0x07FF);
        disp.setTextSize(2);
        disp.setCursor(40, 130);
        disp.print("Hum:");
        disp.setTextSize(3);
        disp.setCursor(40, 160);
        disp.print(hum, 1);
        disp.print("%");

        // Draw Wi-Fi/NTP status (two-line format like Temp and Hum)
        disp.setTextColor(0x00FF);
        disp.setTextSize(2);
        disp.setCursor(150, 40);
        disp.print("WiFi:");
        disp.setTextSize(2);  // Smaller font size for value, like voltage/current
        disp.setCursor(150, 70);
        if(WiFiManager.isConnected()) {
            disp.print("OK");
        } else {
            disp.print("RTC");
        }

        // Draw voltage and current next to humidity (voltage on top, amperage below)
        // Voltage (aligned with Hum label) - raise label to match value vertical position
        disp.setTextColor(0x07E0);
        disp.setTextSize(2);
        disp.setCursor(140, 115);
        disp.print("V:");
        // Reduce INA219 value font size to make layout less dominant
        disp.setTextSize(2);
        disp.setCursor(170, 115);
        disp.print(voltage, 2);
        disp.print("V");

        // Current (aligned with Hum value) - raise label to match value vertical position
        disp.setTextColor(0xF800);
        disp.setTextSize(2);
        disp.setCursor(140, 145);
        disp.print("I:");
        // Reduce INA219 value font size to make layout less dominant
        disp.setTextSize(2);
        disp.setCursor(170, 145);
        disp.print(current, 1);
        disp.print("mA");
        return;
    }

    // Optimized behavior: draw static labels once, then only overwrite numeric values
    if (!layoutInitialized) {
        disp.fillScreen(0x0000);

        // Temperature label
        disp.setTextColor(0xFFE0);
        disp.setTextSize(2);
        disp.setCursor(40, 40);
        disp.print("Temp:");

        // Humidity label
        disp.setTextColor(0x07FF);
        disp.setTextSize(2);
        disp.setCursor(40, 130);
        disp.print("Hum:");

        // Wi-Fi label
        disp.setTextColor(0x00FF);
        disp.setTextSize(2);
        disp.setCursor(150, 40);
        disp.print("WiFi:");

        // Voltage label
        disp.setTextColor(0x07E0);
        disp.setTextSize(2);
        disp.setCursor(140, 115);
        disp.print("V:");

        // Current label
        disp.setTextColor(0xF800);
        disp.setTextSize(2);
        disp.setCursor(140, 145);
        disp.print("I:");

        layoutInitialized = true;
    }

    // Temperature value
    disp.setTextColor(0xFFE0, 0x0000);
    disp.setTextSize(3);
    disp.setCursor(40, 70);
    disp.print(temp, 1);
    disp.print("C   "); // extra spaces to clear remnants

    // Humidity value
    disp.setTextColor(0x07FF, 0x0000);
    disp.setTextSize(3);
    disp.setCursor(40, 160);
    disp.print(hum, 1);
    disp.print("%   ");

    // Wi-Fi/NTP status value
    disp.setTextColor(0x00FF, 0x0000);
    disp.setTextSize(2);
    disp.setCursor(150, 70);
    if(WiFiManager.isConnected()) {
        disp.print("OK ");
    } else {
        disp.print("RTC");
    }
    disp.print("   ");

    // Voltage value
    disp.setTextColor(0x07E0, 0x0000);
    disp.setTextSize(2);
    disp.setCursor(170, 115);
    disp.print(voltage, 2);
    disp.print("V   ");

    // Current value
    disp.setTextColor(0xF800, 0x0000);
    disp.setTextSize(2);
    disp.setCursor(170, 145);
    disp.print(current, 1);
    disp.print("mA  ");
}

void AppController::updateSensorDisplay() {
    float temp = sensorManager.readTemperature();
    float hum = sensorManager.readHumidity();
    float voltage = powerMonitor.getVoltage();
    float current = powerMonitor.getCurrent();
    
    // Trigger a small buzzer sound when temperature crosses above 30.0C
    if(temp > 30.0f && !tempAlertActive) {
        buzzer.playPattern(BuzzerPattern::SINGLE_BEEP);
        tempAlertActive = true;
    } else if(temp <= 30.0f && tempAlertActive) {
        // Reset alert state when temperature falls back below or equal threshold
        tempAlertActive = false;
    }

    drawSensorData(temp, hum, voltage, current);
}

void AppController::updateSensors() {
    unsigned long now = millis();
    if(now - lastSensorUpdate >= SENSOR_UPDATE_INTERVAL) {
        sensorManager.update();
        powerMonitor.update();
        // Update weather on its own cadence (default 10 min). The WeatherClient
        // implementation also throttles retries to avoid loop stalls.
        WeatherClient.update();
        updateSensorDisplay();
        lastSensorUpdate = now;
    }
}

void AppController::drawWeatherData() {
    Adafruit_GC9A01A& disp = displayManager.getDisplay(DISP_SCREENSAVER);
    const WeatherData &w = WeatherClient.get();
    
    static bool layoutInitialized = false;

    // Baseline behavior: clear the full screen every time
    if (USE_BASELINE_FULL_CLEAR) {
        // Clear
        disp.fillScreen(0x0000);

        // Header: Today
        disp.setTextSize(2);
        disp.setTextColor(0xFFFF);
        int16_t bx, by; uint16_t bw, bh;
        const char* header = "Today";
        disp.getTextBounds(header, 0, 0, &bx, &by, &bw, &bh);
        int hx = (240 - bw) / 2;
        disp.setCursor(hx, 10);
        disp.print(header);

        // City name
        disp.setTextSize(1);
        String city = w.valid ? w.city : String("Unknown");
        disp.getTextBounds(city.c_str(), 0, 0, &bx, &by, &bw, &bh);
        int cx = (240 - bw) / 2;
        disp.setCursor(cx, 30);
        disp.print(city);

        // Large temperature in center
        disp.setTextSize(4);
        disp.setTextColor(0xFFE0);
        String tstr = w.valid ? String((int)round(w.tempC)) + String("°") : String("--°");
        disp.getTextBounds(tstr.c_str(), 0, 0, &bx, &by, &bw, &bh);
        int tx = (240 - bw) / 2;
        disp.setCursor(tx, 80);
        disp.print(tstr);

        // Simple icons at 12/3/6/9 positions (use condition mapping)
        auto drawSun = [&](int x, int y) {
            disp.fillCircle(x, y, 8, 0xFFE0);
        };
        auto drawCloud = [&](int x, int y) {
            disp.fillCircle(x-6, y, 8, 0xFFFF);
            disp.fillCircle(x+6, y, 8, 0xFFFF);
            disp.fillRect(x-12, y, 24, 10, 0xFFFF);
        };

        String cond = w.valid ? w.condition : String("");
        bool isClear = cond.indexOf("Clear") >= 0;
        bool isClouds = cond.indexOf("Cloud") >= 0;
        bool isRain = cond.indexOf("Rain") >= 0;

        // 12 o'clock
        if(isClear) drawSun(120, 20); else if(isClouds) drawCloud(120, 20); else drawSun(120,20);
        // 3 o'clock
        if(isRain) drawCloud(200, 120); else if(isClear) drawSun(200, 120); else drawCloud(200,120);
        // 6 o'clock
        if(isClouds) drawCloud(120, 200); else drawSun(120, 200);
        // 9 o'clock
        if(isClear) drawSun(40, 120); else drawCloud(40,120);
        return;
    }

    // Optimized behavior: static header, dynamic text/icons with partial clears
    if (!layoutInitialized) {
        disp.fillScreen(0x0000);

        // Header: Today (static)
        disp.setTextSize(2);
        disp.setTextColor(0xFFFF);
        int16_t bx, by; uint16_t bw, bh;
        const char* header = "Today";
        disp.getTextBounds(header, 0, 0, &bx, &by, &bw, &bh);
        int hx = (240 - bw) / 2;
        disp.setCursor(hx, 10);
        disp.print(header);

        layoutInitialized = true;
    }

    int16_t bx, by; uint16_t bw, bh;

    // City name (overwrite with background color)
    disp.setTextSize(1);
    disp.setTextColor(0xFFFF, 0x0000);
    String city = w.valid ? w.city : String("Unknown");
    disp.getTextBounds(city.c_str(), 0, 0, &bx, &by, &bw, &bh);
    int cx = (240 - bw) / 2;
    disp.setCursor(cx, 30);
    disp.print(city);
    disp.print("   ");

    // Large temperature in center (overwrite with background color)
    disp.setTextSize(4);
    disp.setTextColor(0xFFE0, 0x0000);
    String tstr = w.valid ? String((int)round(w.tempC)) + String("°") : String("--°");
    disp.getTextBounds(tstr.c_str(), 0, 0, &bx, &by, &bw, &bh);
    int tx = (240 - bw) / 2;
    disp.setCursor(tx, 80);
    disp.print(tstr);
    disp.print("  ");

    // Simple icons at 12/3/6/9 positions (clear small regions then draw)
    auto clearIconRegion = [&](int x, int y) {
        disp.fillRect(x - 16, y - 16, 32, 32, 0x0000);
    };
    auto drawSun = [&](int x, int y) {
        disp.fillCircle(x, y, 8, 0xFFE0);
    };
    auto drawCloud = [&](int x, int y) {
        disp.fillCircle(x-6, y, 8, 0xFFFF);
        disp.fillCircle(x+6, y, 8, 0xFFFF);
        disp.fillRect(x-12, y, 24, 10, 0xFFFF);
    };

    String cond = w.valid ? w.condition : String("");
    bool isClear = cond.indexOf("Clear") >= 0;
    bool isClouds = cond.indexOf("Cloud") >= 0;
    bool isRain = cond.indexOf("Rain") >= 0;

    // 12 o'clock
    clearIconRegion(120, 20);
    if(isClear) drawSun(120, 20); else if(isClouds) drawCloud(120, 20); else drawSun(120,20);
    // 3 o'clock
    clearIconRegion(200, 120);
    if(isRain) drawCloud(200, 120); else if(isClear) drawSun(200, 120); else drawCloud(200,120);
    // 6 o'clock
    clearIconRegion(120, 200);
    if(isClouds) drawCloud(120, 200); else drawSun(120, 200);
    // 9 o'clock
    clearIconRegion(40, 120);
    if(isClear) drawSun(40, 120); else drawCloud(40,120);

}

void AppController::updateDisplays() {
    unsigned long now = millis();
    if(now - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
        // Get time from RTC
        TimeData localTime = rtcManager.getLocalTime(); // Display 1: local time (IST)
        // Display 2: show EST (UTC-5:00)
        TimeData estTime = rtcManager.getTimeWithOffset(-5, 0); // EST -5:00

        // Update clocks only when the displayed second changes (reduces redundant redraw).
        static int lastClockSecond = -1;
        if (localTime.second != lastClockSecond) {
            localClock->update(localTime.hour, localTime.minute, localTime.second); // Display 1: IST
            utcClock->update(estTime.hour, estTime.minute, estTime.second); // Display 2: EST
            lastClockSecond = localTime.second;
        }

        // Draw weather less frequently; it does not need 10 FPS.
        static unsigned long lastWeatherDraw = 0;
        if (now - lastWeatherDraw >= 1000) {
            drawWeatherData();
            lastWeatherDraw = now;
        }
        
        lastDisplayUpdate = now;
    }
}

void AppController::update() {
    if(!initialized) return;
    
    updateSensors();
    updateDisplays();
    buzzer.update(); // Handle non-blocking buzzer patterns
}

