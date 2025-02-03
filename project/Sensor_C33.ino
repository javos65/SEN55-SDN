/*

Sensor with MQTT posting (and web interface)
Sen55 Sensor on I2C, readinmg Environment Data PM 10, PM2.5, PM4.0,PM 1.0
Portenta C33 based CPU (bus should work on any arduino wih I2C)

Set your Wire Channel

*************************************************************/
#include <DEBUGF.h>
#include "Sensor.h"         // Sensor routines
#include "Network.h"        // Network / Wifi communications routines


#define VERSION "2.1.0"            // Software verson nr
// #define SENSORTIMEPOST 60  // -> defined in Sensor.h
// #define SENSORTIMEPOST24 3600  // -> defined in Sensor.h
#define TIMESAVE 14400 // save every 4 hours (if changed), ~ 6x per day, >15 year data cycles

uint32_t G_Rtcvalue, G_Rtcvalueo,G_Rtcvaluep,G_Rtcvaluep24,G_Ticktime,G_RtcSavetime ;// Global RTC Epoch value for time keeping               

void setup() {
/*********** Serial SETUP  **********/

int t=10;  //Initialize serial and wait for port to open, max 10 second waiting
  Serial.begin(115200);
  while (!Serial) {
    ; delay(1000);
    if ( (t--)== 0 ) break;
  }
 
/*********** Board LED  SETUP  **********/
pinMode(LED_BOARD, OUTPUT);
G_ledblink(2,150,1,1,1);  // white led 2 times - starting
     
/*********** Read Netwprk Secrets -> Wifi & Mqtt  **********/
Read_Secrets();

/*********** WIFI SETUP  **********/
DEBUGF(" * WiFi starting...\n");
if ( !StartWifi()){
   DEBUGF(" ! WiFi cant connect - halted to reset.\n");
    while (true) G_ledblink(3,500,1,0,0);     // don't continue if no wifi blink led red
    }
addIP(&G_Sensor);
G_ledblink(2,150,0,1,0);  // green led 2 times : wifi ok
/*********** Time Keeper and RTC SETUP  **********/
Start_Timekeeper();
G_Rtcvalue =  Get_Timekeeper() ;   
G_RtcSavetime = G_Rtcvalue-1; // set Data store save timer -> SAVETIME
G_Rtcvaluep = G_Rtcvalue-1 ;  // Set Mqtt send Message timer  -> POSTTIME
G_Rtcvaluep24 = G_Rtcvaluep-1 ;  // Set Mqtt send Message timer  -> POSTTIME24
DEBUGF(" * RTC time is %d\n",G_Rtcvalue); 
//*********** Mqtt preparation  **********/
Mqtt_Initiate();

//*********** WATCHDOG SETUP  **********/
// not used

//*********** Sensor : SEN55  **********/ 
if (Initiate_Sensor(Get_Timekeeper()) ) {
    G_ledblink(2,150,0,1,0);    // blink 2 times green : Sensor ok
    //  DEBUGF(" * Read  Data from store: %f\n",G_Sensor.lifetime);   
    } 
  else  {DEBUGF(" ! Sensor Init Failed\n"); 
      G_ledblink(3,500,1,0,0);   }

 
//*********** Read setup from Flash **********/ 
// Done in Sensor Init

}

void loop() {

//test();
G_Rtcvalue =  Get_Timekeeper() ;                               // read time

// check MQTT posting time 
if( ((G_Rtcvalue - G_Rtcvaluep) > SENSORTIMEPOST)  ) {  
  Read_Sensor(G_Rtcvalue);    
  Calculate_Longterm_1h();
  Mqtt_Makemessage(&G_Sensor,G_Rtcvalue);
  Mqtt_Publishstructure();
  G_Rtcvaluep = G_Rtcvalue ; // reset time value for Tick Posting
  G_ledblink(3,150,0,0,1);  // 3 times blue : posted data
  if( ((G_Rtcvalue - G_Rtcvaluep24) > SENSORTIMEPOST24)  ) {  
      Calculate_Longterm_24h();
       G_ledblink(3,150,1,0,1);  // 3 times purple : 24hr calculation
      G_Rtcvaluep24 = G_Rtcvalue ; // reset time value for Tick Posting
  } 
}

// check Save Sensor Data to Flash time 
if( (G_Rtcvalue-G_RtcSavetime) > TIMESAVE ) {
    Write_DatastoreSensor(G_Rtcvalue);                         // write store to Flash  
    G_RtcSavetime = G_Rtcvalue;                                // reset timer value
           G_ledblink(3,150,1,1,0);  // 3 times yellow : data saved
  }

// check daily counter to reset at midnight the NTP time keeper
if( Check_Time(0,0) )  // midnight Hours = 0, Minutes = 0
    {
      Update_Timekeeper() ;
    }

// Check Wifi connection
if ( !CheckWifi() ) StartWifi();     // Start Wifi login again

/*
// Check Web server, post mqtt if signaled
if( CheckServerClients(&G_Sensor)==1 ){
  DEBUGF(" * Req to sen Mqtt, sending..\n")
  Mqtt_Makemessage(&G_Sensor,G_Rtcvalue);
  Mqtt_Publishstructure();
  G_Rtcvaluep = G_Rtcvalue ; // reset time value for Tick Posting   
  }
*/

delay(250);
  
}


void test()
{
  while(1)
  {
  G_Rtcvalue =  Get_Timekeeper() ;      // store time UnixTime
  Read_Sensor(G_Rtcvalue);              // read sensor, fill structure
  DEBUGF("sensor,%d,%d\n",G_Sensor.timestamp,G_Sensor.lifetime);
  delay(250);
  }
}



void myshutdown()
{   
DEBUGF("\n! We gonna shut down ! ...\n"); 
}
