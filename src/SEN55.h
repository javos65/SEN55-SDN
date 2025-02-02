
/*************************************************** 
  include file for SEN55
  Sensor use I2C to communicate, 2 pins SCL SDA are required to  interface
  
  2025 - Jay Fox
  https://github.com/javos65/SEN55-SDN

 ****************************************************/

#ifndef _SEN55_
#define _SEN55_

#include <Wire.h>
#include "DEBUGF.h"

#define MyWire Wire2
/*********** Sensor Defines  **********/
#define SEN55_STARTMEASURE  0
#define SEN55_STOPMEASURE  1
#define SEN55_SENSORREAD    2
#define SEN55_READNAME      3
#define SEN55_READSERIAL    4
#define SEN55_READFIRMWARE  5
#define SEN55_READSTATUS    6
#define SEN55_CLRSTATUS     7
#define SEN55_READYFLAG     8
#define I2CADDRESS      0x69

struct Sen55_Values {     
float pm1_0;  
float pm2_5;
float pm4_0;
float pm10;
float hum;
float tmp;
float voc;
float nox;
uint16_t dummy;
uint32_t status;
};

extern boolean MeasureSEN55(struct Sen55_Values *S);
extern boolean InitSEN55();
extern boolean IdleSEN55();

#endif