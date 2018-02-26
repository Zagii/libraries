
//#include <SoftwareSerial.h>
#include <EasyTransfer.h>

//create two objects
EasyTransfer ETin, ETout; 
const char* id="COM4";


//SoftwareSerial swSer(14, 12, false, 256);

struct RECEIVE_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  char msg[50];
  char topic[50];
  char typ;
};

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  char msg[50];
  char topic[50];
  char typ;
};

//give a name to the group of data
RECEIVE_DATA_STRUCTURE rxdata;
SEND_DATA_STRUCTURE txdata;



void setup(){
  Serial.begin(115200);
  Serial1.begin(115200);//19200);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
  ETin.begin(details(rxdata), &Serial1);
  ETout.begin(details(txdata), &Serial1);
  Serial.println("\nSetup Start\n");
}
unsigned long m=0;
void loop(){
  
  if(millis()-m>1000)
  {
	m=millis();
  sprintf(txdata.topic,"Reku/%s/info",id);
    txdata.typ='P';
   sprintf(txdata.msg,"%s, czas od restartu: %lu",id,m/1000);
   ETout.sendData();
  Serial.print(id);Serial.print(" wyslal:");Serial.println(txdata.msg);

  }
  if( ETin.receiveData())
  {
	Serial.print(id);Serial.print(" odebral:");Serial.print(rxdata.topic);Serial.print("#");Serial.print(rxdata.typ);Serial.print("#");Serial.println(rxdata.msg);
  }
    
   
  
}