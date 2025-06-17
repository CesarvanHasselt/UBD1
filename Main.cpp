#include "mbed.h"

// Define analog inputs (for 3 Sharp GP2Y0A41SK0F sensors)
AnalogIn analog1(A0); // Middenvoor
AnalogIn analog2(A1); // Links
AnalogIn analog3(A2); // Rechts

#define SYSTEM_VOLTAGE 3.3f

// Stub motor control functions (replace with your real control)
void DriveForward()  { printf("Motors: Forward\n"); }
void TurnLeft()      { printf("Motors: Turn Left\n"); }
void TurnRight()     { printf("Motors: Turn Right\n"); }
void Stop()          { printf("Motors: Stop\n"); }

// Conversion function
float convert_to_cm(float voltage) {
    if (voltage <= 0.1f) return 30.0f;
    float distance = 13.0f / (voltage - 0.1f);
    if (distance < 3.0f) distance = 3.0f;
    if (distance > 30.0f) distance = 30.0f;
    return distance;
}

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

    int laatsteRichting = 0; // Keep track of last turn direction

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

        if (detectieVoor == 1) {
            // Object detected in front: keep turning until front clear
            if (laatsteRichting == 1) {
                TurnRight();
            } else if (laatsteRichting == 2) {
                TurnLeft();
            } else {
                // No known direction? Default to turning right
                TurnRight();
                laatsteRichting = 1;
            }
        } else {
            // Front clear: drive forward for 0.5 seconds, then resume normal checks
            DriveForward();
            ThisThread::sleep_for(500ms);
            // Then normal pause before next loop iteration
        }

        ThisThread::sleep_for(100ms);
    }
}
