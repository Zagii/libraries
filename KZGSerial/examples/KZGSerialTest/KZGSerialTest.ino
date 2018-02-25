#include "KZGSerial.h"

KZGSerial ser;

void setup()
{
	Serial.begin(115200);
	ser.begin(&Serial);
 ser.printRS('x',"test","string");
}

char b[200];

void loop()
{
	ser.loop();
	if(ser.isMsgWaiting())
	{
		sprintf(b,"Serial receive: type = %d / %c, topic= %s, msg= %s", ser.getMsgType(),ser.getMsgType(),ser.getMsgTopic(),ser.getMsgValue());
		Serial.println(b);
		ser.printRS('a',"someTopic","someMsg");
   ser.printRS('b',"abcd","abcdeFGHij123123aabb");
   ser.printRS('c',"Reku/temp/DS0x120x230x340x450x560x780x89","temperatura i 8 b znakow adresu");
   ser.printRS('d',"someTopic","someMsg");
    ser.resetData();
	}
}