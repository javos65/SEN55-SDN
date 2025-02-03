#include <DEBUGF.h>
#include "Sensor.h"     // Sensor structure
#include "StoreData.h"  // needed for Stored Sensor dataWrite_DatastoreSensor


Sen55_Values G_Sensor;   // our Definition of the global Sensor

float PM10buffer1h[MAXSAMPLES];
float PM25buffer1h[MAXSAMPLES];
float PM10buffer24h[MAXSAMPLES24];
float PM25buffer24h[MAXSAMPLES24];
uint8_t PMbuffer_pointer=0;
uint8_t PMbuffer_pointer24=0;
EEData G_Datastore ={31000,10.0,EEData_ID,2,0}; // initial datastore with ID 0x55
long G_Buffercounter=0;

boolean Initiate_Sensor(long timestamp)
{
 boolean oknok;  
 if (InitSEN55(&G_Sensor)) {
    if( Read_DatastoreSensor() ) {}  //  DEBUGF(" * Read  Data from store: %f\n",G_Sensor.lifetime);   
      else  {DEBUGF(" ! No DataStore Read\n"); }  
    DEBUGF(" * Sensor Lifetime set to %f\n",G_Sensor.lifetime);
    for(uint8_t i=0;i<MAXSAMPLES;++i)
      {
       PM10buffer1h[i] =  G_Sensor.lifetime;
       PM25buffer1h[i] =  G_Sensor.lifetime;
      PMbuffer_pointer=0;
      }
    for(uint8_t i=0;i<MAXSAMPLES24;++i)
      {
       PM10buffer24h[i] =  G_Sensor.lifetime;
       PM25buffer24h[i] =  G_Sensor.lifetime;
      PMbuffer_pointer24=0;
      } 
    oknok=MeasureSEN55(& G_Sensor); 
    oknok=MeasureSEN55(& G_Sensor); 
    oknok=MeasureSEN55(& G_Sensor); 
    IdleSEN55();
     Calculate_Longterm_1h();
     Calculate_Longterm_24h();      
   }
 //DEBUGF(" * Sensor initiated\n");
 return(oknok); 
}


byte Read_DatastoreSensor()
{
  Mount_Qspi();
  if( Read_FData(&G_Datastore) ){              // check Datastore in Flash
    G_Sensor.lifetime= G_Datastore.lifetime;
    return(1);
    }
  else
    {
    DEBUGF(" * No Flash Sensor data found, set your actual Lifetime data via webhost interface\n");
    return(0);
    }
}

byte Write_DatastoreSensor(long timestamp)
{
  Mount_Qspi();
  G_Datastore.lifetime = G_Sensor.lifetime;
  G_Datastore.time = timestamp;
  if( Save_FData(&G_Datastore) ){              // Write Datastore in Flash, coutner is increased automaticly
    return(1);
    }
  else
    {
    DEBUGF(" * No Flash Meter file found, data not saved.\n");
    return(0);
    }  
}

int  Count_DatastoreSensor()
{
  return(G_Datastore.counter);
}

// Read Sensor / called every SENSORTIMEPOST time
boolean Read_Sensor(long timestamp)
{
boolean oknok;  
oknok=MeasureSEN55(& G_Sensor);  // read once to activate fan
delay(5000);                     // let fan active
oknok=MeasureSEN55(& G_Sensor);  // read again after 5 seconds
G_Sensor.timestamp = timestamp;
IdleSEN55();                     // Turn fan off - Idle
  G_Sensor.lifetime = (G_Sensor.lifetime*999 + 0.5*(G_Sensor.pm10+G_Sensor.pm2_5) ) / 1000; // lifetime value over 10000 averages PM10 and PM2.5
return(oknok); 
}

// Calculate 1h value out of 1 hour samples / called every measurement in the hour
void Calculate_Longterm_1h() {
float lt10=0;
float lt25=0; 
    PM10buffer1h[PMbuffer_pointer] =  G_Sensor.pm10;     
    PM25buffer1h[PMbuffer_pointer] =  G_Sensor.pm2_5;
    PMbuffer_pointer = (PMbuffer_pointer+1)%MAXSAMPLES; // increase pointer looped
    for(uint8_t i=0;i<MAXSAMPLES;++i)
      {
       lt10 +=PM10buffer1h[i] ;
       lt25 +=PM25buffer1h[i] ;
      }
G_Sensor.pm10_1h=lt10/MAXSAMPLES;
G_Sensor.pm2_5_1h=lt25/MAXSAMPLES;
  DEBUGF(" # 1hr Avg PM10:%.2f PM2.5:%.2f\n",G_Sensor.pm10_1h,G_Sensor.pm2_5_1h);
}

// Calculate 24h value out of hourly samples : called hourly
void Calculate_Longterm_24h() {
float lt10=0;
float lt25=0; 
    PM10buffer24h[PMbuffer_pointer24] =  PM10buffer1h[PMbuffer_pointer];    
    PM25buffer24h[PMbuffer_pointer24] =  PM25buffer1h[PMbuffer_pointer];
    PMbuffer_pointer24 = (PMbuffer_pointer24+1)%MAXSAMPLES24; // increase pointer looped
    for(uint8_t i=0;i<MAXSAMPLES24;++i)
      {
       lt10 +=PM10buffer24h[i] ;
       lt25 +=PM25buffer24h[i] ;
      }
G_Sensor.pm10_24h=lt10/MAXSAMPLES24;
G_Sensor.pm2_5_24h=lt25/MAXSAMPLES24;
  DEBUGF(" # 24hr Avg PM10:%.2f PM2.5:%.2f\n",G_Sensor.pm10_24h,G_Sensor.pm2_5_24h);
}


void Calibrate_Sensor(float sample)
{
//
}

void G_ledblink(int loops,int times)
{
for(int t=0;t<loops;++t)
  {
  digitalWrite(LED_BOARD, LOW); delay(times/2); 
  digitalWrite(LED_BOARD, HIGH); delay(times/2); 
  }
}

void G_ledblink(int loops,int times,boolean r,boolean g, boolean b)
{
  pinMode(LEDRED,OUTPUT);
  pinMode(LEDGREEN,OUTPUT);
  pinMode(LEDBLUE,OUTPUT);
for(int t=0;t<loops;++t)
  {
  delay(times/2); 
  digitalWrite(LEDRED, !r); 
  digitalWrite(LEDGREEN, !g); 
  digitalWrite(LEDBLUE, !b);    
  delay(times/2); 
  digitalWrite(LEDRED, HIGH); 
  digitalWrite(LEDGREEN, HIGH); 
  digitalWrite(LEDBLUE, HIGH);     
  delay(times/2); 

  }
}
