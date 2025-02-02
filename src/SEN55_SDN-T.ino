/*************************************************** 
  This is an example for the SEN55 Environmental Sensor
   NO LIBRARIES, SIMPLE AND DIRECT
  Sensor use I2C to communicate, 2 pins SCL SDA are required to  interface
  
  2025 - Jay Fox
  https://github.com/javos65/SEN55-SDN

 ****************************************************/

#include "SEN55.h"
#include "DEBUGF.h"

Sen55_Values MySensor;

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
if (!MeasureSEN55(&MySensor)) { DEBUGF("* Sensor error");}
else {DEBUGF("Hum%% : %.2f, TempC: %.2f, VOCi : %.2f, NOXi : %.2f\nPM10 : %.2f, PM1.0: %.2f, PM2.5: %.2f, PM4.0: %.2f\nStatus [%08x]\n",
              MySensor.hum,MySensor.tmp,MySensor.voc,MySensor.nox,MySensor.pm10,MySensor.pm1_0,MySensor.pm2_5,MySensor.pm4_0,MySensor.status);
      }
delay(4000);
}

