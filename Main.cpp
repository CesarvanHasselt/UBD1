
/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"
#include <cstdio>

AnalogIn IRrangefinder_middenvoor(A0);
AnalogIn IRrangefinder_links(A1);
AnalogIn IRrangefinder_rechts(A2);
// AnalogIn IRrangefinder_middenlinks(PA_2);
// AnalogIn IRrangefinder_links(PA_4);

AnalogIn IRsensorL();
AnalogIn IRsensorR();
 
// Define Motor Left Front
DigitalOut MLFin1(PA_10);
DigitalOut MLFin2(PB_3);
PwmOut MLFpwm(PB_4);  

// Define Motor Right Front 
DigitalOut MRFin1(D4);
DigitalOut MRFin2(D7);
PwmOut MRFpwm(D6);  

// Define Motor Left Rear
DigitalOut MLRin1(PA_10);
DigitalOut MLRin2(PB_3);
PwmOut MLRpwm(PB_4);  

// Define Motor Right Rear
DigitalOut MRRin1(D4);
DigitalOut MRRin2(D7);
PwmOut MRRpwm(D6);    

void DriveForward()
{
    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

}

void DriveBackward()
{
    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

}

void DriveToTheRight()
{
    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

}

void DriveToTheLeft()
{
    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

}
void TurnAround()
{
    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

    MLFin1 = 0; MLFin2 = 0;
    MLFpwm.write(1.0f);

}

float convert_to_cm(float voltage) {
    // If voltage is very low or equal to offset, return max distance
    if (voltage <= 0.42f) return 30.0f;

    float a = 27.86f; // scale factor
    float b = 0.42f;  // voltage offset

    float distance = a / (voltage - b);

    // Clamp distance within sensor specs
    if (distance < 3.0f) distance = 3.0f;
    else if (distance > 30.0f) distance = 30.0f;

    return distance;
}



int MiddenvoorCheck(float voltageVoor) {
    float distance_cm = convert_to_cm(voltageVoor);
    int DetectieVoor = 0;

    printf("Voor: %.1f cm\t", distance_cm);  // No newline here

    if (distance_cm >= 8.0f && distance_cm <= 20.0f) {
        DetectieVoor = 0; // Safe
    } else if (distance_cm > 20.0f) {
        DetectieVoor = 2; // Cliff / drop
    } else if (distance_cm < 8.0f) {
        DetectieVoor = 1; // Object detected
    }

    return DetectieVoor;
}

int LinksCheck(float voltageLinks) {
    float distance_cm = convert_to_cm(voltageLinks);
    int DetectieLinks = 0;

    printf("Links: %.1f cm\t", distance_cm);  // No newline here

    if (distance_cm >= 8.0f && distance_cm <= 20.0f) {
        DetectieLinks = 0; // Safe
    } else {
        DetectieLinks = 1; // Either object or cliff
    }

    return DetectieLinks;
}

int RechtsCheck(float voltageRechts) {
    float distance_cm = convert_to_cm(voltageRechts);
    int DetectieRechts = 0;

    printf("Rechts: %.1f cm\n", distance_cm);  // Final newline here

    if (distance_cm >= 8.0f && distance_cm <= 20.0f) {
        DetectieRechts = 0; // Safe
    } else {
        DetectieRechts = 1; // Either object or cliff
    }

    return DetectieRechts;
}


int RichtingBepalen(int DetectieLinks, int DetectieRechts)
{
    static int laatsteRichting = 0;  // 0 = onbekend, 1 = rechts, 2 = links

    if ((DetectieRechts == 1) && (DetectieLinks == 0)) {
        printf("links leeg\n");
        laatsteRichting = 1;
    }
    else if ((DetectieLinks == 1) && (DetectieRechts == 0)) {
        printf("rechts leeg\n");
        laatsteRichting = 2;
    }
    else if ((DetectieRechts == 0) && (DetectieLinks == 0)) {
        printf("leeg\n");
        // laatsteRichting blijft ongewijzigd
    }
    else if ((DetectieRechts == 1) && (DetectieLinks == 1)) {
        printf("vast\n");
        laatsteRichting = 3;
    }

    return laatsteRichting;
}

int richtingVoorBepalen(int richtingVoorBepalen, int DetectieVoor)
{
    printf("richtingVoorBepalen :%d \n", richtingVoorBepalen);
    if (DetectieVoor == 0)
    {
        printf("ga naar voren\n");
    }
    else if ((DetectieVoor =! 0)&&(richtingVoorBepalen == 1))//ga naar rechts
    {
        printf("ga naar rechts\n");
    }
    else if ((DetectieVoor =! 0)&&(richtingVoorBepalen == 2)) // ga naar links
    {
        printf("ga naar links\n");
    }
    else if (DetectieVoor == 2)
    {
        printf("draai om\n");
    }
    return 0;
}

int main()
{
    
    while (true)
    {
        float SensorValueVoor = IRrangefinder_middenvoor.read();
        float SensorValueLinks = IRrangefinder_links.read();
        float SensorValueRechts = IRrangefinder_rechts.read();

        int DetectieVoor = MiddenvoorCheck(SensorValueVoor);
        int DetectieLinks = LinksCheck(SensorValueLinks);
        int DetectieRechts = RechtsCheck(SensorValueRechts);

        //printf("sensor voor %f var %d\n", SensorValueVoor, DetectieVoor);
        // printf("sensor links %f var %d\n", SensorValueLinks, DetectieLinks);
        // printf("sensor rechts %f var %d\n", SensorValueRechts, DetectieRechts);
        //richting bepalen

        // int rechtslinks = RichtingBepalen(DetectieLinks, DetectieRechts); //1 is rechts 2 is voor 3 is links
        // int NaarMotor = richtingVoorBepalen(rechtslinks, DetectieVoor);        

        ThisThread::sleep_for(1000ms);
    }


}
