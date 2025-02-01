/*************************************************** 
  This is an example for the SEN55 Environmental Sensor
   NO LIBRARIES, SIMPLE AND DIRECT
  Sensor use I2C to communicate, 2 pins SCL SDA are required to  interface


 ****************************************************/

#include "SEN55.h"
#include <DEBUGF.h>

void setup() {
/*********** Serial SETUP  **********/
int t=10;  //Initialize serial and wait for port to open, max 10 second waiting
  Serial.begin(115200);
  while (!Serial) {
    ; delay(1000);
    if ( (t--)== 0 ) break;
  }
/*********** I2C SEN55SETUP  **********/
delay(1000);
InitSEN55();
}


void loop() {
if (!MeasureSEN55()) { DEBUGF("* Sensor error");}
else {DEBUGF("Hum%% : %.2f, TempC: %.2f, VOCi : %.2f, NOXi : %.2f\nPM10 : %.2f, PM4.0: %.2f, PM2.5: %.2f, PM1.0: %.2f\nStatus [%08x]\n",
      (float)G_Humidity,
      (float)G_Temperature,
      (float)G_Voc,
      (float)G_Nox,
      (float)G_PM10,
      (float)G_PM4_0,
      (float)G_PM2_5,      
      (float)G_PM1_0,
      G_Status);
      }
delay(4000);
}

