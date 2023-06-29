
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>





SoftwareSerial mySerial(5, 6); // RX, TX

void setup() {
 
 Mirf.csnPin = 9;
    Mirf.cePin = 10;
  Mirf.spi = &MirfHardwareSpi;     // Wykorzystanie SPI do komunikacji pryz użyciu modułów nRF24L01
  Mirf.init();                     // Inicjalizacja modułu MIRF
  Mirf.setTADDR((byte *)"67890");  // ustawienie adresu do nadawania
  Mirf.setRADDR((byte *)"12345"); 
 Mirf.payload = sizeof(char[10]);
  // Maksymalny przesył przez moduły radiowe ustawiony na wielkośc zmiennej FLOAT (4 bajty)
  Mirf.config();    





  
  // Open serial communications and wait for port to open:
  Serial.begin(9200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("to jest nadajnik");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.println("Hello, world?");
}

String getData(char* napis,bool znak){
String napstr=String(napis);
String str;
 String result;
 
if (znak==0){
  str=napstr.substring(0,4);
  for(int a=0;a<str.length();a++) {
  if (str[a]!='$'){
      result = result+str[a];
  }
}
   }
   
if (znak==1){//zabieram $ w stringu
  str=napstr.substring(4,8);
    for(int a=0;a<str.length();a++) {
         if (str[a]!='$'){
         result = result+str[a];
  }
}
String res;

    for(int a=0;a<result.length();a++) {
      
         if (result[a]=='F'){
                 
                  res="-"+res;
         }
         else{
           res=res+result[a];
          
          }
         
  }
  
  result=res;
}

 return result;
}










void loop() { // run over and over









char napisS[8]; //odbiór radiowy
  if(Mirf.dataReady())  // Warunek wykonywany tylko, gdy coś zostanie odebrane
   { Mirf.getData((byte *) &napisS); 
 Serial.println(napisS);      // Wystawienie zmiennej na UART
 Serial.println(getData(napisS,0));//getdata to funkcja odczytajaca ze stringu, z 0 pierwszyczłon z 1 drugi
Serial.println(getData(napisS,1)); 
  }

String temp_S=String(random(-20,40));
String press_S=String(random(800,1025));


String piec_S=getData(napisS,0);
String zew_S=getData(napisS,1);
  
 mySerial.println("{\"S\":01,\"T\":"+ zew_S +",\"P\":"+ press_S +",\"H\":84,\"D\":18,\"O\":"+ piec_S +"}");
  Serial.println("wysylam.....");
  Serial.println("{\"S\":01,\"T\":"+ zew_S +",\"P\":"+ press_S +",\"H\":84,\"D\":18,\"O\":"+ piec_S +"}");
 delay(2000);

// delay(500);
}
