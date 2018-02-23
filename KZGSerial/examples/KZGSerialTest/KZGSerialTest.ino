#include "KZGSerial.h"

KZGSerial ser;

setup()
{
	Serial.begin(115200);
	ser.begin(&Serial);
}

char b[200];

loop()
{
	ser.loop();
	if(ser.isMsgWaiting())
	{
		sprintf(b,"Serial receive: type = %d, topic= %s, msg= %s", ser.getMsgType(),ser.getMsgTopic(),ser.getMsgValue()");
		Serial.println(b);
		ser.printRS(3,"someTopic","someMsg");
	}
}