
#include <SoftwareSerial.h>
#include <EasyTransfer.h>

//create two objects
EasyTransfer ETin, ETout; 
const char* id="ALFA";


SoftwareSerial swSer(14, 12, false, 256);

struct RECEIVE_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  char msg[50];
  
};

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  char msg[50];
};

//give a name to the group of data
RECEIVE_DATA_STRUCTURE rxdata;
SEND_DATA_STRUCTURE txdata;



void setup(){
  Serial.begin(115200);
  swSer.begin(115200);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
  ETin.begin(details(rxdata), &swSer);
  ETout.begin(details(txdata), &swSer);
  Serial.println("Setup Start");
}
unsigned long m=0;
void loop(){
  
  if(millis()-m>10000)
  {
	m=millis();
   sprintf(txdata.msg,"%s, czas od restartu: %lu",id,m);
   ETout.sendData();
  }
  if( ETin.receiveData())
  {
	Serial.print(id);Serial.print(" odebral:");Serial.println(rxdata.msg);
  }
    
   
  
}