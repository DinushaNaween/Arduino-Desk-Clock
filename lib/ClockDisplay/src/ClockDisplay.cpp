#include "ClockDisplay.h"

ClockDisplay::ClockDisplay(Adafruit_GC9A01A* disp, ClockStyle s) 
    : display(disp), style(s), faceDrawn(false), 
      prevHourAngle(-1), prevMinuteAngle(-1), prevSecondAngle(-1),
      cx(DISPLAY_CENTER_X), cy(DISPLAY_CENTER_Y), radius(DISPLAY_RADIUS), bgColor(0x0000) {
    bgOverride = false;
    bgOverrideColor = 0x0000;
        digitalYOffset = 198; // default Y offset for digital time (can be overridden)
        // reasonable min/max for digital Y position (keep within lower third/quarter of screen)
        minDigitalYOffset = cy + radius / 3;
        if(minDigitalYOffset < 10) minDigitalYOffset = 10;
        maxDigitalYOffset = 230;
}

void ClockDisplay::begin() {
    faceDrawn = false;
    prevHourAngle = -1;
    prevMinuteAngle = -1;
    prevSecondAngle = -1;
}

void ClockDisplay::setStyle(ClockStyle s) {
    style = s;
    faceDrawn = false;
}

float ClockDisplay::toRadians(float degrees) {
    return degrees * 3.14159265359 / 180.0;
}

void ClockDisplay::drawHand(float angle, int length, uint16_t color, int thickness) {
    float rad = toRadians(angle - 90); // -90 to start at top
    int x2 = cx + length * cos(rad);
    int y2 = cy + length * sin(rad);
    display->drawLine(cx, cy, x2, y2, color);
    if (thickness > 1) {
        for(int i = 1; i < thickness; i++) {
            float offset = (i - thickness/2.0) * 0.5;
            int x2a = cx + (length + offset) * cos(rad);
            int y2a = cy + (length + offset) * sin(rad);
            display->drawLine(cx, cy, x2a, y2a, color);
        }
    }
}

void ClockDisplay::drawClassicFace() {
    bgColor = bgOverride ? bgOverrideColor : 0x0010;
    display->fillScreen(bgColor);
    
    // Outer ring (ice blue tone)
    display->drawCircle(cx, cy, radius, 0x07FF);
    display->drawCircle(cx, cy, radius-1, 0x07FF);
    
    // Hour markers (12 positions)
    for(int i = 0; i < 12; i++) {
        float angle = i * 30.0 - 90;
        float rad = toRadians(angle);
        int x1 = cx + (radius - 15) * cos(rad);
        int y1 = cy + (radius - 15) * sin(rad);
        int x2 = cx + (radius - 5) * cos(rad);
        int y2 = cy + (radius - 5) * sin(rad);
        display->drawLine(x1, y1, x2, y2, 0xFFFF);
        display->fillCircle(x2, y2, 2, 0xFFFF);
    }
    
    display->fillCircle(cx, cy, 4, 0xFFFF);
}

void ClockDisplay::drawMinimalistFace() {
    bgColor = bgOverride ? bgOverrideColor : 0x0000;
    display->fillScreen(bgColor);
    
    // Use a larger outer circle similar to the classic face (ice blue tone)
    display->drawCircle(cx, cy, radius, 0x07FF);
    display->drawCircle(cx, cy, radius-1, 0x07FF);
    
    // 4 main markers (12, 3, 6, 9)
    for(int i = 0; i < 4; i++) {
        float angle = i * 90.0 - 90;
        float rad = toRadians(angle);
        int x = cx + (radius - 18) * cos(rad);
        int y = cy + (radius - 18) * sin(rad);
        display->fillCircle(x, y, 3, 0x07E0);
    }
    
    display->fillCircle(cx, cy, 3, 0x07E0);
}

void ClockDisplay::drawColorfulFace() {
    bgColor = bgOverride ? bgOverrideColor : 0x1001;
    display->fillScreen(bgColor);
    for(int r = radius; r > 0; r -= 10) {
        uint16_t color = 0x1001 + (r * 0x0100);
        display->drawCircle(cx, cy, r, color);
    }
    
    uint16_t markerColors[12] = {0xF800, 0xFF20, 0x07E0, 0x07FF, 0x001F, 0x801F,
                                 0xF81F, 0xFFE0, 0x07E0, 0x001F, 0x801F, 0xF800};
    for(int i = 0; i < 12; i++) {
        float angle = i * 30.0 - 90;
        float rad = toRadians(angle);
        int x = cx + (radius - 10) * cos(rad);
        int y = cy + (radius - 10) * sin(rad);
        display->fillCircle(x, y, 4, markerColors[i]);
    }
    
    display->fillCircle(cx, cy, 4, 0xFFFF);
}

void ClockDisplay::drawElegantFace() {
    bgColor = bgOverride ? bgOverrideColor : 0x0000;
    display->fillScreen(bgColor);
    
    display->drawCircle(cx, cy, radius, 0xFFE0);
    display->drawCircle(cx, cy, radius-2, 0xFFE0);
    display->drawCircle(cx, cy, radius-4, 0x0000);
    
    for(int i = 0; i < 12; i++) {
        float angle = i * 30.0 - 90;
        float rad = toRadians(angle);
        int x = cx + (radius - 12) * cos(rad);
        int y = cy + (radius - 12) * sin(rad);
        display->fillCircle(x, y, 3, 0xFFE0);
        
        int x2 = cx + (radius - 20) * cos(rad);
        int y2 = cy + (radius - 20) * sin(rad);
        display->fillCircle(x2, y2, 1, 0xFFE0);
    }
    
    display->fillCircle(cx, cy, 5, 0xFFE0);
    display->fillCircle(cx, cy, 2, 0x0000);
}

void ClockDisplay::drawFace() {
    if(faceDrawn) return;
    
    switch(style) {
        case ClockStyle::CLASSIC:
            drawClassicFace();
            break;
        case ClockStyle::MINIMALIST:
            drawMinimalistFace();
            break;
        case ClockStyle::COLORFUL:
            drawColorfulFace();
            break;
        case ClockStyle::ELEGANT:
            drawElegantFace();
            break;
    }
    faceDrawn = true;
}

void ClockDisplay::updateClassicHands(int hours, int minutes, int seconds) {
    if(prevHourAngle >= 0) {
        drawHand(prevHourAngle, 50, bgColor, 4);
        drawHand(prevMinuteAngle, 75, bgColor, 3);
        drawHand(prevSecondAngle, 85, bgColor, 2);
        display->fillCircle(cx, cy, 3, bgColor);
    }
    
    float hourAngle = (hours % 12) * 30.0 + minutes * 0.5;
    float minuteAngle = minutes * 6.0 + seconds * 0.1;
    float secondAngle = seconds * 6.0;
    
    drawHand(hourAngle, 50, 0xFFFF, 4);
    drawHand(minuteAngle, 75, 0x07FF, 3);
    drawHand(secondAngle, 85, 0xF800, 2);
    display->fillCircle(cx, cy, 3, 0x0000);
    display->fillCircle(cx, cy, 4, 0xFFFF);
    
    prevHourAngle = hourAngle;
    prevMinuteAngle = minuteAngle;
    prevSecondAngle = secondAngle;
}

void ClockDisplay::updateMinimalistHands(int hours, int minutes, int seconds) {
    if(prevHourAngle >= 0) {
        drawHand(prevHourAngle, 45, bgColor, 3);
        drawHand(prevMinuteAngle, 70, bgColor, 2);
    }
    
    float hourAngle = (hours % 12) * 30.0 + minutes * 0.5;
    float minuteAngle = minutes * 6.0 + seconds * 0.1;
    
    drawHand(hourAngle, 45, 0x07E0, 3);
    drawHand(minuteAngle, 70, 0x87F0, 2);
    display->fillCircle(cx, cy, 3, 0x07E0);
    
    prevHourAngle = hourAngle;
    prevMinuteAngle = minuteAngle;
    prevSecondAngle = 0;
}

void ClockDisplay::updateColorfulHands(int hours, int minutes, int seconds) {
    if(prevHourAngle >= 0) {
        drawHand(prevHourAngle, 50, bgColor, 4);
        drawHand(prevMinuteAngle, 75, bgColor, 3);
        drawHand(prevSecondAngle, 80, bgColor, 2);
    }
    
    float hourAngle = (hours % 12) * 30.0 + minutes * 0.5;
    float minuteAngle = minutes * 6.0 + seconds * 0.1;
    float secondAngle = seconds * 6.0;
    
    drawHand(hourAngle, 50, 0xFFE0, 4);
    drawHand(minuteAngle, 75, 0x07FF, 3);
    drawHand(secondAngle, 80, 0xF800, 2);
    display->fillCircle(cx, cy, 4, 0xFFFF);
    
    prevHourAngle = hourAngle;
    prevMinuteAngle = minuteAngle;
    prevSecondAngle = secondAngle;
}

void ClockDisplay::updateElegantHands(int hours, int minutes, int seconds) {
    if(prevHourAngle >= 0) {
        drawHand(prevHourAngle, 55, bgColor, 5);
        drawHand(prevMinuteAngle, 80, bgColor, 3);
        drawHand(prevSecondAngle, 75, bgColor, 1);
    }
    
    float hourAngle = (hours % 12) * 30.0 + minutes * 0.5;
    float minuteAngle = minutes * 6.0 + seconds * 0.1;
    float secondAngle = seconds * 6.0;
    
    drawHand(hourAngle, 55, 0xFFE0, 5);
    drawHand(minuteAngle, 80, 0xFFE0, 3);
    drawHand(secondAngle, 75, 0xFFFF, 1);
    display->fillCircle(cx, cy, 5, 0xFFE0);
    display->fillCircle(cx, cy, 2, 0x0000);
    
    prevHourAngle = hourAngle;
    prevMinuteAngle = minuteAngle;
    prevSecondAngle = secondAngle;
}

void ClockDisplay::updateHands(int hours, int minutes, int seconds) {
    if(!faceDrawn) {
        drawFace();
    }
    
    switch(style) {
        case ClockStyle::CLASSIC:
            updateClassicHands(hours, minutes, seconds);
            break;
        case ClockStyle::MINIMALIST:
            updateMinimalistHands(hours, minutes, seconds);
            break;
        case ClockStyle::COLORFUL:
            updateColorfulHands(hours, minutes, seconds);
            break;
        case ClockStyle::ELEGANT:
            updateElegantHands(hours, minutes, seconds);
            break;
    }
}

void ClockDisplay::drawDigitalTime(int hours, int minutes, int seconds, int yOffset) {
    char timeStr[16];
    // Convert to 12-hour format with AM/PM
    int displayHour = hours % 12;
    if(displayHour == 0) displayHour = 12;
    const char* ampm = (hours >= 12) ? "PM" : "AM";
    sprintf(timeStr, "%02d:%02d:%02d %s", displayHour, minutes, seconds, ampm);
    // Draw with proper text bounds calculation (respecting text size)
    display->setTextSize(2);
    display->setTextColor(0xFFFF);

    // Compute centered X and text bounds for erase and collision detection
    int x = 0;
    int16_t bx, by;
    uint16_t bw, bh;
    // getTextBounds expects the intended cursor coordinates
    display->getTextBounds(timeStr, 0, 0, &bx, &by, &bw, &bh);
    x = (240 - bw) / 2;
    display->getTextBounds(timeStr, x, yOffset, &bx, &by, &bw, &bh);

    // Erase the exact area we will draw into
    display->fillRect(bx, by, bw, bh, bgColor);

    display->setCursor(x, yOffset);
    display->print(timeStr);
}

void ClockDisplay::update(int hours, int minutes, int seconds) {
    updateHands(hours, minutes, seconds);
    // Draw digital time at default position (moved up from bottom)
    drawDigitalTime(hours, minutes, seconds, 150);
}

void ClockDisplay::setDigitalYOffset(int y) {
    digitalYOffset = y;
    faceDrawn = false; // force redraw with new position
}

void ClockDisplay::setBackgroundColor(uint16_t color) {
    bgOverride = true;
    bgOverrideColor = color;
    faceDrawn = false; // force redraw with new bg
}

void ClockDisplay::clearBackgroundOverride() {
    bgOverride = false;
    faceDrawn = false;
}

bool ClockDisplay::pointInRect(int px, int py, int rx, int ry, int rw, int rh) {
    return (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh);
}

// Helper: check if two line segments (p1->p2 and p3->p4) intersect
static bool segmentsIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    auto orient = [](int ax, int ay, int bx, int by, int cx, int cy) {
        long long v = (long long)(bx - ax) * (cy - ay) - (long long)(by - ay) * (cx - ax);
        if (v == 0) return 0;
        return v > 0 ? 1 : -1;
    };
    int o1 = orient(x1,y1,x2,y2,x3,y3);
    int o2 = orient(x1,y1,x2,y2,x4,y4);
    int o3 = orient(x3,y3,x4,y4,x1,y1);
    int o4 = orient(x3,y3,x4,y4,x2,y2);

    if (o1 != o2 && o3 != o4) return true;

    return false;
}

bool ClockDisplay::lineIntersectsRect(int x1, int y1, int x2, int y2, int rx, int ry, int rw, int rh) {
    // If either endpoint inside rect
    if (pointInRect(x1, y1, rx, ry, rw, rh) || pointInRect(x2, y2, rx, ry, rw, rh)) return true;

    // Rect corners
    int rx1 = rx, ry1 = ry;
    int rx2 = rx + rw, ry2 = ry;
    int rx3 = rx + rw, ry3 = ry + rh;
    int rx4 = rx, ry4 = ry + rh;

    // Check intersection with each rect edge
    if (segmentsIntersect(x1,y1,x2,y2, rx1,ry1, rx2,ry2)) return true;
    if (segmentsIntersect(x1,y1,x2,y2, rx2,ry2, rx3,ry3)) return true;
    if (segmentsIntersect(x1,y1,x2,y2, rx3,ry3, rx4,ry4)) return true;
    if (segmentsIntersect(x1,y1,x2,y2, rx4,ry4, rx1,ry1)) return true;

    return false;
}

void ClockDisplay::adjustDigitalYOffsetIfOverlaps(const char* timeStr) {
    // Determine text bounds at the current digitalYOffset
    display->setTextSize(2);
    int16_t bx, by;
    uint16_t bw, bh;
    display->getTextBounds(timeStr, 0, 0, &bx, &by, &bw, &bh);
    int baseX = (240 - bw) / 2;

    auto overlapsAt = [&](int testY) -> bool {
        display->getTextBounds(timeStr, baseX, testY, &bx, &by, &bw, &bh);
        int rx = bx;
        int ry = by;
        int rw = bw;
        int rh = bh;

        // Determine hand lengths for current style
        int hourLen = 50, minuteLen = 75, secondLen = 85;
        switch(style) {
            case ClockStyle::CLASSIC:
                hourLen = 50; minuteLen = 75; secondLen = 85; break;
            case ClockStyle::MINIMALIST:
                hourLen = 45; minuteLen = 70; secondLen = 0; break;
            case ClockStyle::COLORFUL:
                hourLen = 50; minuteLen = 75; secondLen = 80; break;
            case ClockStyle::ELEGANT:
                hourLen = 55; minuteLen = 80; secondLen = 75; break;
        }

        // Use prev angles (they represent the currently drawn angles)
        if(prevHourAngle >= 0) {
            float rad = toRadians(prevHourAngle - 90);
            int hx = cx + (int)(hourLen * cos(rad));
            int hy = cy + (int)(hourLen * sin(rad));
            if(lineIntersectsRect(cx, cy, hx, hy, rx, ry, rw, rh)) return true;
        }
        if(prevMinuteAngle >= 0) {
            float rad = toRadians(prevMinuteAngle - 90);
            int mx = cx + (int)(minuteLen * cos(rad));
            int my = cy + (int)(minuteLen * sin(rad));
            if(lineIntersectsRect(cx, cy, mx, my, rx, ry, rw, rh)) return true;
        }
        // Ignore the seconds hand when adjusting digital Y offset to avoid
        // rapid/noticeable jumps caused by the fast-moving seconds hand.

        return false;
    };

    int orig = digitalYOffset;
    // If there's no overlap at the current offset, nothing to do
    if(!overlapsAt(digitalYOffset)) return;

    // Try moving up in small steps first
    int step = 5;
    int y;
    for(y = orig - step; y >= minDigitalYOffset; y -= step) {
        if(!overlapsAt(y)) { digitalYOffset = y; faceDrawn = false; return; }
    }

    // If moving up didn't help, try moving down
    for(y = orig + step; y <= maxDigitalYOffset; y += step) {
        if(!overlapsAt(y)) { digitalYOffset = y; faceDrawn = false; return; }
    }

    // If neither direction cleared it, keep original (or last tried value)
    digitalYOffset = orig;
}

