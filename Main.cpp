#include "mbed.h"

// Define analog inputs (for 3 Sharp GP2Y0A41SK0F sensors)
AnalogIn analog1(A0); // Middenvoor
AnalogIn analog2(A1); // Links
AnalogIn analog3(A2); // Rechts

#define SYSTEM_VOLTAGE 3.3f

// Motor pins setup
DigitalOut MLFin1(D12);
DigitalOut MLFin2(D13);
PwmOut MLFpwm(D5);

DigitalOut MLRin3(D11);
DigitalOut MLRin4(D10);
PwmOut MLRpwm(D6);

DigitalOut MRFin1(D7);
DigitalOut MRFin2(D8);
PwmOut MRFpwm(D9);

DigitalOut MRRin3(D4);
DigitalOut MRRin4(D3);
PwmOut MRRpwm(D2);

// Configurable parameters
const float motorSpeed = 0.6f;      // Speed for all motor movements (0.0 to 1.0)
const int turnDelayMs = 1500;       // Time to keep turning (ms) when avoiding obstacles
const int rotateTimeMs = 2000;      // Time to rotate on cliff detection (ms)

// Conversion function
float convert_to_cm(float voltage) {
    if (voltage <= 0.1f) return 30.0f;
    float distance = 13.0f / (voltage - 0.1f);
    if (distance < 3.0f) distance = 3.0f;
    if (distance > 30.0f) distance = 30.0f;
    return distance;
}

// Motor control helper function
void setMotor(DigitalOut& in1, DigitalOut& in2, PwmOut& pwm, bool forward, float speed) {
    if (forward) {
        in1 = 1;
        in2 = 0;
    } else {
        in1 = 0;
        in2 = 1;
    }
    pwm.write(speed);
}

void stopMotors() {
    MLFpwm.write(0.0f);
    MLRpwm.write(0.0f);
    MRFpwm.write(0.0f);
    MRRpwm.write(0.0f);
}

// Movement functions
void driveForward(float speed) {
    setMotor(MLFin1, MLFin2, MLFpwm, true, speed);
    setMotor(MLRin3, MLRin4, MLRpwm, true, speed);
    setMotor(MRFin1, MRFin2, MRFpwm, true, speed);
    setMotor(MRRin3, MRRin4, MRRpwm, true, speed);
}

void driveLeft(float speed) {
    setMotor(MLFin1, MLFin2, MLFpwm, false, speed);
    setMotor(MLRin3, MLRin4, MLRpwm, true, speed);
    setMotor(MRFin1, MRFin2, MRFpwm, true, speed);
    setMotor(MRRin3, MRRin4, MRRpwm, false, speed);
}

void driveRight(float speed) {
    setMotor(MLFin1, MLFin2, MLFpwm, true, speed);
    setMotor(MLRin3, MLRin4, MLRpwm, false, speed);
    setMotor(MRFin1, MRFin2, MRFpwm, false, speed);
    setMotor(MRRin3, MRRin4, MRRpwm, true, speed);
}

void rotateClockwise(float speed) {
    setMotor(MLFin1, MLFin2, MLFpwm, true, speed);
    setMotor(MLRin3, MLRin4, MLRpwm, true, speed);
    setMotor(MRFin1, MRFin2, MRFpwm, false, speed);
    setMotor(MRRin3, MRRin4, MRRpwm, false, speed);
}

void rotateCounterClockwise(float speed) {
    setMotor(MLFin1, MLFin2, MLFpwm, false, speed);
    setMotor(MLRin3, MLRin4, MLRpwm, false, speed);
    setMotor(MRFin1, MRFin2, MRFpwm, true, speed);
    setMotor(MRRin3, MRRin4, MRRpwm, true, speed);
}

// Sensor checks
int MiddenvoorCheck(float voltageVoor) {
    float distance_cm = convert_to_cm(voltageVoor);
    printf("Middenvoor: %.1f cm\t", distance_cm);
    if (distance_cm >= 8.0f && distance_cm <= 20.0f) return 0;      // Safe
    else if (distance_cm > 20.0f) return 2;                         // Cliff / drop
    else return 1;                                                  // Object detected
}

int LinksCheck(float voltageLinks) {
    float distance_cm = convert_to_cm(voltageLinks);
    printf("Links: %.1f cm\t", distance_cm);
    return (distance_cm >= 8.0f && distance_cm <= 20.0f) ? 0 : 1;
}

int RechtsCheck(float voltageRechts) {
    float distance_cm = convert_to_cm(voltageRechts);
    printf("Rechts: %.1f cm\n", distance_cm);
    return (distance_cm >= 8.0f && distance_cm <= 20.0f) ? 0 : 1;
}

// Decide direction to avoid obstacles on sides
int RichtingBepalen(int DetectieLinks, int DetectieRechts)
{
    static int laatsteRichting = 0;  // 0 = unknown, 1 = right, 2 = left, 3 = stuck

    if ((DetectieRechts == 1) && (DetectieLinks == 0)) {
        printf("Decision: go left\n");
        laatsteRichting = 2;
    }
    else if ((DetectieLinks == 1) && (DetectieRechts == 0)) {
        printf("Decision: go right\n");
        laatsteRichting = 1;
    }
    else if ((DetectieRechts == 0) && (DetectieLinks == 0)) {
        printf("Decision: clear both sides\n");
        // Keep last direction or 0 = unknown
    }
    else if ((DetectieRechts == 1) && (DetectieLinks == 1)) {
        printf("Decision: stuck - obstacle both sides\n");
        laatsteRichting = 3;
    }

    return laatsteRichting;
}

int main() {
    printf("GP2Y0A41SK0F IR Sensor Distance & Direction Readings\n");

    // PWM frequency setup
    MLFpwm.period(0.001f);
    MLRpwm.period(0.001f);
    MRFpwm.period(0.001f);
    MRRpwm.period(0.001f);

    int laatsteRichting = 0; // Keep track of last turn direction
    int cliffDetectionCounter = 0;     // Counter for consecutive cliff detections
    const int cliffThreshold = 3;      // Require 3 consecutive cliff detections before reacting

    Timer actionTimer;
    actionTimer.start();

    enum State { FORWARD, TURN_LEFT, TURN_RIGHT, ROTATE, STOPPED };
    State currentState = STOPPED;

    while (true) {
        float vMiddenvoor = analog1.read() * SYSTEM_VOLTAGE;
        float vLinks = analog2.read() * SYSTEM_VOLTAGE;
        float vRechts = analog3.read() * SYSTEM_VOLTAGE;

        int detectieVoor = MiddenvoorCheck(vMiddenvoor);
        int detectieLinks = LinksCheck(vLinks);
        int detectieRechts = RechtsCheck(vRechts);

        int richting = RichtingBepalen(detectieLinks, detectieRechts);
        if (richting != 0) laatsteRichting = richting;

        printf("Detectie Voor: %d, Links: %d, Rechts: %d, Richting: %d\n", 
                detectieVoor, detectieLinks, detectieRechts, richting);

        // Decide next action based on sensor input and timers
        switch(currentState) {
            case FORWARD:
            if (detectieVoor == 1 && detectieLinks == 1 && detectieRechts == 1) {
                // Surrounded - rotate
                rotateClockwise(motorSpeed);
                actionTimer.reset();
                currentState = ROTATE;
                printf("Surrounded: rotating 180 degrees\n");
                break;
            }

            if (detectieVoor == 2) {
                cliffDetectionCounter++;
                if (cliffDetectionCounter >= cliffThreshold) {
                    rotateClockwise(motorSpeed);
                    actionTimer.reset();
                    currentState = ROTATE;
                    printf("Cliff detected 3x: rotate\n");
                    cliffDetectionCounter = 0;
                } else {
                    driveForward(motorSpeed);
                }
            } else {
                cliffDetectionCounter = 0;

                if (detectieVoor == 1) {
                    if (laatsteRichting == 1) {
                        driveRight(motorSpeed);
                        currentState = TURN_RIGHT;
                    } else if (laatsteRichting == 2) {
                        driveLeft(motorSpeed);
                        currentState = TURN_LEFT;
                    } else {
                        driveRight(motorSpeed);
                        currentState = TURN_RIGHT;
                        laatsteRichting = 1;
                    }
                    actionTimer.reset();
                } else {
                    driveForward(motorSpeed);
                }
            }
            break;
            
            case TURN_LEFT:
                if (actionTimer.read_ms() >= turnDelayMs) {
                    // Done turning, go forward
                    driveForward(motorSpeed);
                    currentState = FORWARD;
                }
                break;

            case TURN_RIGHT:
                if (actionTimer.read_ms() >= turnDelayMs) {
                    // Done turning, go forward
                    driveForward(motorSpeed);
                    currentState = FORWARD;
                }
                break;

            case ROTATE:
                if (actionTimer.read_ms() >= rotateTimeMs) {
                    // Done rotating, go forward
                    driveForward(motorSpeed);
                    currentState = FORWARD;
                }
                break;

            case STOPPED:
            default:
                stopMotors();
                currentState = FORWARD;  // Start moving forward
                break;
        }

        ThisThread::sleep_for(20ms); // Small delay for CPU relief & sensor update rate
    }
}
