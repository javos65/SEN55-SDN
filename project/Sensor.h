// ------------------- Sensor initiations and fucntions
//    
//
 
#ifndef __SENSOR_H
#define __SENSOR_H

#include <Arduino.h>
#include "SEN55.h"

#define byte uint8_t
#define LED_BOARD LED_BUILTIN // Wifi1010 io D6 / Nano IOIT 33 io D13



#define SENSORTIMEPOST   300           // seconds base time for every measurement & post ie: 60 seconds = every minute     !! shoud be less than 1 Hour !!
#define SENSORTIMEPOST24 3600        // 1 hour (3600 second) base time for 24h average measurement
#define MAXSAMPLES (60*60/SENSORTIMEPOST) // Buffer size for measuring in 1 hour = 60x60 seconds / TIMEPOST,, ie 60 seconds time post = 60 measures in 1 hours
#define MAXSAMPLES24  24 // Buffer for 24 hourly samples


extern Sen55_Values G_Sensor;  // our Sensor
extern boolean Initiate_Sensor(long timestamp);
extern byte Read_DatastoreSensor();
extern byte Write_DatastoreSensor(long timestamp);
extern int  Count_DatastoreSensor();
extern boolean Read_Sensor(long timestamp);
extern void Calibrate_Sensor(float sample);
void G_ledblink(int loops,int times);

extern void Calculate_Longterm_1h();
extern void Calculate_Longterm_24h();

#ifdef ARDUINO_PORTENTA_C33
    #define LEDRED 34
    #define LEDGREEN 35
    #define LEDBLUE 36
  void G_ledblink(int loops,int times,boolean r,boolean g, boolean b);
#endif

#endif
