
/*************************************************** 
  include file for SEN55
  Sensor use I2C to communicate, 2 pins SCL SDA are required to  interface
  
  2025 - Jay Fox
  https://github.com/javos65/SEN55-SDN

 ****************************************************/

#ifndef _SEN55_
#define _SEN55_

#include <Wire.h>
#include <DEBUGF.h>

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

extern Sen55_Values G_SensorValues;
extern float G_Temperature;
extern float G_Humidity;
extern float G_PM10;
extern float G_PM2_5;
extern float G_PM4_0;
extern float G_PM1_0;
extern float G_PM25;
extern float G_Voc;
extern float G_Nox;
extern uint32_t G_Status;

extern boolean MeasureSEN55();
extern boolean InitSEN55();
extern boolean IdleSEN55();
extern boolean MeasureSEN55();
extern boolean StatusSEN55();

#endif