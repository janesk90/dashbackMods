/*
 * Slightly different solution that uses a delay and sharp change to imitate PODE.
 * Like an actual PODE controller, you may encounter pivot, ledge dash, and first
 * frame tilt issues.
 *
 * You know, it'd be kinda funny to plug a controller with PODE into this thing
 * and watch what a fucking mess it'd make.
 */

#include "Nintendo.h"

CGamecubeController gcController(4);
CGamecubeConsole gcConsole(3);

// Pin definitions
#define pinLed LED_BUILTIN

// Basically I'm gonna fuck with how many milliseconds I think I need to
// consistently smash turn until I'm happy
#define PODE_TIMER 4

uint8_t lastXAxis; // the last x axis value we tried to send to the GameCube
unsigned long lastXAxisChange; // the last time the x axis value we tried to report to the GameCube changed


void setup()
{
    // Set up debug led
    pinMode(pinLed, OUTPUT);
    lastXAxis = 128; // 128 is the default neutral value for the x axis
    lastXAxisChange = 0;
}

void loop()
{
    auto d = defaultGamecubeData;
    
    // if we fail to read the controller, we're just going to send a controller in the neutral state
    // I'm not sure exactly what impact this will have, since Melee doesn't seem to give a shit if
    // you unplug your controller in the middle of anything
    if(gcController.read())
    {
        auto gcControllerReport = gcController.getReport();

        d.origin = gcController.getOrigin();

        d.report.a = gcControllerReport.a;
        d.report.b = gcControllerReport.b;
        d.report.x = gcControllerReport.x;
        d.report.y = gcControllerReport.y;
        d.report.start = gcControllerReport.start;

        d.report.dleft = gcControllerReport.dleft;
        d.report.dright = gcControllerReport.dright;
        d.report.ddown = gcControllerReport.ddown;
        d.report.dup = gcControllerReport.dup;

        d.report.z = gcControllerReport.z;
        d.report.r = gcControllerReport.r;
        d.report.l = gcControllerReport.l;

        // xAxis isn't included yet, there's some shenanigans to pull
        d.report.yAxis = gcControllerReport.yAxis;
        d.report.cxAxis = gcControllerReport.cxAxis;
        d.report.cyAxis = gcControllerReport.cyAxis;
        d.report.left = gcControllerReport.left;
        d.report.right = gcControllerReport.right;

        // if the controller's x axis value doesn't equal the last x axis value we tried to send to the GameCube
        if(gcControllerReport.xAxis != lastXAxis) {
            if(millis() > lastXAxisChange + PODE_TIMER) { // if it's been a few milliseconds since the x axis started to change
                d.report.xAxis = gcControllerReport.xAxis; // imitate the delay and sharp electrical value change of PODE
                // lastXAxis should only change if the controller's actual x axis value is different after the PODE delay period
                lastXAxis = d.report.xAxis; // update the last attempted-reported x axis value to this report's x axis value
                lastXAxisChange = millis(); // update the last time the last x axis value changed, reseting the PODE window
            } 
            else {
                d.report.xAxis = lastXAxis; // otherwise, we're still in window, so keep sending the last reported x axis value
            }
        }
        else { // if something I didn't think to account for happens or you're just Star Fucking Platinum
            d.report.xAxis = gcControllerReport.xAxis;
        }
    }
    gcConsole.write(d); // we're gucci if we continuously fail to write?
}
