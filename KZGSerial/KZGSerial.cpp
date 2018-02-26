/*
Library for Serial
*/

#include "Arduino.h"
#include "KZGSerial.h"

void KZGSerial::deb(char* str)
{
	#ifdef KZGSerial_DEBUGFLAG
		Serial.println(str);
	#endif
}
void KZGSerial::deb(char c)
{
	#ifdef KZGSerial_DEBUGFLAG
		Serial.println(c);
	#endif
}
void KZGSerial::deb(int c)
{
	#ifdef KZGSerial_DEBUGFLAG
		Serial.println(c);
	#endif
}

KZGSerial::KZGSerial()
{
	
}
void KZGSerial::begin(Stream *theStream)
{
	_serial=theStream;
	resetData();
}
void KZGSerial::resetData()
{
	_type='\0';
	strcpy(_topic,"");
	strcpy(_msg,"");
	_index=0;
	_allLen=0;
	_crcint=0;
	_isMsgWaiting=false;
	_mode=KZGSerialMODE_PRZED;
	deb("resetData");
}
void KZGSerial::loop()
{
	if(_serial->available()>0)
	{
		char c=_serial->read();
//Serial.print("_index=");Serial.print(_index);
	//	Serial.print(", nowy znak=");Serial.println(c);
		if(c=='<') 
		{
			resetData();
			_mode=KZGSerialMODE_START;
			deb("Loop - Bledny znak poczatku");
			
			return;
		}
		_index++;
		if(_index>MAX_TOPIC_LENGTH + MAX_MSG_LENGTH+2) //plus 2 bo zakończenie > oraz crc
		{
			resetData();
			deb("Loop - za długi komunikat");
			return;
		}
		switch(_mode)
		{
			case KZGSerialMODE_PRZED:
				deb("Loop - KZGSerialMODE_PRZED nieistotne znaki");
				deb(c);
			break;
			case KZGSerialMODE_START:
				if(c==';')
				{
					_mode=KZGSerialMODE_TYPE;
				}
				else
				{
					deb("modeStart");
					if(!isDigit(c)||_index>3)
					{
						resetData();
						deb("Loop - KZGSerialMODE_START - blad dlugosci komunikatu");
						return;
					}else
					{
						uint8_t n=(int)c-'0';
						_allLen=_allLen*10+n;
						deb("znak len, _allLen=");
						deb(_allLen);
					}
				}
			break;
			case KZGSerialMODE_TYPE:			
				if(c==';'&&_type!='\0')
				{
					_mode=KZGSerialMODE_TOPIC;
					deb("topicMODE");
				}
				else
				{
					if(!isPrintable(c)||_index>5||_index>_allLen)
					{
						deb("Loop - KZGSerialMODE_TYPE - błąd odczytu typu");
						resetData();
						return;
					}else
					{
						_type=c;
						deb("_type");
						deb(_type);
					}
				}
			break;
			case KZGSerialMODE_TOPIC:
				if(c==';')
				{
					_mode=KZGSerialMODE_TRESC;
					deb("_topic=");deb(_topic);
					deb("trescMode");
				}
				else
				{
					if(!isPrintable(c)||_index>_allLen)
					{
						deb("Loop - KZGSerialMODE_TOPIC - blad odczytu topicu");
						
						deb("index, allLen");
						deb(_index);
						deb(_allLen);
						resetData();
						return;
					}else
					{
						int len = strlen(_topic);
						_topic[len] = c;
						_topic[len+1] = '\0';
					}
				}
			break;
			case KZGSerialMODE_TRESC:
				
					if(!isPrintable(c)||_index>_allLen)
					{
						deb("Loop - KZGSerialMODE_TRESC - blad odczytu treści");
						
						deb("index, allLen");
						deb(_index);
						deb(_allLen);
						resetData();
						return;
					}else
					{
						int len = strlen(_msg);
						_msg[len] = c;
						_msg[len+1] = '\0';
						if(_index==_allLen)
						{		
							_mode=KZGSerialMODE_KONIEC;
							deb("_msg=");deb(_msg);
							deb("koniecMode");
						}
					}
				
			break;
			case KZGSerialMODE_KONIEC:
				if(c=='>'&&_index==_allLen+1)
				{
					_mode=KZGSerialMODE_CRC;
				}
				else
				{
					deb("Loop - KZGSerialMODE_KONIEC - blad konca komunikatu");
					resetData();
				}
			break;
			case KZGSerialMODE_CRC:
				if(_index-_allLen<5)
				{
						uint8_t n=(int)c-'0';
						_crcint=_crcint*10+n;
						deb("CRC ");
						deb(_crcint);
						if(_index-_allLen==4)
						{
							if(checkCRC(_crcint))
							{
								_isMsgWaiting=true;
							}
							else
							{
								deb("Loop - KZGSerialMODE_CRC - blad CRC");
							}
						_mode=KZGSerialMODE_PRZED;
						}
				}
				
				
			break;
		}
	}
}

bool KZGSerial::checkCRC(char crc)
{
	deb("checkCRC ");
	char c1=calcCRC();
	deb(c1);deb(crc);
	if(c1==crc) return true; else return false;
}
bool KZGSerial::checkCRC(uint8_t crc)
{
	deb("checkCRC ");
	uint8_t c1=(int)calcCRC();
	deb(c1);deb(crc);
	if(c1==crc) return true; else return false;
}
void KZGSerial::printRS(char type, char* topic, char* msg)
{
	char str[MAX_TOPIC_LENGTH + MAX_MSG_LENGTH];
	createMsg(str,type,topic,msg);
	uint8_t crc= (uint8_t) crc8(str,strlen(str));
sprintf(str,"%s%03d",str,crc);	
	//strcat(str,(const char *)crc);
	deb("printRS");
	deb(str);
	_serial->println(str);
}
bool KZGSerial::isMsgWaiting()
{
	return _isMsgWaiting;
}
char KZGSerial::getMsgType()
{
	return _type;
}
char* KZGSerial::getMsgTopic()
{
	return _topic;
}
char* KZGSerial::getMsgValue()
{
	return _msg;
}
void KZGSerial::createMsg(char* buf,char t, char* top,char* msg)
{
	
	uint8_t n=strlen(top)+strlen(msg)+3; //ddd;+;+;+;
	char tmp[5];
	sprintf(buf,";%c;%s;%s",t,top,msg);
	n=strlen(buf);
	int d=2;
	if(n>=100)d=3;else if(n<10)d=1; 
	sprintf(buf,"<%d;%c;%s;%s>",n+d,t,top,msg);
	deb("createMsg");
	deb(buf);
}
char KZGSerial::calcCRC()
{
	char str[MAX_TOPIC_LENGTH + MAX_MSG_LENGTH];
	createMsg(str,_type,_topic,_msg);
	char crc= crc8(str,strlen(str));	   
	deb("calcCRC ");deb(str);deb(crc);deb("crc int=");deb((int)crc);
	return crc;
}

// This table comes from Dallas sample code where it is freely reusable,
// though Copyright (C) 2000 Dallas Semiconductor Corporation
static const char PROGMEM dscrc_table[] = {
      0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
     35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
     70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
     17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
     50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
     87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

//
// Compute a Dallas Semiconductor 8 bit CRC. These show up in the ROM
// and the registers.  (note: this might better be done without to
// table, it would probably be smaller and certainly fast enough
// compared to all those delayMicrosecond() calls.  But I got
// confused, so I use this table from the examples.)
//
char KZGSerial::crc8(const char *addr, uint8_t len)
{
	char crc = 0;

	while (len--) {
		crc = pgm_read_byte(dscrc_table + (crc ^ *addr++));
	}
	return crc;
}