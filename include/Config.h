#ifndef CONFIG_H
#define CONFIG_H

// SPI Configuration
#define PIN_SCLK 12
#define PIN_MOSI 11

// Display Pin Definitions
// Display 1 (Local Time)
#define DISP1_CS  4
#define DISP1_DC  5
#define DISP1_RST 6

// Display 2 (UTC Time)
#define DISP2_CS  13
#define DISP2_DC  14
#define DISP2_RST 10

// Display 3 (Sensor Data)
#define DISP3_CS  15
#define DISP3_DC  16
#define DISP3_RST 17

// Display 4 (Screensaver)
#define DISP4_CS  21
#define DISP4_DC  38
#define DISP4_RST 39

// I2C Configuration
#define I2C_SDA 9
#define I2C_SCL 8

// I2C Device Addresses
#define RTC_DS3231_ADDRESS 0x68  // DS3231 RTC module
#define EEPROM_AT24C32_ADDRESS 0x57  // AT24C32 EEPROM (on ZS-042 module, optional)
#define AHT10_ADDRESS 0x38  // AHT10 sensor
#ifndef INA219_ADDRESS
#define INA219_ADDRESS 0x40  // INA219 (can be 0x40-0x45 depending on A0/A1 pins)
#endif

// Buzzer Pin
#define BUZZER_PIN 7  // Adjust based on your wiring

// Constants
#define NUM_DISPLAYS 4
#define DISPLAY_CENTER_X 120
#define DISPLAY_CENTER_Y 120
#define DISPLAY_RADIUS 110

// Update Intervals (in milliseconds)
#define SENSOR_UPDATE_INTERVAL 1000
#define DISPLAY_UPDATE_INTERVAL 100
#define SCREENSAVER_UPDATE_INTERVAL 75

// Display Indices
#define DISP_LOCAL_TIME 0
#define DISP_UTC_TIME 1
#define DISP_SENSOR_DATA 2
#define DISP_SCREENSAVER 3

// Wi-Fi credentials (optional). Define these to enable Wi‑Fi + NTP sync at startup.
// Example:
#define WIFI_SSID "SLT-Fiber-G3Fy8-2.4G"
#define WIFI_PASSWORD "5MaRCcYe"

// OpenWeather API settings (optional). Obtain an API key from https://openweathermap.org/
// Provide latitude/longitude for your location.
// Example:
#define OPENWEATHER_API_KEY "5b7ee8031509b582df10dcab0c4ae0a0"
#define OPENWEATHER_LAT 6.9271
#define OPENWEATHER_LON 79.8612

#endif // CONFIG_H
