/*
  Portenta C33 - Test QSPI Flash with Little FS

  The sketch shows how to mount an QSPI and list its content.
  The circuit:  - Portenta C33 + Portenta Breakout 

Target is to create an maintain this file : /qspi/C33/highscore.bin, and write a Data Structure in raw bytes.

*/
#include "StoreData.h"

////////// THIS IS YOUR SECRET  !!! //////
EECredentials C_Object = {"MYWIRELESS","PAss1","mylogin","PassX","192.168.200.20","nodered/sensor/PM2/state","admin","am223220",EECREDENTIAL_ID,10,0}; // Global Variable Object structure
////////// THIS IS YOUR SECRET  !!! //////
EECredentials X_Object = {"MYtestnetwork","testpass","Testadmin","xxxtestxxx","192.168.100.63","topic/test","admintest","testcode",0x10,1,0}; // Global Variable Test structure

EEData G_Object = {5555555,6.66,EEData_ID,10,0}; // Global Variable Object structure example
EEData T_Object =  {0,0.0,0x33,0,0}; ;

void setup() {
  Serial.begin(115200);
  while(!Serial) {}
}


void loop() {

Serial.println("\nMenu");
Serial.println("1= Data Store Test");
Serial.println("2= Credential Store Test");
Serial.println("3= Read All");
Serial.println("4= Write&Test All");
Serial.println("Your input ?\n");

while(!Serial.available() ) {} // wait for input
  char i = Serial.read();
  if (i=='1') test_datastore();
  if (i=='2') test_credentialstore();
  if (i=='3') test_readall();  
  if (i=='4') test_writeall();  
  Serial.flush();
}


void test_writeall()
{
// Write Object data, and write to FS
 Mount_Qspi();
 DEBUGF("Write Cred: ");Debug_CData(&C_Object);
 Save_CData(&C_Object);

// Read FS system Test data and print
 Mount_Qspi();
 Read_CData(&X_Object);
 DEBUGF("Read Cred: ");Debug_CData(&X_Object);

// Change data, and write to FS
 Mount_Qspi();
 G_Object.lifetime+=1.1;
 DEBUGF("Write Object :");Debug_FData(&G_Object);
 Save_FData(&G_Object);

// Read FS system data and print
 Mount_Qspi();
 Read_FData(&T_Object);
 DEBUGF("Read Object :");Debug_FData(&T_Object);

}


void test_readall()
{

// Read FS system Test data and print
 Mount_Qspi();
 Read_CData(&X_Object);
 DEBUGF("Read Cred: ");Debug_CData(&X_Object);

// Read FS system data and print
 Mount_Qspi();
 Read_FData(&T_Object);
 DEBUGF("Read Object :");Debug_FData(&T_Object);

}


void test_credentialstore()
{
// Print Data EEdata Structures
DEBUGF("Print X: ");Debug_CData(&X_Object);
DEBUGF("Print C: ");Debug_CData(&C_Object);

// Read FS system Test-data and print
 Mount_Qspi();
 Read_CData(&X_Object);
 DEBUGF("Read X: ");Debug_CData(&X_Object);

// Write Object data, and write to FS
 Mount_Qspi();
 DEBUGF("Write C: ");Debug_CData(&C_Object);;
Save_CData(&C_Object);

// Read FS system Test data and print
 Mount_Qspi();
 Read_CData(&X_Object);
 DEBUGF("Read X: ");Debug_CData(&X_Object);
}

void test_datastore()
{
  // Print Data EEdata Structures
DEBUGF("Null Object :");Debug_FData(&T_Object);
DEBUGF("Data Object :"); Debug_FData(&G_Object);

// Read FS system data and print
 Mount_Qspi();
 Read_FData(&T_Object);
 DEBUGF("Read Object :");Debug_FData(&T_Object);
// Change data, and write to FS
 Mount_Qspi();
 G_Object.lifetime+=1.1;
  DEBUGF("Write Object :");Debug_FData(&G_Object);
 Save_FData(&G_Object);

// Read FS system data and print
 Mount_Qspi();
 Read_FData(&T_Object);
DEBUGF("Read Object :");Debug_FData(&T_Object);

}
