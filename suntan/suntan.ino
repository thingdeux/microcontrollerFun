// This #include statement was automatically added by the Particle IDE.
#include "VL53L0X.h"
// 30 Seconds - Device listens for 30 then sleeps for 30.
const unsigned int MS_TO_ALLOW_BEFORE_SLEEPING = 30000;
const float SLEEP_DURATION_IN_SECS = 30;
int lastReading = 0;
unsigned int lastSleptTimeInMs = 0;
bool shouldSleep = false;

VL53L0X distanceSensor;

void setup() {
    // Particle.function("getVolts", getVolts);
    Particle.function("getCharge", getStateOfCharge);
    Particle.function("readDist", getReading);
    Particle.variable("currDistance", lastReading);
    Serial.begin(9600);
    setupDistanceSensor();
}

void loop() {
    // manageSleepMode();
}

void manageSleepMode() {
    if (shouldSleep == true) {
        Serial.println("Sleeping for 30 secs");
        shouldSleep = false;
        lastSleptTimeInMs = 0;
        System.sleep(SLEEP_DURATION_IN_SECS);
    } else {
        // Device has just woken up from sleep or just cold booted.
        if (lastSleptTimeInMs == 0) {
            Serial.println("Just woke up - should be for 30 secs");
            lastSleptTimeInMs = millis();
        }
        // Has slept recently - wait for at least as long as MS_TO_ALLOW_BEFORE_SLEEPING
        // Then start another sleep cycle
        if ((millis() - lastSleptTimeInMs) > MS_TO_ALLOW_BEFORE_SLEEPING) {
            Serial.println("Setting should sleep to TRUE");
            shouldSleep = true;
        } else {
            Serial.print(" . ");
        }
    }
}


void setupDistanceSensor() {
    // #if defined LONG_RANGE
    //     // lower the return signal rate limit (default is 0.25 MCPS)
    //     distanceSensor.setSignalRateLimit(0.1);
    //     // increase laser pulse periods (defaults are 14 and 10 PCLKs)
    //     distanceSensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    //     distanceSensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
    // #endif
    Wire.begin();
    distanceSensor.init();
    distanceSensor.setTimeout(500);
    // Increases the accuracy of the reading - allows for 200ms of reading. High speed would be 20ms or 33ms
    distanceSensor.setMeasurementTimingBudget(200000);
    // distanceSensor.startContinuous(100);
}

// Particle Functions
int getReading(String command) {
    int currentReading = distanceSensor.readRangeSingleMillimeters();
    if (distanceSensor.timeoutOccurred()) {
        Serial.print(" TIMEOUT");
        return -1;
    } else {
        lastReading = currentReading;
    }
    return lastReading;
}

int getStateOfCharge(String command) {
    FuelGauge fuel;
    double batteryCharge = 0;
    batteryCharge = fuel.getSoC();
    return batteryCharge;
}
