#include <SoftwareSerial.h>
#define RX 10
#define TX 11
SoftwareSerial esp8266(RX,TX);
String AP = "SSID of Network";     
String PASS = "Password"; 
String API = "7BQBAVCZMRXV79G3";//Retrieved from ThingSpeak.com  
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
  
void setup() 
{
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT+CIFSR",10,"OK");
  //esp8266.println("AT+CIFSR");
  sendCommand("AT",5,"OK");//This will check if the module is connected properly and its functioning, the module will reply with an acknowledgment.
  sendCommand("AT+CWMODE=1",5,"OK");//This sets the Wifi mode. It should be always set to Mode 1 if the module is going to be used as a node (Like our mobile’s connection to the access points)
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");// To disconnect from any access point
}
void loop() 
{
  int sensorValue = analogRead(A0);//Read data from A0 pin
  float valSensor = sensorValue * (5.0 / 1023.0);// Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
  sendCommand("AT+CIPMUX=1",5,"OK");//This AT Command is used to enable multiple TCP Connections."1" stands for multiple connections.
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");//This AT Command is used to establish TCP connections.
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),10,">");//Set date length which will be sent.
  esp8266.println(getData);
  //delay(1500);
  countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",1,"OK");//Close the TCP Connection.
}
int getSensorData()
{
  return random(1000); // Replace with Sensor Data
}
void sendCommand(String command, int maxTime, char readReplay[])
{
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//AT+CIPMUX/CIPSTART
    if(esp8266.find(readReplay))//"OK"
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    if(command=="AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"")      
    {
      digitalWrite(7, HIGH);  
      digitalWrite(6, LOW);
    }
    Serial.println("CONNECTED");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
        if(command=="AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"")//If unable to connect to internet, then disconnect from SSID
    {
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
    }
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
