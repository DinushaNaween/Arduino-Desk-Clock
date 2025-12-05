#ifndef BUZZER_CONTROLLER_H
#define BUZZER_CONTROLLER_H

#include <Arduino.h>
#include "../../../include/Config.h"

enum class BuzzerPattern {
    SINGLE_BEEP,
    DOUBLE_BEEP,
    TRIPLE_BEEP,
    ALARM,
    NOTIFICATION
};

class BuzzerController {
private:
    int pin;
    bool initialized;
    bool isPlaying;
    unsigned long patternStartTime;
    BuzzerPattern currentPattern;
    int patternStep;
    // ledc channel for ESP32 PWM/tone fallback
    int ledcChannel;
    bool pwmEnabled;
    // Melody playback state (non-blocking)
    const int* melodyNotes;
    const int* melodyDurations;
    int melodyLen;
    int melodyIndex;
    unsigned long melodyNoteStart;
    bool melodyPlaying;
    
    void playSingleBeep();
    void playDoubleBeep();
    void playTripleBeep();
    void playAlarm();
    void playNotification();

public:
    BuzzerController();
    bool begin(int buzzerPin = BUZZER_PIN);
    void beep(int durationMs = 100);
    void playPattern(BuzzerPattern pattern);
    void update(); // Call in loop for non-blocking patterns
    void stop();
    // Play a non-blocking melody: arrays of frequencies (Hz) and durations (ms)
    void playMelody(const int* notes, const int* durations, int len, bool interrupt = true);
    // Play a single tone non-blocking
    void playTone(int freqHz, int durationMs);
    bool isActive() const { return isPlaying; }
};

#endif // BUZZER_CONTROLLER_H

