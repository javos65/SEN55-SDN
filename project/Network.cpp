// ------------------- Wifi Network initiations and Time Keeper functions
//    
// Wifi routines - Server Routines - MQTT routines - NTP / RTC routines
//
#include "StoreData.h" // for credentials data store
#include <DEBUGF.h>
#include "Sensor.h"         // Sensor structure
#include "Network.h"        // Network / Wifi communications routin
#include <RTC.h>          
#include "WiFiC3.h"
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient
#include <NTPClient.h>      // https://github.com/arduino-libraries/NTPClient

#define SERVERIMAGE "<img src=\"http://192.168.200.6/web_images/LocaLSensor.png\">"

EECredentials G_MySecret;             // your secret credentials structure
//WiFiServer G_Myserver(80);
WiFiClient pubsubclient;
PubSubClient G_Mqttclient(pubsubclient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
RTCTime G_rtc;                  // Global used RealTime Clock handler

String G_P1payload="{\"Device\":\"PMSEN02\",\"Name\":\"Environment Sensor\",\"epochtime\":1608102499483,\"X\":10.0,\"Y\":5.0,\"id\":101,\"status\":128,\"message\":\"Sensor Idle\",\"Hunit\":\"ug\",\"Cunit\":\"Celcius\"}";  

byte Read_Secrets()
{
Mount_Qspi(); // mount secure file
if (!Read_CData(&G_MySecret))  { DEBUGF(" * Credentials not read !\n"); return(0); }  
else return(1);
}

/*********** Mqtt preparation  **********/
byte Mqtt_Initiate()
{
//G_MySecret.identity=EECREDENTIAL_ID; // your credentials ID  - see StoragePAss.ino
//if (!Read_CData(&G_MySecret))  { DEBUGF(" * Credentials not read !\n"); return(0); }
//DEBUGF(" * Credentials read \n");
G_Mqttclient.setServer(G_MySecret.mqttadr, 1883);
G_Mqttclient.setBufferSize(128);
return(1);
}

/****** create message string into Gobal G_P1payload from sensor object and time stamp ************/
void Mqtt_Makemessage(Sen55_Values *S,uint32_t R)
{
  G_P1payload="{\"Device\":\"PM2\",\"Name\":\"SEN55 Sensor\"";
  G_P1payload+= ",\"Time\":\"";  G_P1payload.concat( timeClient.getFormattedTime() );  //timeClient.getFormattedTime()
  G_P1payload+= "\",\"IP\":\"";  G_P1payload.concat( S->ip );
    G_P1payload+= "\",\"Lifetime\":";  G_P1payload.concat((float) S->lifetime);  
    G_P1payload+= ",\"PM10\":";  G_P1payload.concat((float) S->pm10); 
    G_P1payload+= ",\"PM4_0\":";  G_P1payload.concat((float) S->pm4_0);     
    G_P1payload+= ",\"PM2_5\":";  G_P1payload.concat((float) S->pm2_5); 
    G_P1payload+= ",\"PM1_0\":";  G_P1payload.concat((float) S->pm1_0);    
    G_P1payload+= ",\"PM10_1h\":";  G_P1payload.concat((float) S->pm10_1h);  
    G_P1payload+= ",\"PM10_24h\":";  G_P1payload.concat((float) S->pm10_24h);     
    G_P1payload+= ",\"PM2_5_1h\":";  G_P1payload.concat((float) S->pm2_5_1h);  
    G_P1payload+= ",\"PM2_5_24h\":";  G_P1payload.concat((float) S->pm2_5_24h);             
  G_P1payload+= ",\"Punit\":\"ug/m3\"";     
  G_P1payload+= ",\"Temp\":";  G_P1payload.concat((float) S->tmp);       
  G_P1payload+= ",\"Tunit\":\"Celsius\""; 
  G_P1payload+= ",\"Hum\":";  G_P1payload.concat((float) S->hum);       
  G_P1payload+= ",\"Hunit\":\"RH%\"";   
  G_P1payload+= ",\"VOCi\":";  G_P1payload.concat((float) S->voc);     
  G_P1payload+= ",\"NOXi\":";  G_P1payload.concat((float) S->nox);      
  G_P1payload+= ",\"VNunit\":\"%\"";    
  G_P1payload+= ",\"Timestamp\":";  G_P1payload.concat(R);      
  G_P1payload += ",\"valid\": 1,\"message\":\"ok\"}\0"; 
}


// Publish Message over MQTT, keep pubsub-client Global - otherwiasse running out of avaialbel sockets... PubSub bug ??
int Mqtt_Publishstructure()
{
  uint16_t i,z,l; 
  long timer;
  boolean P1_mqqtsend=false;
  boolean P1_mqqterror=false;
  //char *topic; topic=&(G_P1topic[0]); 
  char *payload; payload=&(G_P1payload[0]);
  l=G_P1payload.length();
  
  timer = millis();
//
  while (!P1_mqqtsend && !P1_mqqterror) 
  {
    // Attempt to connect
    char *cc = (char*) &G_P1payload[0]; // String type is fucking up DEBUGF / sprintf()
    if (G_Mqttclient.connect("JSONSensor01",G_MySecret.mqttlogin,G_MySecret.mqttpass) ) {
      DEBUGF(" * Sending message:%s\n * Mqtt Connected.. ", cc);    
      z=G_Mqttclient.beginPublish(G_MySecret.mqtttop,l,true); 
      for(i=0;i<=l;++i) { G_Mqttclient.print(payload[i]); }
      z=G_Mqttclient.endPublish();
      DEBUGF(".. published payload, Client status: %d, Return %d\n",G_Mqttclient.state(),z);
      break;
    } 
    else {
      if (millis()-timer > 5000) {P1_mqqterror=true;break;} // check max 5 seconds
      delay(100);
    } // end if- connected
  }  
if(P1_mqqterror==true)   
    { 
      DEBUGF(" * Connection to mqttserver failed, Client status %d",G_Mqttclient.state());   
      return(z);  
    }  
else return(0);    
}



byte StartWifi()
{
  int count=0, status = WL_IDLE_STATUS;
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
  DEBUGF(" * Attempting to connect to SSID: %s",G_MySecret.ssid); 
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(G_MySecret.ssid, G_MySecret.wifipass);
    delay(1000);
    count++;if(count>5) break;
  }
 if (WiFi.status() == WL_CONNECTED) { 
    DEBUGF(" * Wifi connected.\n");
    printWifiStatus();
    //G_Myserver.begin();
    //DEBUGF(" * Webserver starting\n");
    return(1);
    }
 else return(0); 
}

void printWifiStatus() {
  DEBUGF(" * SSID: %s",WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  DEBUGF(" / IP:%u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
  long rssi = WiFi.RSSI();
  DEBUGF(" * Signal strength (RSSI): %d dB\n",rssi); 
}


boolean addIP(struct Sen55_Values *S)
{
  IPAddress ip = WiFi.localIP();
  sprintf(S->ip,"%u.%u.%u.%u\0", ip[0], ip[1], ip[2], ip[3]);  
}

byte CheckWifi()
{
if ( WiFi.status() != WL_CONNECTED) return(0); 
else return(1);
}

/*
void SendHTMLHeader1(WiFiClient client){
     client.println(F("HTTP/1.1 200 OK")); 
     client.println(F("Content-type:text/html"));
     client.println();    
     client.println(F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")); // metaview          
     client.println(F("<p style=\"font-family:Helvetica;\">"));
     client.print(F("<body style=\"background-color:#"));
     client.print("A");client.print(random(0,15),HEX);
     client.print("A");client.print(random(0,15),HEX);
     client.print("A");client.print(random(0,15),HEX);
     client.println("\">"); 
}

// read 4 'point'-separated integer values from open client, format GET [command]wwww.xxxx.yyyy.zzzz -> read as from first x-value after GET comand.
// maps wwww-> h1 xxxx -> h2 yyyy -> h3 zzzz -> h4, separator can be :  [.] [/] [:] [,], usable for time, dates, ipadress etc etc.
void ReadPointSeparatedValues(WiFiClient client,long int *h1,long int *h2, long int *h3,long int *h4)
{
byte hr,t=1;
uint16_t h;
int signh=1;
*h1=0;*h2=0;*h3=0,*h4=0;
hr=(byte)client.read();                                               // readfirst character
while( (hr!=32) && (hr!=13) )                                         // Read till space (end of command) or CR character (end of line)
      { 
      if( hr =='.' || hr ==':' || hr==',' || hr=='/') { *h4=*h3;*h3=*h2;*h2=(*h1)*signh;*h1=0;signh=1;++t; }                     // at separator: new int valueto read: shift values to next pushed variable
      if( hr =='-') { signh=-1; }  
      if (hr>=48 && hr<=57) { *h1= (*h1)*10 + (hr-48); }              // process only ascii numbers 0-9
      hr=(byte)client.read(); //DEBUGF(hr);
      }    
 if  (t ==1)  *h1=(*h1)*signh;                                 // 1 ints read: value in h1 = ok, adapt sign
 else if  (t ==2)  {h=*h1;*h1=*h2;*h2=h;}                        // 2 ints read : swap value h1 and h2, h3 is kept 0, h4 is kept 0
 else if  (t ==3)  {h=*h1;*h1=*h3;*h3=h;}                        // 3 ints read : swap order 3->1, (2=2), 1->3
 else if  (t ==4)  {h=*h1;*h1=*h4;*h4=h;h=*h2;*h2=*h3;*h3=h;}    // 4 ints read : swap order : 4->1, 1->4, 2->3 3->2
 else {*h1=0;*h2=0;*h3=0;*h4=0; }                                // 5 or more ints read : failure, return 0's
}


//******** WEBSERVER ROUTINES ************
// Check server if client is there, and service requests 
byte CheckServerClients(Sen55_Values *S) {
  G_ledblink(2,250); // blink leds 2x
  // Local Variables
  String currentLine = "";  // date line input client
  int t=0, d = 0,v=0; // client data counter and loop counter, t is local loop counter
  byte hr,mn,sc,post=0; // hour and minute read characters
  long int h1,h2,h3,h4; // input read words
  int32_t R = Get_Timekeeper(); //RTC value
  
  WiFiClient client = G_Myserver.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
//
//    DEBUGF(" *New client*\n");           // print a message out the serial port
//
    currentLine = "";                       // make a String to hold incoming data from the client
    d = 0;
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
//
//        Serial.write(c);                     // print it out the serial monitor - debug only
//
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row. that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK) and a content-type so the client knows what's coming, then a blank line:
          SendHTMLHeader1(client);     
            client.print( "<p style=\"font-family:Candara; color:GhostWhite\"></font>");
            client.print( "<font size=6>Environment Sensor</font>&nbsp&nbsp&nbsp&nbsp<font size=3>Version ");client.print(VERSION);client.println("</font><br>");
            client.print( "<font size=2>");client.print(WiFi.SSID());client.println(" / ");client.print( WiFi.localIP());client.println("<br>");
            client.print("MQTT Server ");client.print(G_MySecret.mqttadr);client.println(" / ");client.print(G_MySecret.mqtttop);client.println("</font></p>");            
            client.print("<p style=\"font-family:Helvetica; color:#334455\"><font size=2>-------------------------------------------<br>");
            client.print("LifeTime : ");client.print(S->lifetime);client.print(" whatever.<br>");     
            client.print("Flash written: ");client.print(Count_DatastoreSensor());client.print(" times<br>");   
            client.print("Last Calibration: ");client.print(S->calibrated);client.print(" .<br>");              
            client.print("Time Client: ");client.print(timeClient.getFormattedTime());client.print(" <br>");
            client.print("<br>-------------------------------------------<br>");
            client.print("Click <a href=\"/H\">help</a> for Help.<br><br>");
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            DEBUGF(" * Web: Html Home-page send\n");

            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        if (currentLine.endsWith("GET /H")) {
          SendHTMLHeader1(client);     
          client.print( "<p style=\"font-family:Helvetica; color:GhostWhite\">&nbsp<font size=1>o</font><font size=2>o</font><font size=3>O</font><font size=2>O</font><font size=1>o</font><font size=2>O</font><font size=3>O</font><font size=2>o</font><font size=1>o</font> <br>");
          client.print( "<font size=5>Water Meter </font>&nbsp&nbsp&nbsp&nbsp<font size=4>Help Menu</font>");
          client.print("<p style=\"font-family:Helvetica; color:#334455\"><font size=2>-------------------------------------------<br>");

          client.print("Use HTML-command   http:\\\\"); client.print(WiFi.localIP()); client.print("/[cmd]<br><br>");
          client.print("[cmd] = JSON  Request JSON data<br>");
          client.print("[cmd] = R xyz ReSet, save, post Meter Data<br>");
          client.print("[cmd] = S     Save Meter Data<br>");
          client.print("[cmd] = F     Force NTP Time<br>");
          client.print("[cmd] = P     Publish Mqtt data<br>");
          client.print("[cmd] = T     Test Mode : send 250 Sensor values<br>");
          client.print("[cmd] = H     This Help<br>");
          client.print("<br>");
          client.print("Click <a href=\"/\">here</a> to return to menu.<br><br>");
          client.println();
          DEBUGF(" * Web: Help Send\n");
          break;
        }

        if (currentLine.endsWith("GET /JSON")) {
        
        client.println(F("HTTP/1.1 200 OK"));
        client.println(F("Content-type:text/html"));
        client.println();
        Mqtt_Makemessage(&G_Sensor,R);client.print(G_P1payload);
        client.println();
        DEBUGF(" * Web: HTTP JSON send\n");
        break;
        } 


        if (currentLine.endsWith("GET /R")) {
          ReadPointSeparatedValues(client,&h1,&h2,&h3,&h4);
          S->lifetime = (float) h1;                           // read new tick value
          Write_DatastoreSensor(R);                       // write store to Flash  
          post=1; // signal mqtt post back     
          SendHTMLHeader1(client);     
          client.print("New Meter data set to : "); client.print(h1); client.print("<br>"); 
          client.print("<br>Click <a href=\"/\">here</a> to return to menu.<br>");
          client.print("<meta http-equiv=\"refresh\" content=\"5;url=/\" />");
          client.println();
          DEBUGF(" *Meter Reset and saved, break out*\n");
          break;
        }

        if (currentLine.endsWith("GET /P")) {
          SendHTMLHeader1(client);     
          client.print("Meter data published. <br>"); 
          client.print("<br>Click <a href=\"/\">here</a> to return to menu.<br>");
          client.print("<meta http-equiv=\"refresh\" content=\"5;url=/\" />");
          client.println();
          post=1;
          DEBUGF(" * Meter Published,  break out*\n");
          break;
        }

        if (currentLine.endsWith("GET /S")) {
          Write_DatastoreSensor(R);                         // write store to Flash  
          SendHTMLHeader1(client);     
          client.print("Meter data saved : "); client.print(S->lifetime); client.print("<br>"); 
          client.print("<br>Click <a href=\"/\">here</a> to return to menu.<br>");
          client.print("<meta http-equiv=\"refresh\" content=\"5;url=/\" />");
          client.println();
          DEBUGF(" * Meter Saved,  break out*\n");
          break;
        }

        if (currentLine.endsWith("GET /F")) {
          Update_Timekeeper();
          SendHTMLHeader1(client);     
          client.print("NTP time epoch : "); client.print(timeClient.getFormattedTime() ); client.print("<br>"); 
          client.print("<br>Click <a href=\"/\">here</a> to return to menu.<br>");
          client.print("<meta http-equiv=\"refresh\" content=\"5;url=/\" />");
          client.println();
          DEBUGF(" * NTP time set,  break out*\n");
          break;
        }
        
      }
      else {
        d++;
        if (d > MAX_MISSED_DATA) { // defined in arduino_secrets.h
          DEBUGF(" * Client missed-data time-out*\n");
          break;   // time-out to prevent to ever waiting for misssed non-send data newclient //
        }
      }
    }
    // close the connection:
    client.stop();
    
//
//    DEBUGF(" *Client disonnected*\n");
//
  } // end ifloop (client)
  else
  {
    client.stop();
  } 
return(post);  
}
*/


// Time keeper initialisation : setup RTC, setup NTP-server , load time
uint32_t Start_Timekeeper()
{
  RTC.begin();
  DEBUGF(" * Starting connection to NTP server\n");
  timeClient.begin();
  timeClient.update();

  // Get the current date and time from an NTP server and convert
  // it to UTC +2 by passing the time zone offset in hours.
  // You may change the time zone offset to your local one.
   uint32_t unixTime = timeClient.getEpochTime() + 3600;
  G_rtc = RTCTime(unixTime);
  RTC.setTime(G_rtc);
  DEBUGF(" * RTC was just set to NTP: %d\n",G_rtc.getUnixTime());
return( G_rtc.getUnixTime());
}


// Time keeper Update : NTP-server , load time
uint32_t Update_Timekeeper()
{
timeClient.update();
uint32_t unixTime = timeClient.getEpochTime() +  3600;
G_rtc = RTCTime(unixTime);
RTC.setTime(G_rtc);
DEBUGF(" * RTC was just updated to: %d\n",G_rtc.getUnixTime());
return( G_rtc.getUnixTime() );
}

uint32_t Get_Timekeeper()
{
G_rtc = RTCTime(timeClient.getEpochTime());
return( G_rtc.getUnixTime());
}

uint8_t Check_Time(int h,int m)
{
G_rtc = RTCTime(timeClient.getEpochTime());
if( (G_rtc.getHour()==h) && (G_rtc.getMinutes()==m))   return(1);
else return(0);
}