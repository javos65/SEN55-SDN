
/*****************************************************************************
* | File      	:	Debugf.h
* | Function    :	Debug with C-style printf functionality
* | Info        : JV 2024
*----------------
* |	This version:   V1.0
* | Date        :   2024-01-16
* | Info        :   Basic version
*
******************************************************************************/

#include "DEBUGF.h"

#if DEBUGF_

char _SERIALBUF[256]; // used 128 bytes Memory

void DEBUGFinit(uint32_t baudrate)
{
 if (!_MYSERIALPORT) _MYSERIALPORT.begin(baudrate);          // start your debug serial port         
}

// Debug function : print buffer in hex and char format //
void Debugbuffer_8H(uint8_t* b,int size,int step)
{
int t,u;
DEBUGF(" \n\r")
for( t=0 ; t< size ; t=t+step)
    {
    DEBUGF("[%0.8x] ",(uint32_t) ((uint8_t*) b+t) );  
    for(u=0;u<step;++u) { DEBUGF("%0.2x_",b[t+u]); }
    DEBUGF("["); 
    for(u=0;u<step;++u) { 
      if(b[t+u]<32 || b[t+u]>127 ){DEBUGF(".");}
      else {DEBUGF("%c",(char) b[t+u]);}
    }
    DEBUGF("]\n\r");
    }
DEBUGF(" \n\r")
}

// Debug function : print buffer in hex and char format //
void Debugbuffer_8B(uint8_t* b,int size, int step)
{
int t,u,v;
DEBUGF(" \n\r")
for( t=0 ; t< size ; t=t+step)
    {
    DEBUGF("[%0.8x] ",(uint32_t) ((uint8_t*) b+t) );  
    for(u=0;u<step;++u) {
      for(v=0;v<8;v++) {DEBUGF("%x",(b[t+u]>>(7-v))&0x01);}
      DEBUGF("_");   
    }
    DEBUGF("["); 
    for(u=0;u<step;++u) { 
      if(b[t+u]<32 || b[t+u]>127 ){DEBUGF(".");}
      else {DEBUGF("%c",(char) b[t+u]);}
    }
    DEBUGF("]\n\r");
    }
DEBUGF(" \n\r")
}

// Debug function : print buffer in hex and char format, beware: 32bit numbers are shown Litte Endian style - byte wise reversed vs address//
void Debugbuffer_32H(uint32_t* b,int size, int step)
{
int t,u,v;
char c;
DEBUGF(" \n\r")
for( t=0 ; t< size ; t=t+step)
    {
    DEBUGF("[%0.8x] ",(uint32_t) ((uint32_t*) b+t) );  // print address in 32 bit step size
    for(u=0;u<step;++u) {
    DEBUGF("%0.8x_",b[t+u]);
    }
    DEBUGF("["); 
    for(u=0;u<step;++u) { 
        for(v=0;v<4;++v){
            c= (char) ((b[t+u]>>((v)*8))&0x000000ff);
            if( c<32 || c>127 ){DEBUGF(".");}
            else {DEBUGF("%c",c);}
            }
        DEBUGF("_");    
    }
    DEBUGF("]\n\r");
    }
DEBUGF(" \n\r")
}


// Debug function : print buffer in hex and char format beware: 16bit numbers are shown Litte Endian - byte wise reversed vs address//
void Debugbuffer_16H(uint16_t* b,int size, int step)
{
int t,u,v;
char c;
DEBUGF(" \n\r")
for( t=0 ; t< size ; t=t+step)
    {
    DEBUGF("[%0.8x] ",(uint32_t) ((uint16_t*) b+t) );   // print adress in 16 bit step size
    for(u=0;u<step;++u) {
    DEBUGF("%0.4x_",b[t+u]);
    }
    DEBUGF("["); 
    for(u=0;u<step;++u) { 
        for(v=0;v<2;++v){
            c= (char) ((b[t+u]>>((v)*8))&0x00ff);
            if( c<32 || c>127 ){DEBUGF(".");}
            else {DEBUGF("%c",c);}
            }
        DEBUGF("_");    
    }
    DEBUGF("]\n\r");
    }
DEBUGF(" \n\r")
}

#endif