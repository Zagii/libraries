/*
Library for Serial
*/
#ifndef KZGSerial_h
#define KZGSerial_h

#define KZGSerial_DEBUGFLAG

/// MSG pattern
// <abc;T;topic;msg>crc
// < -begin msg
//a,b,c,d,e,f,g [0..9]
//abc - all msg length
// > -end of msg
// crc8 chceck of all msg <..>
///// MAX_TOPIC_LENGTH + MAX_MSG_LENGTH < 250  !!
#define MAX_TOPIC_LENGTH 20
#define MAX_MSG_LENGTH 50

#define KZGSerialMODE_PRZED		0
#define KZGSerialMODE_START		1
#define KZGSerialMODE_TYPE	 	2
#define KZGSerialMODE_TOPIC		3
#define KZGSerialMODE_TRESC		4
#define KZGSerialMODE_KONIEC	5
#define KZGSerialMODE_CRC		6

class KZGSerial 
{
	private:
		
		void deb(char *);
		uint8_t _mode;
		Stream *_serial;
		uint8_t _index;
		uint8_t _allLen;
		char _type;
		char _topic[MAX_TOPIC_LENGTH];
		char _msg[MAX_MSG_LENGTH];
		bool _isMsgWaiting;
		void resetData();
		bool checkCRC(int crc);
		uint8_t calcCRC();
		uint8_t crc8(const uint8_t *addr, uint8_t len);
		void createMsg(char* buf,char t, char* top,char* msg);
	public:
		KZGSerial();
		void begin(Stream *theStream);
		void loop();
		void printRS(char type, char* topic, char* msg);
		bool isMsgWaiting();
		char getMsgType(); 
		char* getMsgTopic();
		char* getMsgValue();
		
};

#endif;