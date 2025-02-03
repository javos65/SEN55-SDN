// ------------------- Wifi Network initiations and Time keeping functions.
//    
// Wifi routines - Server Routines - MQTT routines
//
// use Credetnials routines 
//
 
#ifndef __NETWORK_H
#define __NETWORK_H

#define MAX_MISSED_DATA 2000        // MAX data missed from Client/Web HTTP reply before time-out (accept short messages only)
#define VERSION "2.1.0"            // Software verson nr


extern String G_P1payload;
extern byte Mqtt_Initiate();
extern void Mqtt_Makemessage(Sen55_Values *S,long unsigned int R);
extern int Mqtt_Publishstructure();
extern byte StartWifi();
extern byte CheckWifi();
extern void printWifiStatus();
extern byte CheckServerClients(Sen55_Values *S);
extern uint32_t Start_Timekeeper();
extern uint32_t Get_Timekeeper();
extern uint32_t Update_Timekeeper();
extern uint8_t Check_Time(int h,int m);
extern byte Read_Secrets();

extern boolean addIP(struct Sen55_Values *S);

#endif
