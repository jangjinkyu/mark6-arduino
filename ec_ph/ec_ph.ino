/*
 * file DFRobot_EC.ino
 * @ https://github.com/DFRobot/DFRobot_EC
 *
 * This is the sample code for Gravity: Analog Electrical Conductivity Sensor / Meter Kit V2 (K=1.0), SKU: DFR0300.
 * In order to guarantee precision, a temperature sensor such as DS18B20 is needed, to execute automatic temperature compensation.
 * You can send commands in the serial monitor to execute the calibration.
 * Serial Commands:
 *   enterec -> enter the calibration mode
 *   calec -> calibrate with the standard buffer solution, two buffer solutions(1413us/cm and 12.88ms/cm) will be automaticlly recognized
 *   exitec -> save the calibrated parameters and exit from calibration mode
 *
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * version  V1.0
 * date  2018-03-21
 */

#include "DFRobot_EC.h"
#include "DFRobot_PH.h"
#include <EEPROM.h>

#define EC_PIN A1
#define PH_PIN A2

float evoltage,ecValue,etemperature = 25;
float pvoltage,phValue,ptemperature = 25;

DFRobot_EC ec;
DFRobot_PH ph;

void setup()
{
  Serial.begin(115200);  
  ec.begin();
  ph.begin();
}

void loop()
{
    getEc();
    delay(300);
    getPh();
}

void getEc(){
  static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U)  //time interval: 1s
    {
      timepoint = millis();
      evoltage = analogRead(EC_PIN)/1024.0*5000;   // read the evoltage
      //temperature = readTemperature();          // read your temperature sensor to execute temperature compensation
      ecValue =  ec.readEC(evoltage,etemperature);  // convert evoltage to EC with temperature compensation
      Serial.print("etemperature:");
      Serial.print(etemperature,1);
      Serial.print("^C  EC:");
      Serial.print(ecValue,2);
      Serial.println("ms/cm");
    }
    ec.calibration(evoltage,etemperature);          // calibration process by Serail CMD
}

void getPh(){
      static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U){                  //time interval: 1s
        timepoint = millis();
        //temperature = readTemperature();         // read your temperature sensor to execute temperature compensation
       pvoltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
        phValue = ph.readPH(pvoltage,ptemperature);  // convert voltage to pH with temperature compensation
        Serial.print("ptemperature:");
        Serial.print(ptemperature,1);
        Serial.print("^C  pH:");
        Serial.println(phValue,2);
    }
    ph.calibration(pvoltage,ptemperature);           // calibration process by Serail CMD
}

float readTemperature()
{
  //add your code here to get the temperature from your temperature sensor
}
