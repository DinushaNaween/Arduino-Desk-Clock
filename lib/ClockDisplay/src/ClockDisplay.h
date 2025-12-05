#ifndef CLOCK_DISPLAY_H
#define CLOCK_DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <math.h>
#include "../../../include/Config.h"

enum class ClockStyle {
    CLASSIC,      // Classic analog with hour markers
    MINIMALIST,   // Modern minimalist
    COLORFUL,     // Colorful with gradient
    ELEGANT       // Elegant with gold accents
};

class ClockDisplay {
private:
    Adafruit_GC9A01A* display;
    ClockStyle style;
    bool faceDrawn;
    float prevHourAngle;
    float prevMinuteAngle;
    float prevSecondAngle;
    int cx, cy, radius;
    uint16_t bgColor;
    bool bgOverride;
    uint16_t bgOverrideColor;
    int digitalYOffset;
    int minDigitalYOffset;
    int maxDigitalYOffset;
    // overlap helpers
    bool pointInRect(int px, int py, int rx, int ry, int rw, int rh);
    bool lineIntersectsRect(int x1, int y1, int x2, int y2, int rx, int ry, int rw, int rh);
    void adjustDigitalYOffsetIfOverlaps(const char* timeStr);
    
    // Helper methods
    float toRadians(float degrees);
    void drawHand(float angle, int length, uint16_t color, int thickness);
    void drawClassicFace();
    void drawMinimalistFace();
    void drawColorfulFace();
    void drawElegantFace();
    void updateClassicHands(int hours, int minutes, int seconds);
    void updateMinimalistHands(int hours, int minutes, int seconds);
    void updateColorfulHands(int hours, int minutes, int seconds);
    void updateElegantHands(int hours, int minutes, int seconds);

public:
    ClockDisplay(Adafruit_GC9A01A* disp, ClockStyle s = ClockStyle::CLASSIC);
    void begin();
    void setBackgroundColor(uint16_t color);
    void clearBackgroundOverride();
    void setDigitalYOffset(int y);
    void setStyle(ClockStyle s);
    void drawFace();
    void updateHands(int hours, int minutes, int seconds);
    void drawDigitalTime(int hours, int minutes, int seconds, int yOffset = 180);
    void update(int hours, int minutes, int seconds);
};

#endif // CLOCK_DISPLAY_H

