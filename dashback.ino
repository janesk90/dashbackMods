/*
 * Slightly different solution that uses a delay and sharp change to imitate PODE.
 * Like an actual PODE controller, you may encounter pivot, ledge dash, and first
 * frame tilt issues.
 */

#include "Nintendo.h"

CGamecubeController gcController(4);
CGamecubeConsole gcConsole(3);

// Pin definitions
#define pinLed LED_BUILTIN
#define PODE_TIMER 4

uint8_t lastXAxis;
unsigned long lastXAxisChange; // the last time the x axis changed in value


void setup()
{
    // Set up debug led
    pinMode(pinLed, OUTPUT);
    lastXAxis = 128; // 128 is the default neutral value for the x axis
    lastXAxisChange = 0;
}

void loop()
{
    gcController.read();

    auto gcControllerReport = gcController.getReport();

    auto d = defaultGamecubeData;
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
    
    d.report.yAxis = gcControllerReport.yAxis;
    d.report.cxAxis = gcControllerReport.cxAxis;
    d.report.cyAxis = gcControllerReport.cyAxis;
    d.report.left = gcControllerReport.left;
    d.report.right = gcControllerReport.right;

    if(gcControllerReport.xAxis != lastXAxis) { // if the controller's x axis value doesn't equal the last x axis value we sent to the GameCube
        if(millis() > lastXAxisChange + PODE_TIMER) { // if it's been a few milliseconds since the x axis started to change
            d.report.xAxis = gcControllerReport.xAxis; // imitate the delay and sharp electrical value change of PODE
        } 
        else {
            d.report.xAxis = lastXAxis; // otherwise, we're still in window, so keep sending the last reported x axis value
        }
    }
    else {
        d.report.xAxis = gcControllerReport.xAxis;
    }

    if(lastXAxis != d.report.xAxis) { // if the last x axis value we report doesn't equal this report's x axis value
        lastXAxis = d.report.xAxis; // update the last reported x axis value to this report's x axis value
        lastXAxisChange = millis(); // update the last time the last x axis value changed, reseting the PODE window
    }

    gcConsole.write(d);
}
