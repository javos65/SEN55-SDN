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
#ifndef __DEBUGF_H
#define __DEBUGF_H

#include <stdio.h>
#include <stdint.h>
#include <Arduino.h>


#define DEBUGF_ 1 // 1 = DEBUGF inline;  0 = no DEBUGF inline

extern char _SERIALBUF[256]; // used 128 bytes Memory
extern void Debugbuffer_8H(uint8_t* b,int size,int step);
extern void Debugbuffer_16H(uint16_t* b,int size,int step);
extern void Debugbuffer_32H(uint32_t* b,int size,int step);
extern void Debugbuffer_8B(uint8_t* b,int size,int step);
extern void DEBUGFinit(uint32_t speed);

#if DEBUGF_
#define _MYSERIALPORT Serial
#define DEBUGF(__info,...)       {_SERIALBUF[0]=0;sprintf(_SERIALBUF,__info,##__VA_ARGS__);_MYSERIALPORT.print(_SERIALBUF);} // Debug Test
#define DEBUGBUFFER_32H(b,l,sh)  {Debugbuffer_32H(b,l,sh);}
#define DEBUGBUFFER_16H(b,l,sh)  {Debugbuffer_16H(b,l,sh);}
#define DEBUGBUFFER_8H(b,l,sh)   {Debugbuffer_8H(b,l,sh);}
#define DEBUGBUFFER_8B(b,l,sh)   {Debugbuffer_8B(b,l,sh);}

#else
#define DEBUGF(__info,...)    ;;
#define DEBUGBUFFER_32H(...)  ;;
#define DEBUGBUFFER_16H(...)  ;;
#define DEBUGBUFFER_8H(...)  ;;
#define DEBUGBUFFER_8B(...)  ;;
#endif


#endif
