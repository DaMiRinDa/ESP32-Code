#include <WiFi.h>
#include <HTTPClient.h>
#include "ThingSpeak.h"
#include <DHT.h>
#include <PZEM004Tv30.h>


#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#endif
#if !defined(PZEM_SERIAL)
#define PZEM_SERIAL Serial2
#endif
#if defined(ESP32)
PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN);
#elif defined(ESP8266)
#else
PZEM004Tv30 pzem(PZEM_SERIAL);
#endif

#define ANALOG_IN_PIN 35  
#define DHTPIN 25  

int sensorIn = 34;
int relay1= 23;
int relay2= 22;
int relay3= 21;
int relay4= 19;
int relay5= 18;
int relay6= 5;
int relay7= 27;

int ledPower = 33;  
int dustPin = 32;  
int delayTime = 280;
int delayTime2 = 40;
float offTime = 9680;
float adc_voltage = 0.0;
float in_voltage = 0.0;
float R1 = 30000.0;
float R2 = 7500.0; 
float ref_voltage = 5.0;
int adc_value = 0;
float current;
float voltage;

// current sensor
#define currentdc 36
float vpp = 0.0007947771785; //3.3/4095  
float sens = 0.185;

WiFiClient  client;

HTTPClient http;
DHT dht(DHTPIN, DHT11);

const char* ssid ="S10";
const char* pass ="12345678";
String ServerName = "https://api.thingspeak.com/update?api_key=UZOBPP8IJ3H1SSBB&field1=0";


unsigned long myChannelNumber = 1956914;
unsigned long myChannelNumber1 = 2150563;
const char * myWriteAPIKey = "UZOBPP8IJ3H1SSBB";
const char * myReadAPIKey = "DS5YZASSUZARG2XE";
const char * myReadAPIKey1 = "T25I4H4YD8CXR5QT";
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

void setup() {
  Serial.begin(115200);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);
  digitalWrite(relay5, HIGH);
  digitalWrite(relay6, HIGH);
  pinMode(ledPower, OUTPUT);
  pinMode(dustPin, INPUT);
  pinMode(sensorIn, INPUT);
  
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(relay3,OUTPUT);
  pinMode(relay4,OUTPUT);
  pinMode(relay5,OUTPUT);
  pinMode(relay6,OUTPUT);
  pinMode(relay7,OUTPUT);
  pinMode(currentdc, INPUT);
  

  dht.begin();
  ThingSpeak.begin(client);
   Wifi_connect();
}

void loop() {
  /// dust sensor
  digitalWrite(ledPower, LOW); // power on the LED
  delayMicroseconds(delayTime);
  float dustVal = analogRead(dustPin); 
  delayMicroseconds(delayTime2);
  digitalWrite(ledPower, HIGH); // turn the LED off
  delayMicroseconds(offTime);
  float dustDensity = (0.172*(dustVal*(5/4095.0))-0.0999)*1000;
// DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();
   if (isnan(h) || isnan(t)) 
        {
          Serial.println("Failed to read from DHT sensor!");
           return;
        }
        
/// Voltage Sensor
   adc_value = analogRead(ANALOG_IN_PIN);
   adc_voltage = (adc_value * 3.3) / 4096.0;  // Determine voltage at ADC input
   in_voltage = adc_voltage / (R2/(R1+R2)) ;  // Calculate voltage at divider input

// 
// ========================current DC=====================   
float Samples = 0.0 , counts=0.0, avgCounts=0.0;
  for (int x =0; x<500; x++){
     counts = analogRead(currentdc);
     Samples += counts;
      delay(3);
    }
  avgCounts = Samples/500;
 
  float voltage = avgCounts * vpp; // 3.3/4096
  float voltage1 = avgCounts * vpp;
  if (voltage1 <= 2.50) {
    voltage -= 2.4;
  }else if (voltage1 >= 2.76) {
    voltage -= 2.47;
  }else if (voltage1 < 2.76){
      voltage -= 2.45;
  } 

  float amp = voltage / sens;

   if (amp < 0.10) {
       amp = 0;
    }
    
  

  Serial.print("Dust:");
  Serial.print(dustDensity);
  Serial.println("  ug/m3");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" degrees Celcius, Humidity: ");
  Serial.println(h);

  Serial.print("Input Voltage = ");
  Serial.println(in_voltage, 2);

  Serial.print("amps "+ String(amp));


   voltage = pzem.voltage();
   current = pzem.current();

// relay channel 1
 int last_light_state = ThingSpeak.readIntField(myChannelNumber1, 1, myReadAPIKey1);
 if (last_light_state == 2){
   digitalWrite(relay1, LOW);
    Serial.println("LIGHT is On");
 }else{
  digitalWrite(relay1, HIGH);
  Serial.println("LIGHT is Off");
 }
// relay channel 2

 int last_light_state1 = ThingSpeak.readIntField(myChannelNumber1, 2, myReadAPIKey1);
 if (last_light_state1 == 2){
   digitalWrite(relay2, LOW);
    Serial.println("LIGHT2 is On");
 }else{

      digitalWrite(relay2, HIGH);
      Serial.println("LIGHT2 is Off");
 }

// relay 3

int last_light_state2 = ThingSpeak.readIntField(myChannelNumber1, 3, myReadAPIKey1);
 if (last_light_state2 == 2){
   digitalWrite(relay3, LOW);
    Serial.println("LIGHT3 is On");
 }else{

      digitalWrite(relay3, HIGH);
      Serial.println("LIGHT3 is Off");
 }

//relay 4
 int last_light_state3 = ThingSpeak.readIntField(myChannelNumber1, 4, myReadAPIKey1);
 if (last_light_state3 == 2){
   digitalWrite(relay4, LOW);
    Serial.println("LIGHT4 is On");
 }else{

      digitalWrite(relay4, HIGH);
      Serial.println("LIGHT4 is Off");
 }
 
 
 //relay 5
 int last_light_state4 = ThingSpeak.readIntField(myChannelNumber1, 5, myReadAPIKey1);
 if (last_light_state4 == 2){
   digitalWrite(relay5, LOW);
    Serial.println("LIGHT5 is On");
 }else{
  digitalWrite(relay5, HIGH);
  Serial.println("LIGHT5 is Off");
 }
 
 // relay 6

 int last_light_state5 = ThingSpeak.readIntField(myChannelNumber1, 6, myReadAPIKey1);
 if (last_light_state5 == 2){
   digitalWrite(relay6, LOW);
    Serial.println("LIGHT6 is On");
 }else{

      digitalWrite(relay6, HIGH);
      Serial.println("LIGHT6 is Off");
 }
 
 /// relay 7 
 
  int last_light_state6 = ThingSpeak.readIntField(myChannelNumber1, 7, myReadAPIKey1);
 if (last_light_state6 == 2){
   digitalWrite(relay7, HIGH);
    Serial.println("LIGHT6 is On");
 }else{

      digitalWrite(relay6, LOW);
      Serial.println("LIGHT6 is Off");
 }


    String url = ServerName + "&field1="+ t + "&field2=" + h + "&field3="+ dustDensity + "&field4=" + current +"&field5=" + in_voltage +"&field6="+ amp +"&field8=" + voltage ;
    http.begin(url.c_str());
    int httpResponseCode = http.GET();
    http.end();
    lastTime=millis();   
  delay(1000);
}



void Wifi_connect(){
        WiFi.begin(ssid,pass);
        Serial.println("Connecting to ");
        Serial.println(ssid);

        while (WiFi.status() != WL_CONNECTED) 
          {
                  delay(500);
                  Serial.print(".");
          }
            Serial.println("");
            Serial.println("WiFi connected");
            Serial.println(WiFi.localIP());

}