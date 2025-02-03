/*****************************************************************************
* | File      	:	StoreData.cpp
* | Function    :	Storing data on QSPI for C33 on LittleFileSystem
* | Info        : JV 2024
*----------------
* |	This version:   V1.0
* | Date        :   2024-01-16
* | Info        :   Basic version
*
******************************************************************************/
#include <DEBUGF.h>
#include "StoreData.h"

#include <vector>
#include <string>
#include "QSPIFlashBlockDevice.h"
#include <MBRBlockDevice.h>
#include "LittleFileSystem.h"


//File system structures
  BlockDevice* block_device = BlockDevice::get_default_instance();
  MBRBlockDevice mbr(block_device, 2);
  LittleFileSystem fs(FS_NAME);
  FILE* fp;
  DIR *dir;
// Folder and file name strings
  std::string root_folder       = std::string("/") + std::string(FS_NAME);
  std::string folder_test_name  = root_folder + std::string("/") + std::string(FOLDER_NAME);
  std::string file_test_name    = folder_test_name + std::string("/") + std::string(FILEHS);  // Data File
  std::string Cfile_test_name    = folder_test_name + std::string("/") + std::string(CFILEHS);  // Credential file
  std::vector<std::string> dir_list;
// Global Variable
  int err;
  struct dirent *ent;
  int dirIndex = 0;
  int numbytes;
  
// Mount QSPI and read directiry to check file structure /qspi/C33/sensorstore.txt
int Mount_Qspi(){
        DEBUGF(" * Mounting QSPI FLASH...\n");
        err =  fs.mount(&mbr);
        if (err) {
          err = fs.reformat(&mbr);
          DEBUGF(" *! No filesystem found, formatting error#0x%x",err);
        }
      if (err) {
        DEBUGF(" *! Error mounting File system, error#0x%x\n",err);
        while(1);
        }

  // Run Through Root Directory
  if ((dir = opendir(root_folder.c_str())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      if(ent->d_type == DT_REG) ; //Debug("- [File]: ");
      else if(ent->d_type == DT_DIR) {
        //Debug("- [Fold]: ");
        if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..")) {
          dir_list.push_back(ent->d_name);
        }
      }
      //DEBUGF(ent->d_name);
      dirIndex++;
    }
    closedir (dir); // moved th
  } 
  else {
    // Could not open directory
    DEBUGF(" *! Error opening Qspi\n");
    while(1);
  }
  if(dirIndex == 0) {
    DEBUGF(" *! Empty Qspi\n");
  }

  // Check if our directory is there, if not , create it.
  bool found_test_folder = false;
  for(unsigned int i = 0; i < dir_list.size(); i++) { // find your subdirectory
    if(dir_list[i] == FOLDER_NAME) found_test_folder = true;
  }
      err = 0;
      if(!found_test_folder) {
        DEBUGF(" * FOLDER NOT FOUND... creating folder\n"); 
        err = mkdir(folder_test_name.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        if(err != 0) {
            DEBUGF(" *! FAILED folder creation with error#0x%x\n",err);
            }
      }
  return(err);

}


/*

STORAGE DATA FUNCTIONS

*/

// delete Fata file
int Delete_DFile()
{
      if(remove(file_test_name.c_str()) == 0) {
        DEBUGF(" *!DATA-FILE HAS BEEN DELETED!\n");
        return(1);
      }
    else return(0);
}



int Check_DFilesize()
{
    int size; 
    fp = fopen(file_test_name.c_str(), "r");
    if(fp != NULL) {
      fseek(fp, 0L, SEEK_END);
      size = ftell(fp);
      fseek(fp, 0L, SEEK_SET);  
      return size;
    }
    else return(0);
}

/* Read Data and verify ID */
uint8_t Read_FData(struct EEData *O)
{
  fp = fopen(file_test_name.c_str(), "r");
  if(fp != NULL) {
    EEData t; // dummy structure to test-read
    int s, u;
    uint8_t b;
    uint8_t* ptr = (uint8_t*)&t;       // make pointer to structure t
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);  
    DEBUGF(" * Found file :size %d\n",size);
    for( u=0 ; u<sizeof(EEData);++u){ fread(&b, 1 , 1, fp); (*(ptr+u))=b;} // read structure raw into t -> DO NOT USE BUFFER WRITE, cant handle non-characters !!!
    //Debug(" * Raw Read");Debug_FData(&t); // raw read
    O->counter = t.counter; O->identity=t.identity;
    O->lifetime=t.lifetime;O->time = t.time;
    DEBUGF(" * Data read from File system, with ID 0x%x\n",t.identity);
    fclose(fp); 
    return(1);
  } // check file        
  else { 
    DEBUGF(" * File Read Failed : No File system\n");
    fclose(fp); 
    return(0);
    }         
}


// verify ID, verify FFS, Safe data, increase counter*/
uint8_t Save_FData(struct EEData *O)
{
     uint8_t b,zero=0;
  Delete_DFile(); // Delete Data file
  fp = fopen(file_test_name.c_str(), "w");
  if(fp != NULL) {
     O->counter++;
     int u,c = O->counter;          DEBUGF(" * received counter +1 %d\n",c);
     EEData t;                 // dummy structure to test-read
     uint8_t* ptr = (uint8_t*)&t;       // make pointer to structure t
         //Debug_FData(O);
         O->counter=c; t.counter = c; t.identity=EEData_ID;
         t.lifetime = O->lifetime; t.time = O->time;
         //DEBUGF(" * Save: Encrypted data %x",&t);  
         for(u=0; u<sizeof(EEData);++u){ b=*(ptr+u); fwrite(&b, 1, 1, fp); } // write struct to file  -> DO NOT USE BUFFER WRITE, cant handle non-characters !!!
         fclose(fp); 
         DEBUGF(" * Saved Object to File system with ID %d, size %d,filesize %d",t.identity,sizeof(EEData),Check_DFilesize());
         return(1);
  }
  else DEBUGF(" *! File not open for writing\n");
 }


/* Print debug information of Data structure,*/
uint8_t Debug_FData(struct EEData *O)
{
    DEBUGF(" * Data:[%f],t=[%d],id=[0x%x],c=[%d],s=[%d]\n", O->lifetime,O->time,O->identity,O->counter,O->stop);
     return(1);
}



/*

CREDENTIAL DATA FUNCTIONS

*/

// Delete Credential file
int Delete_CFile()
{
      if(remove(Cfile_test_name.c_str()) == 0) {
        DEBUGF(" *!CRED-FILE HAS BEEN DELETED!\n");
        return(1);
      }
    else return(0);
}

// Check Credential Filesize
int Check_CFilesize()
{
    int size; 
    fp = fopen(Cfile_test_name.c_str(), "r");
    if(fp != NULL) {
      fseek(fp, 0L, SEEK_END);
      size = ftell(fp);
      fseek(fp, 0L, SEEK_SET);  
      return size;
    }
    else return(0);
}

/* Read Credential Data and verify ID */
uint8_t Read_CData(struct EECredentials *O)
{
  fp = fopen(Cfile_test_name.c_str(), "r");
  if(fp != NULL) {
    EECredentials t; // dummy structure to test-read
    int s, u;
    uint8_t b;
    uint8_t* ptr = (uint8_t*)&t;       // make pointer to structure t
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);  
    DEBUGF(" * Found file :size %d\n",size);
    for( u=0 ; u<sizeof(EECredentials);++u){ fread(&b, 1 , 1, fp); (*(ptr+u))=b;} // read structure raw into t -> DO NOT USE BUFFER WRITE, cant handle non-characters !!!
    //DEBUGF(" * Raw Read");Debug_CData(&t); // raw read
    O->counter = t.counter; O->identity=t.identity;O->stop=t.stop;
            SimpleDecypher(t.ssid,O->ssid); SimpleDecypher(t.wifipass,O->wifipass);
            SimpleDecypher(t.login1,O->login1); SimpleDecypher(t.pass1,O->pass1);
            SimpleDecypher(t.mqttadr,O->mqttadr); SimpleDecypher(t.mqtttop,O->mqtttop);
            SimpleDecypher(t.mqttlogin,O->mqttlogin);SimpleDecypher(t.mqttpass,O->mqttpass);
    DEBUGF(" * Credential Data read from File system, with ID 0x%x\n",t.identity);
    fclose(fp); 
    return(1);
  } // check file        
  else { 
    DEBUGF(" * Credential File Read Failed : No File system\n");
    fclose(fp); 
    return(0);
    }         
}


// verify ID, verify FFS, Safe data, increase counter*/
uint8_t Save_CData(struct EECredentials *O)
{
     uint8_t b,zero=0;
  Delete_CFile();
  fp = fopen(Cfile_test_name.c_str(), "w");
  if(fp != NULL) {
     O->counter++;
     int u,c = O->counter;          DEBUGF(" * received counter +1 %d\n",c);
     EECredentials t;                 // dummy structure to test-read
     uint8_t* ptr = (uint8_t*)&t;       // make pointer to structure t
         //Debug_CData(O);
         O->counter=c; t.counter = c; t.identity=EECREDENTIAL_ID; t.stop=  O->stop;
         SimpleCypher(O->ssid,t.ssid); SimpleCypher(O->wifipass,t.wifipass);
         SimpleCypher(O->login1,t.login1); SimpleCypher(O->pass1,t.pass1);
         SimpleCypher(O->mqtttop,t.mqtttop); SimpleCypher(O->mqttadr,t.mqttadr); 
         SimpleCypher(O->mqttlogin,t.mqttlogin);SimpleCypher(O->mqttpass,t.mqttpass);          
         //DEBUGF(" * Save: Encrypted data %x",&t);  
         for(u=0; u<sizeof(EECredentials);++u){ b=*(ptr+u); fwrite(&b, 1, 1, fp); } // write struct to file  -> DO NOT USE BUFFER WRITE, cant handle non-characters !!!
         fclose(fp); 
         DEBUGF(" * Saved Credentials to File system with ID %d, size %d,filesize %d",t.identity,sizeof(EECredentials),Check_CFilesize());
         return(1);
  }
  else DEBUGF(" *! Credential File not open for writing\n");
 }

/* Print debug information of Credentaisl structure,*/
uint8_t Debug_CData(struct EECredentials *O)
{
     DEBUGF(" * Credentials Wifi:1[%s],[%s] 2[%s],[%s] -",O->ssid,O->wifipass,O->login1,O->pass1);
     DEBUGF(" Mqtt:3[%s],[%s] 4[%s],[%s] -",O->mqttadr,O->mqtttop,O->mqttlogin,O->mqttpass);
     DEBUGF(" Others:I[0x%x], C[%d] S[%d]\n",O->identity,O->counter,O->stop);
     return(1);
}


/* Simple Cyphering the text code */
byte SimpleCypher(char * textin, char * textout)
{
int t=0;
bool st=true;
while(textin[t]!=0) {
   //textout[t]=textin[t]; // testing only
   textout[t]=textin[t]+EE_SEED%(111)-t%17;
   t++; if( t>=EE_MAXCHAR){DEBUGF(" * Cypher error: data size out of range") ;st=false;break;}
  }
  textout[t]=0;
  if (st==true){
  //DEBUGF(" * Decyphered ");DEBUGF(t);DEBUGF(" - ");Debugln(textout);
  return(1);
  }
  else return(0);
}

/* Simple DeCyphering the text code */
byte SimpleDecypher(char * textin, char * textout)
{
int t=0;
bool st=true;
while(textin[t]!=0) {
   //textout[t]=textin[t]; / testing only
   textout[t]=textin[t]-EE_SEED%(111)+t%17;
   t++; if( t>=EE_MAXCHAR){DEBUGF(" * Decypher error : data too long") ;st=false;break;}
  }
  textout[t]=0;
  if (st==true){
  //DEBUGF(" * Decyphered ");DEBUGF(t);DEBUGF(" - ");Debugln(textout);
  return(1);
  }
  else return(0);
}
 

 
