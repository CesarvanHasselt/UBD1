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

// int speed = 10;
int MiddenvoorCheck(float SensorValueVoor)
{
    int DetectieVoor = 0;

    if ((SensorValueVoor >= 0.37)&&(SensorValueVoor <= 0.44))// safe zone
    {
        DetectieVoor = 0;
    }
    else if (SensorValueVoor < 0.37) //afgrond
    {
        DetectieVoor = 2;
    }
    else if (SensorValueVoor > 0.44) // opject
    {
        DetectieVoor = 1;
    }

    return DetectieVoor;
}

int LinksCheck(float SensorValueLinks)
{
    int DetectieLinks = 0;

    if ((SensorValueLinks < 0.37)&&(SensorValueLinks <= 0.44))
    {
        DetectieLinks = 0;
    }
    else if (SensorValueLinks < 0.37) //afgrond
    {
        DetectieLinks = 1;
    }
    else if (SensorValueLinks > 0.44) // opject
    {
        DetectieLinks = 1;
    }

    return DetectieLinks;
}

int RechtsCheck(float SensorValueRechts)
{
    int DetectieRechts = 0;

    if ((SensorValueRechts < 0.37)&&(SensorValueRechts <= 0.44))
    {
        DetectieRechts = 0;
    }
    else if (SensorValueRechts < 0.37) //afgrond
    {
        DetectieRechts = 1;
    }
    else if (SensorValueRechts > 0.44) // opject
    {
        DetectieRechts = 1;
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
        printf("sensor links %f var %d\n", SensorValueLinks, DetectieLinks);
        printf("sensor rechts %f var %d\n", SensorValueRechts, DetectieRechts);

        //richting bepalen

        // int rechtslinks = RichtingBepalen(DetectieLinks, DetectieRechts); //1 is rechts 2 is voor 3 is links
        // int NaarMotor = richtingVoorBepalen(rechtslinks, DetectieVoor);        

        ThisThread::sleep_for(1000ms);
    }


}
