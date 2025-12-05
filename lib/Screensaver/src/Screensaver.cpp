#include "Screensaver.h"
#include <math.h>

Screensaver::Screensaver(Adafruit_GC9A01A* disp) 
        : display(disp), currentAnimation(AnimationType::BOUNCING_BALL),
            frame(0), lastUpdate(0), updateInterval(SCREENSAVER_UPDATE_INTERVAL),
            angle(0), pulseSize(5), expanding(true) {
        // Initialize multiple balls with different positions, velocities and colors
        for (int i = 0; i < BALL_COUNT; ++i) {
            // Spread five balls centered around the display center
            x[i] = DISPLAY_CENTER_X + (i - 2) * 30; // spread them horizontally
            y[i] = DISPLAY_CENTER_Y + (i - 2) * 10; // slight vertical offset
                // Increase base velocity ~2x to speed up screensaver relative to moderate
                dx[i] = (i % 2 == 0) ? 6 : -4;
                dy[i] = (i % 3 == 0) ? 4 : 6;
            ballRadius[i] = 12 + i * 3;
            // Colors: cycle through red, green, blue, yellow, cyan
            if (i == 0) ballColor[i] = 0xF800; // red
            else if (i == 1) ballColor[i] = 0x07E0; // green
            else if (i == 2) ballColor[i] = 0x001F; // blue
            else if (i == 3) ballColor[i] = 0xFFE0; // yellow
            else ballColor[i] = 0x07FF; // cyan
        }
}

void Screensaver::begin() {
    display->fillScreen(0x0000);
    frame = 0;
    for (int i = 0; i < BALL_COUNT; ++i) {
        x[i] = DISPLAY_CENTER_X + (i - 2) * 30;
        y[i] = DISPLAY_CENTER_Y + (i - 2) * 10;
        dx[i] = (i % 2 == 0) ? 6 : -4;
        dy[i] = (i % 3 == 0) ? 4 : 6;
        ballRadius[i] = 12 + i * 3;
    }
    angle = 0;
    pulseSize = 5;
    expanding = true;
}

void Screensaver::setAnimation(AnimationType type) {
    currentAnimation = type;
    begin();
}

void Screensaver::drawBouncingBall() {
    // Erase previous positions
    for (int i = 0; i < BALL_COUNT; ++i) {
        display->fillCircle(x[i], y[i], ballRadius[i] + 2, 0x0000);
    }

    // Update positions and draw each ball
    for (int i = 0; i < BALL_COUNT; ++i) {
        x[i] += dx[i];
        y[i] += dy[i];

        // Bounce off edges (keep inside 0..239 / 0..239 with margins)
        int r = ballRadius[i];
        if (x[i] <= r || x[i] >= 239 - r) dx[i] = -dx[i];
        if (y[i] <= r || y[i] >= 239 - r) dy[i] = -dy[i];

        // Draw ball with its color and a small highlight
        display->fillCircle(x[i], y[i], r, ballColor[i]);
        display->fillCircle(x[i] - (r/4), y[i] - (r/4), max(2, r/6), 0xFFFF);
    }
}

void Screensaver::drawRotatingCircle() {
    display->fillScreen(0x0000);
    
    int centerX = DISPLAY_CENTER_X;
    int centerY = DISPLAY_CENTER_Y;
    int radius = 80;
    
    // Draw rotating circle
    float rad = angle * 3.14159265359 / 180.0;
    int x = centerX + radius * cos(rad);
    int y = centerY + radius * sin(rad);
    
    display->fillCircle(x, y, 20, 0xFFE0);
    
    // Draw trail
    for(int i = 1; i <= 5; i++) {
        float trailRad = (angle - i * 10) * 3.14159265359 / 180.0;
        int trailX = centerX + radius * cos(trailRad);
        int trailY = centerY + radius * sin(trailRad);
        int alpha = 255 - (i * 40);
        uint16_t color = ((alpha >> 3) << 11) | ((alpha >> 2) << 5) | (alpha >> 3);
        display->fillCircle(trailX, trailY, 20 - i * 2, 0xFFE0);
    }
    
    angle = (angle + 5) % 360;
}

void Screensaver::drawPulsingDot() {
    display->fillScreen(0x0000);
    
    if(expanding) {
        pulseSize += 2;
        if(pulseSize >= 50) expanding = false;
    } else {
        pulseSize -= 2;
        if(pulseSize <= 5) expanding = true;
    }
    
    display->fillCircle(DISPLAY_CENTER_X, DISPLAY_CENTER_Y, pulseSize, 0x07E0);
    display->fillCircle(DISPLAY_CENTER_X, DISPLAY_CENTER_Y, pulseSize / 2, 0x001F);
}

void Screensaver::drawWalkingCharacter() {
    display->fillScreen(0x0000);
    
    int centerX = DISPLAY_CENTER_X;
    int centerY = DISPLAY_CENTER_Y;
    
    // Simple walking character animation
    int legOffset = (frame % 20 < 10) ? 5 : -5;
    
    // Body
    display->fillCircle(centerX, centerY - 20, 15, 0xFFFF);
    
    // Legs
    display->fillRect(centerX - 8, centerY, 6, 30 + legOffset, 0x001F);
    display->fillRect(centerX + 2, centerY, 6, 30 - legOffset, 0x001F);
    
    // Arms
    int armOffset = (frame % 20 < 10) ? -10 : 10;
    display->fillRect(centerX - 20, centerY - 15, 8, 20 + armOffset, 0xFFFF);
    display->fillRect(centerX + 12, centerY - 15, 8, 20 - armOffset, 0xFFFF);
    
    frame++;
    if(frame > 1000) frame = 0;
}

void Screensaver::update() {
    unsigned long now = millis();
    if(now - lastUpdate < updateInterval) {
        return;
    }
    lastUpdate = now;
    
    draw();
}

void Screensaver::draw() {
    switch(currentAnimation) {
        case AnimationType::BOUNCING_BALL:
            drawBouncingBall();
            break;
        case AnimationType::ROTATING_CIRCLE:
            drawRotatingCircle();
            break;
        case AnimationType::PULSING_DOT:
            drawPulsingDot();
            break;
        case AnimationType::WALKING_CHARACTER:
            drawWalkingCharacter();
            break;
    }
}

