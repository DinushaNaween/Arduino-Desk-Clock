#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "../../../include/Config.h"

enum class AnimationType {
    BOUNCING_BALL,
    ROTATING_CIRCLE,
    PULSING_DOT,
    WALKING_CHARACTER
};

class Screensaver {
private:
    Adafruit_GC9A01A* display;
    AnimationType currentAnimation;
    int frame;
    unsigned long lastUpdate;
    unsigned long updateInterval;
    static const int BALL_COUNT = 5;
    int x[BALL_COUNT];
    int y[BALL_COUNT];
    int dx[BALL_COUNT];
    int dy[BALL_COUNT]; // For bouncing balls
    int ballRadius[BALL_COUNT];
    uint16_t ballColor[BALL_COUNT];
    int angle; // For rotating circle
    int pulseSize; // For pulsing dot
    bool expanding; // For pulsing dot
    
    void drawBouncingBall();
    void drawRotatingCircle();
    void drawPulsingDot();
    void drawWalkingCharacter();

public:
    Screensaver(Adafruit_GC9A01A* disp);
    void begin();
    void setAnimation(AnimationType type);
    void update();
    void draw();
};

#endif // SCREENSAVER_H

