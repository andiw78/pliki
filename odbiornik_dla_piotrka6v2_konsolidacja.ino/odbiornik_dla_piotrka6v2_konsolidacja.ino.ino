
// program terminal pytam sie o zapis nowych danych do projektu typu nazwa wifi ssid hasło
// i zapisuje w postaci json z pamieci spifs
String odebraneDane=""; //Pusty ciąg odebranych danych

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <LittleFS.h>
#include "ThingSpeak.h" 
#include <SoftwareSerial.h>
#include <SimpleTimer.h>
#include "secrets.h"
WiFiClient  client;
char *myssid; // your network SSID (name) 
char *mypass;   // your network password
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;




boolean buttonState = true; // aktualny stan zaświecenia diody
int buttonPin = D2; //pin do odczytu stanu przycisku
bool zapis_ssid=true;
bool czas_menu=false;// sprwdzam czas zeby menu sie samo wyłaczyło
uint32_t previousMillis = 0;
uint32_t currentMillis;
const uint32_t interval = 50000;   

SoftwareSerial mySerial(D7, D8); // RX, TX
//SimpleTimer firstTimer(60000);
SimpleTimer firstTimer(6000);
int ledState = LOW;
char json[100];
int i = 0;



String wifi="andik";
 String haslo="dupkaemilki";   

     
void setup() {
  Serial.begin(9600); //Uruchomienie komunikacji
loadConfig();

//String haslo="dupkaemilki";
int dlug = wifi.length()+1;
wifi.toCharArray(myssid,dlug);
//haslo.toCharArray(mypass,haslo.length()+1);


 //myssid =  wifi;  // your network SSID (name) 
 mypass =  "dupkaemilki";  // your network password

 Serial.println("to jest konversja");
 
 Serial.print(myssid);
 Serial.print(mypass);
 
   pinMode(D2, INPUT_PULLUP);
   pinMode(LED_BUILTIN, OUTPUT);
 
digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("odbiornik wita");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  
WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak





Serial.println("......................................................");



if (!LittleFS.begin()) {//...............................................................................uruchom obsluge spifs
    Serial.println("Failed to mount file system");
    return;
  }

   if (!loadConfig()) {//..................................................................................wczytaj hasło i token
    Serial.println("nie moge odczytac danych, może ich tam brak?");
  } else {
    Serial.println("udało sie wczytać z pamięci");
  }

}

  long S; 
  long T; 
  long P; 
  long H; 
  long D; 
  long O; 

void mySerialFunction() {
   // whatever you want
   StaticJsonDocument<100> doc;//200
String myString = "";
myString = mySerial.readStringUntil('\n'); 
 Serial.println(myString);
  //char json[myString.length()];//działa
  myString.toCharArray(json, myString.length()+1);//zamieniam otrzymanego stringa na char array
      Serial.println(json);
   DeserializationError error = deserializeJson(doc, json);
 if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

   S = doc["S"];
   T = doc["T"];
   P = doc["P"];
   H = doc["H"];
   D = doc["D"];
   O = doc["O"];
  Serial.println("zdekodowałem");
   // Print values.
  Serial.println(S);
  Serial.println(T);
  Serial.println(P);
  Serial.println(H);
  Serial.println(D);
  Serial.println(O); //wykonaj, jeśli dostępne dane
   
}







String ssid_s;// nazwa sieci
String pass_s;//hasło sieci
String token_s;// nazwa sieci
String api_s;//hasło sieci





bool saveConfig() {//..................................................................................zapisuje dane do pamieci spifs
  StaticJsonDocument<200> doc;



                                //jezeli wybieramy w menu tylko samis ssid i pass
  doc["ssid"] = ssid_s;
  doc["pass"] = pass_s;
 doc["token"] = token_s; 
 doc["api"] = api_s;
 
if (zapis_ssid){


}

 // doc["token"] = token_s;                             // jezeli wybieramy w menu tylko zmiane tokena
 // doc["api"] = api_s;



  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  serializeJson(doc, configFile);
  return true;
}


//char ssid[] = "andik";   // your network SSID (name) 
//char pass[] = "dupkaemilki"; 





bool loadConfig() {//odczyt danych z pamieci spifs//............................................................odczyt danych z pamieci
File configFile = LittleFS.open("/config.json", "r");
   
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  const char* ssid = doc["ssid"];
  const char* pass = doc["pass"];
  const char* token = doc["token"];
  const char* api = doc["api"];


  // Real world application would store these values in some variables for
  // later use.
  Serial.print("wifi ssid: ");
  Serial.println(ssid);
  Serial.print("hasło pass: ");
  Serial.println(pass);
  Serial.print("          token: ");
  Serial.println(token);
  Serial.print("       Write api: ");
  Serial.println(api);
 
//String ssid_s;// nazwa sieci
//String pass_s;//hasło sieci
if (zapis_ssid){
  token_s=token;// nazwa sieci
  api_s=api;//hasło sieci
}else{
   ssid_s=ssid;// nazwa sieci
  api_s=api;//hasło sieci
}



  
  return true;
}









void scanwifi(){
  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t* bssid;
  int32_t channel;
  bool hidden;
  int scanResult;

  WiFi.mode(WIFI_STA);

  // Disconnect from an AP if it was previously connected
  WiFi.disconnect();
  delay(100);



  Serial.println(F("zaczynam skan WiFi..."));

  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);

  if (scanResult == 0) {
    Serial.println(F("brak sieci"));
  } else if (scanResult > 0) {
    Serial.printf(PSTR("%d znaleziono sieci:\n"), scanResult);

    // Print unsorted scan results
    for (int8_t i = 0; i < scanResult; i++) {
      WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);

      Serial.printf(PSTR("  %02d: [CH %02d]  %s\n"),
                    i,
                    channel,

                    ssid.c_str());
      delay(0);
    }
  } else {
    Serial.printf(PSTR("WiFi scan error %d"), scanResult);
  }

  // Wait a bit before scanning again
  delay(500);
}








bool decyzja=false;;

void ssid_(){//...............................................................................pierwsza opcja menu
//scanwifi();//skanuje siec potem odkomentuj
zapis_ssid=true;


  
   Serial.println("zmiana sieci wifi? T/N"); // upewnia sie czy chce zmienic dane
  while(!Serial.available());
   String znak_decyz = Serial.readStringUntil('\n'); 
 if (znak_decyz== "T" || znak_decyz== "t"){    decyzja = true; }
   
   
  while (decyzja){


 
  delay(200);

  
   Serial.println("wpisz nazwę nowej sieci,  zatwierdz enterem");
  while(!Serial.available());
 ssid_s = Serial.readStringUntil('\n'); 


   
    Serial.println("wybrana siec>     " + ssid_s); //Wyświetl komunikat
 
   Serial.println("podaj haslo,  zatwierdz enterem");
 
  delay(200);
   while(!Serial.available());
    pass_s = Serial.readStringUntil('\n'); 
   Serial.println("wybrana siec>     " + pass_s); //Wyświetl komunikat
  
// wywoluje procedure, ktora zapisuje dane /wifi i haslo/ do pamieci
 if (!saveConfig()) {
    Serial.println("Failed to save config");
  } else {
    Serial.println("dane zapisane");
  }

decyzja = false;
  }     //koniec while
  zapis_ssid=false;
}

void tokeny()//.......................................................................................//druga opcja menu
{
zapis_ssid=false;


  
   Serial.println("zmiana tokenu? T/N"); // upewnia sie czy chce zmienic dane
  while(!Serial.available());
   String znak_decyz = Serial.readStringUntil('\n'); 
 if (znak_decyz== "T" || znak_decyz== "t"){    decyzja = true; }
   
   
  while (decyzja){


 
  delay(200);

  
   Serial.println("wpisz nowy token");
  while(!Serial.available());
token_s = Serial.readStringUntil('\n'); 


   
    Serial.println("token >     " + token_s); //Wyświetl komunikat
 
   Serial.println("podaj writeapi,  zatwierdz enterem");
 
  delay(200);
   while(!Serial.available());
    api_s = Serial.readStringUntil('\n'); 
   Serial.println("wybrana siec>     " + api_s); //Wyświetl komunikat
  
// wywoluje procedure, ktora zapisuje dane /wifi i haslo/ do pamieci
 if (!saveConfig()) {
    Serial.println("Failed to save config");
  } else {
    Serial.println("dane zapisane");
  }

decyzja = false;
  }     //koniec while


zapis_ssid=true;


  
}






void menu() {

if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }





  do{





Serial.println("dotychczasowe dane:"); 
  
    if (!loadConfig()) {
    Serial.println("nie moge odczytac danych, może ich tam brak?");
  } else {
    Serial.println("udało sie wczytać z pamięci");
  }



  
Serial.println("to jest terminal konfiguracyjny/masz 20sekund"); //Wyświetl komunikat
  Serial.println("wybierz co chcesz zrobić, wpisz odpowiednia cyfrę i zatwierdz enterem"); //Wyświetl komunikat
  Serial.println("1-  konfiguracja wifi"); //Wyświetl komunikat
  Serial.println("2-  konfiguracja tokenów Thingspeak"); //Wyświetl komunikat
  Serial.println("3-  wyjdź"); //Wyświetl komunikat


//while(!Serial.available());

int a=0;
do{
if(Serial.available())
   {
       odebraneDane = Serial.readStringUntil('\n');
       czas_menu=false;
   }
a++;delay(100);
if (a>200){czas_menu=false;
  Serial.println(".......................................czas minął");
odebraneDane="3";
};

}while(czas_menu);
    // break;
     a=0;
  Serial.println(odebraneDane);

 

  
  if (odebraneDane=="1"){
    Serial.println("wybrano 1");

//odczyt zapisanych danych

    ssid_();
  }
    if (odebraneDane=="2"){
    Serial.println("wybrano 2");
  tokeny();
    
  }

   // delay(1000);

czas_menu=true;

  }while(odebraneDane!="3" );
   Serial.println("wychodze z menu");
   previousMillis = currentMillis;
    czas_menu=true;
}


void loop() {


  
 
 //********************************************************************************************************************************************************* 
if (digitalRead(buttonPin) == LOW) {
     delay(20); //20ms na obsługę drgań styków
     buttonState = !buttonState; //ustawienie przeciwnego stanu diody
     if (buttonState == true){
      Serial.println("BRAWO    wcisnoles przycisk");
     menu();
     }  else {
       
     }

     }

 //*********************************************************************************************************************************************************
if(mySerial.available())
   {
       mySerialFunction();
   }

 //*********************************************************************************************************************************************************
   if (firstTimer.isReady()) {            // Check is ready a first timer
        Serial.println(".............................................................minął czas");

 if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(myssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(wifi, haslo); 
      //WiFi.begin(myssid, mypass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }






//int number1 = random(-20,40);
//int number2 = random(0,100);
//int number3 = random(0,50);
//float number4 = random(950,1040);
  ThingSpeak.setField(1, T);
  ThingSpeak.setField(2,D);
  ThingSpeak.setField(3,H);
  ThingSpeak.setField(4, P);




        
         digitalWrite(LED_BUILTIN, LOW);
      delay(50);
      digitalWrite(LED_BUILTIN, HIGH);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
Serial.println(".............................................................wysłałem");
    // Turn the LED on (Note that LOW is the voltage level
  // but actually the LED is on; this is because
  // it is active low on the ESP-01)
                        // Wait for a second
 
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

          
        // Set the flag so as not to fall into this condition
       firstTimer.reset(); 
    }

 

}
