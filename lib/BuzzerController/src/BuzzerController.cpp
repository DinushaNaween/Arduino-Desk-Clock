#include "BuzzerController.h"
#include <Arduino.h>

BuzzerController::BuzzerController() 
    : pin(BUZZER_PIN), initialized(false), 
      isPlaying(false), patternStartTime(0), 
      currentPattern(BuzzerPattern::SINGLE_BEEP), patternStep(0) {
    ledcChannel = 0;
    pwmEnabled = true; // enable PWM/tone fallback by default on ESP32
        melodyNotes = nullptr;
        melodyDurations = nullptr;
        melodyLen = 0;
        melodyIndex = 0;
        melodyNoteStart = 0;
        melodyPlaying = false;
}

bool BuzzerController::begin(int buzzerPin) {
    pin = buzzerPin;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    initialized = true;
    // Setup ledc channel for tone fallback on ESP32
    // Use a default channel; if multiple buzzers are used, this should be parameterized
    if(pwmEnabled) {
        const int freq = 2000; // default frequency for tone fallback
        const int resolution = 8; // 8-bit resolution
        ledcSetup(ledcChannel, freq, resolution);
        ledcAttachPin(pin, ledcChannel);
        // ensure tone is off
        ledcWriteTone(ledcChannel, 0);
    }
    return true;
}

void BuzzerController::beep(int durationMs) {
    if(!initialized) return;
    if(pwmEnabled) {
        // Use a 2kHz tone for beep
        ledcWriteTone(ledcChannel, 2000);
        delay(durationMs);
        ledcWriteTone(ledcChannel, 0);
    } else {
        digitalWrite(pin, HIGH);
        delay(durationMs);
        digitalWrite(pin, LOW);
    }
}

void BuzzerController::playSingleBeep() {
    unsigned long elapsed = millis() - patternStartTime;
    if(elapsed < 100) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 2000);
        else digitalWrite(pin, HIGH);
    } else if(elapsed < 200) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 0);
        else digitalWrite(pin, LOW);
        isPlaying = false;
    }
}

void BuzzerController::playDoubleBeep() {
    unsigned long elapsed = millis() - patternStartTime;
    if(elapsed < 100) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 2000);
        else digitalWrite(pin, HIGH);
    } else if(elapsed < 150) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 0);
        else digitalWrite(pin, LOW);
    } else if(elapsed < 250) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 2000);
        else digitalWrite(pin, HIGH);
    } else if(elapsed < 350) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 0);
        else digitalWrite(pin, LOW);
        isPlaying = false;
    }
}

void BuzzerController::playTripleBeep() {
    unsigned long elapsed = millis() - patternStartTime;
    if(elapsed < 80) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 2000);
        else digitalWrite(pin, HIGH);
    } else if(elapsed < 120) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 0);
        else digitalWrite(pin, LOW);
    } else if(elapsed < 200) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 2000);
        else digitalWrite(pin, HIGH);
    } else if(elapsed < 240) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 0);
        else digitalWrite(pin, LOW);
    } else if(elapsed < 320) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 2000);
        else digitalWrite(pin, HIGH);
    } else if(elapsed < 400) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 0);
        else digitalWrite(pin, LOW);
        isPlaying = false;
    }
}

void BuzzerController::playAlarm() {
    unsigned long elapsed = millis() - patternStartTime;
    int cycle = (elapsed / 500) % 2;
    if(cycle == 0) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 1200);
        else digitalWrite(pin, HIGH);
    } else {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 0);
        else digitalWrite(pin, LOW);
    }
    // Stop after 3 seconds
    if(elapsed > 3000) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 0);
        else digitalWrite(pin, LOW);
        isPlaying = false;
    }
}

void BuzzerController::playNotification() {
    unsigned long elapsed = millis() - patternStartTime;
    if(elapsed < 50) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 2000);
        else digitalWrite(pin, HIGH);
    } else if(elapsed < 100) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 0);
        else digitalWrite(pin, LOW);
    } else if(elapsed < 150) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 2000);
        else digitalWrite(pin, HIGH);
    } else if(elapsed < 200) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 0);
        else digitalWrite(pin, LOW);
        isPlaying = false;
    }
}

void BuzzerController::playPattern(BuzzerPattern pattern) {
    if(!initialized) return;
    currentPattern = pattern;
    patternStartTime = millis();
    isPlaying = true;
    patternStep = 0;
    // Debug trace
    Serial.print("Buzzer: playPattern called -> ");
    switch(pattern) {
        case BuzzerPattern::SINGLE_BEEP: Serial.println("SINGLE_BEEP"); break;
        case BuzzerPattern::DOUBLE_BEEP: Serial.println("DOUBLE_BEEP"); break;
        case BuzzerPattern::TRIPLE_BEEP: Serial.println("TRIPLE_BEEP"); break;
        case BuzzerPattern::ALARM: Serial.println("ALARM"); break;
        case BuzzerPattern::NOTIFICATION: Serial.println("NOTIFICATION"); break;
    }
}

void BuzzerController::playMelody(const int* notes, const int* durations, int len, bool interrupt) {
    if(!initialized || notes == nullptr || durations == nullptr || len <= 0) return;
    // If requested, stop any current pattern playback
    if(interrupt) {
        isPlaying = false;
        currentPattern = BuzzerPattern::NOTIFICATION; // arbitrary
    }
    melodyNotes = notes;
    melodyDurations = durations;
    melodyLen = len;
    melodyIndex = 0;
    melodyNoteStart = millis();
    melodyPlaying = true;
    // Start first note (if frequency > 0)
    if(pwmEnabled && melodyNotes[0] > 0) ledcWriteTone(ledcChannel, melodyNotes[0]);
    else if(!pwmEnabled && melodyNotes[0] > 0) digitalWrite(pin, HIGH);
}

void BuzzerController::playTone(int freqHz, int durationMs) {
    static int note[1];
    static int dur[1];
    note[0] = freqHz;
    dur[0] = durationMs;
    playMelody(note, dur, 1, true);
}

void BuzzerController::update() {
    if(!initialized) return;

    // Melody playback has priority
    if(melodyPlaying) {
        unsigned long elapsed = millis() - melodyNoteStart;
        if(melodyIndex < melodyLen) {
            if(elapsed >= (unsigned long)melodyDurations[melodyIndex]) {
                // move to next note
                // stop previous note
                if(pwmEnabled) ledcWriteTone(ledcChannel, 0);
                else digitalWrite(pin, LOW);
                melodyIndex++;
                if(melodyIndex >= melodyLen) {
                    melodyPlaying = false;
                    melodyNotes = nullptr;
                    melodyDurations = nullptr;
                    melodyLen = 0;
                    melodyIndex = 0;
                    return;
                }
                // start next note
                melodyNoteStart = millis();
                if(pwmEnabled && melodyNotes[melodyIndex] > 0) ledcWriteTone(ledcChannel, melodyNotes[melodyIndex]);
                else if(!pwmEnabled && melodyNotes[melodyIndex] > 0) digitalWrite(pin, HIGH);
            }
        }
        return;
    }

    if(!isPlaying) return;

    switch(currentPattern) {
        case BuzzerPattern::SINGLE_BEEP:
            playSingleBeep();
            break;
        case BuzzerPattern::DOUBLE_BEEP:
            playDoubleBeep();
            break;
        case BuzzerPattern::TRIPLE_BEEP:
            playTripleBeep();
            break;
        case BuzzerPattern::ALARM:
            playAlarm();
            break;
        case BuzzerPattern::NOTIFICATION:
            playNotification();
            break;
    }
}

void BuzzerController::stop() {
    if(initialized) {
        if(pwmEnabled) ledcWriteTone(ledcChannel, 0);
        else digitalWrite(pin, LOW);
        isPlaying = false;
    }
}

