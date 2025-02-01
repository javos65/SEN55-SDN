
/*************************************************** 
  Code file for SEN55
  Sensor use I2C to communicate, 2 pins SCL SDA are required to  interface
  
  2025 - Jay Fox
  https://github.com/javos65/SEN55-SDN

 ****************************************************/
#include "SEN55.h"
#include <DEBUGF.h>


/*********** Sensor Structures  **********/
struct Sen55_Cmd {     
uint16_t command;  
uint16_t delay;
uint16_t size;
};

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

struct Sen55_Data {     
uint16_t pm1_0;  
uint16_t pm2_5;
uint16_t pm4_0;
uint16_t pm10;
uint16_t hum;
uint16_t tmp;
uint16_t voc;
uint16_t nox;
uint16_t dummy;
uint32_t status;
};

/*********** Command Array: adr,delay,size  **********/
Sen55_Cmd CommandArray[9]= {    0x0021, 50, 0,     // Measure Start
                                0x0104,200, 0,      // Measure Stop
                                0x03C4, 20, 24,    // Read Sensor
                                0xD014, 20, 48,   // Read Name
                                0xD033, 20, 48,   // Read Serial
                                0xD100, 20, 1,    // Read Firmware Version
                                0xD206, 20, 6,    // Read Status
                                0xD210, 20, 0,    // Clear Status 
                                0x0202, 20, 3     // Read Ready Flag
                                };

/*********** Global Variables  **********/
Sen55_Data   G_SensorData;
Sen55_Values G_SensorValues;
float G_Temperature=0;
float G_Humidity=0;
float G_PM10=0;
float G_PM2_5=0;
float G_PM4_0=0;
float G_PM1_0;
float G_Voc=0;
float G_Nox=0;
uint32_t G_Status=0;

uint8_t G_SensorBuffer[32], G_SensorSize=0;
uint8_t G_CRC_error=0, G_Read_error=0;                                


/*********I2C Read and Write routines *************/

// CRC check 2 bytes
boolean crc_check(uint8_t a, uint8_t b, uint8_t c)
{
  //DEBUGF("[%x]crc[%x]=[%x] - ",a,b,c);
    uint8_t crc = 0xFF;
     crc ^= (a);
        for (uint8_t crc_bit = 8; crc_bit > 0; --crc_bit) {
            if (crc & 0x80) crc = (crc << 1) ^ 0x31u;
            else crc = (crc << 1);
            }
     crc ^= (b);
        for (uint8_t crc_bit = 8; crc_bit > 0; --crc_bit) {
            if (crc & 0x80) crc = (crc << 1) ^ 0x31u;
            else crc = (crc << 1);
            }
 // DEBUGF("[%x]",crc); 
if ( ((uint8_t) crc) == c)  return(true);
else {
  DEBUGF("* CRC error\n");
  return(false);
  }
}

boolean readsensor(uint16_t command, uint8_t *data, uint16_t waitms,uint16_t size)
{
  uint8_t da,t=0;
  //DEBUGF("Read Received : c=%x,d=%x,s=%x",command,waitms,size)
  MyWire.beginTransmission(I2CADDRESS);
  if (MyWire.write( (uint8_t) (command>>8)&0x00FF) != 1) {DEBUGF("! write command[0] failed\n");return false;}
  if (MyWire.write( (uint8_t) (command)&0x00FF) != 1) {DEBUGF("! write command[1] failed\n");return false;}
    delay(waitms);
  MyWire.endTransmission(true); // returns a code 3: Data notacknowledged ? - ignored
  delay(waitms);
 if(size!=0) { // only read if data is required
    MyWire.requestFrom(I2CADDRESS, size);

    // check if the same number of bytes are received that are requested.
    da=MyWire.available(); //DEBUGF(", available data : %d  .",da);
    if (da != size) {DEBUGF("! Datalength not available\n");return false;}

    for (int i = 0; i < size; ++i) {
      if( (i+1)%3 != 0) { 
          *(data+t)=MyWire.read(); //DEBUGF("%x.",*(data+t));
          t++;
          } // byte 0 + 1 = data
        else { 
          crc_check( *(data+t-2),*(data+t-1),MyWire.read());
          }                    // byte 3 = crc
    } 
    //DEBUGF("\n");
  }
  MyWire.endTransmission();
  return true;
}

/********* SEN55 routines *************/

// Read raw sensor Command
boolean ReadSEN55(uint16_t c)
{
boolean st;
uint8_t data[6]; 
uint16_t val; 
if(readsensor(CommandArray[c].command,&G_SensorBuffer[0],CommandArray[c].delay,CommandArray[c].size )){
  G_SensorSize=CommandArray[c].size;
  return(true);
  }
else {DEBUGF("* Read Error\n");return(false);}
}



// Map Sensor raw data into Global variables
void  MapdataSEN55()
{
  // Map global variables
G_Humidity = (float)G_SensorData.hum/100;
G_Temperature =  (float)G_SensorData.tmp/200;
G_Voc = (float)G_SensorData.voc/10;
G_Nox = (float)G_SensorData.nox/10;
G_PM10 = (float)G_SensorData.pm10/10;
G_PM1_0 = (float)G_SensorData.pm1_0/10;
G_PM2_5 = (float)G_SensorData.pm2_5/10;
G_PM4_0 = (float)G_SensorData.pm4_0/10;
  // Map global Sensor Structure 
G_SensorValues.hum=G_Humidity;
G_SensorValues.tmp=G_Temperature;
G_SensorValues.voc=G_Voc;
G_SensorValues.nox=G_Nox;
G_SensorValues.pm10=G_PM10;
G_SensorValues.pm1_0=G_PM1_0;
G_SensorValues.pm2_5=G_PM2_5;;
G_SensorValues.pm4_0=G_PM4_0;

}

// stop measurement, set SEN55 into idle mode
boolean IdleSEN55()
{
if (!ReadSEN55(SEN55_STOPMEASURE)) return(false);
return(true);
}

// start I2C wire, read Sensor data
boolean InitSEN55()
{
MyWire.end();
MyWire.begin();
if(!ReadSEN55(SEN55_READNAME)) return(false);
DEBUGF("* Sensor Name[%s], ",G_SensorBuffer);
if (!ReadSEN55(SEN55_READSERIAL)) return(false);
DEBUGF(" S#[%s],",G_SensorBuffer);
if (!ReadSEN55(SEN55_READFIRMWARE)) return(false);
DEBUGF(" F#[%x]\n",G_SensorBuffer[0]);
return(true);
}

// Do Measurement, read data, read statusregister, map it into Structures
boolean MeasureSEN55()
{
uint8_t *d = (uint8_t *) &G_SensorData;  
if (!ReadSEN55(SEN55_CLRSTATUS)) return(false);
if (!ReadSEN55(SEN55_STARTMEASURE)) return(false);
delay(2000);
if(!StatusSEN55()) return(false);  
if (!ReadSEN55(SEN55_READYFLAG)) return(false);
if(G_SensorBuffer[1]==0x01) { // Measure flag =1
  if(ReadSEN55(SEN55_SENSORREAD) )
    {
      for(uint8_t i=0;i<8;++i) {
        *( d + (2*i)) = *(G_SensorBuffer + (2*i) + 1);  // Map data into Data structure,
        *( d + (2*i) +1) = *(G_SensorBuffer + (2*i));   // Convert big Endian to little endian
          } 
      MapdataSEN55();  
      return(true);
    }
  else return(false);
  }
return(false);
}

boolean StatusSEN55()
{
if (!ReadSEN55(SEN55_READSTATUS)) return(false);
G_SensorData.status= ((uint32_t) G_SensorBuffer[3]<<24)&0xFF000000 + ((uint32_t) G_SensorBuffer[2]<<16)&0x00FF0000  + ((uint32_t) G_SensorBuffer[1]<<8)&0x0000FF00 + ((uint32_t) G_SensorBuffer[0])&0x000000FF;
G_Status=G_SensorData.status;
//DEBUGF("* Status[%08x]\n",G_SensorData.status);
return(true);
}


