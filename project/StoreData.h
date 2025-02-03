
/*****************************************************************************
* | File      	:	StoreData.h
* | Function    :	Storing data on QSPI for C33 on LittleFileSystem
* | Info        : JV 2024
*----------------
* |	This version:   V1.0
* | Date        :   2024-01-16
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __STOREDATA_H
#define __STOREDATA_H

#include <Arduino.h>
#include <DEBUGF.h>

#define FS_NAME "qspi"
#define FOLDER_NAME "C33"
#define FILEHS "sensorstore.bin"
#define CFILEHS "credentials.bin"
#define EEData_ID 0xDA        // EEprom /FFS save ID for Data object to retrieve
#define EECREDENTIAL_ID 0xC4  // EEprom /FFS save ID for credential object to retrieve
#define EE_SEED 77            // Seed for Cyphering
#define EE_MAXCHAR 32         // structure for FFs storage credentials : max string size=32


struct EECredentials {         
char ssid[EE_MAXCHAR];
char wifipass[EE_MAXCHAR];
char login1[EE_MAXCHAR];
char pass1[EE_MAXCHAR];
char mqttadr[2*EE_MAXCHAR];
char mqtttop[2*EE_MAXCHAR];
char mqttlogin[EE_MAXCHAR];
char mqttpass[EE_MAXCHAR];
byte identity;
int counter;
byte stop; // dummy
};

struct EEData {         
long time;
float lifetime;
uint8_t identity;
int counter;
uint8_t stop; // dummy
};


extern int      Mount_Qspi();
extern int      Delete_DFile();
extern int      Delete_CFile();
extern int      Check_Filesize();
extern uint8_t  Read_FData(struct EEData *O);
extern uint8_t  Save_FData(struct EEData *O);
extern uint8_t  Read_CData(struct EECredentials  *O);
extern uint8_t  Save_CData(struct EECredentials  *O);
extern uint8_t  Debug_FData(struct EEData *O);
extern uint8_t  Debug_CData(struct EECredentials  *O);

extern byte SimpleDecypher(char * textin, char * textout);
extern byte SimpleCypher(char * textin, char * textout);

#endif