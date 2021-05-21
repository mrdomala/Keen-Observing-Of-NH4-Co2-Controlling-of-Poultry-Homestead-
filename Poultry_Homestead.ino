#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include <MHZ19PWM.h>
#include "DHT.h"
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
MHZ19PWM mhz(2, MHZ_ASYNC_MODE);
unsigned long _time = 0;
bool waitMode = false;
#define FIREBASE_HOST "major-project-04c1721-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "I0s4Dm97mfZYiGk18WMj21Tsrj2D3QeRxoBTJ5av"
#define WIFI_SSID "Realme 2"//"ACT115527830665"//
#define WIFI_PASSWORD "12345678"//"77590949"//
int fireStatus = 0;
void setup() {
  Serial.begin(115200);
  Serial.println("DHT11 test!");
  dht.begin();
  Serial.println(F("Starting..."));
  delay(2000);
  mhz.useLimit(5000);
  
  pinMode(19,OUTPUT);//nh4
  pinMode(18,OUTPUT);//nh4
  pinMode(23,OUTPUT);//dw
  pinMode(22,OUTPUT);//dw
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.set("Motor",0);
}
void showValue()
{
  unsigned long start = millis(); 
  float  co2 = mhz.getCO2();
  unsigned long duration = millis() - start;
  Serial.print(" \t");
  Serial.print(F("Carbondioxide : "));
  Serial.print(co2);
  Serial.println(" PPM ");
  Serial.print(" \t");
  Serial.print(F("Duration: "));
  Serial.println(duration);
  Serial.println();
}
void loop() {
  //
   unsigned long ms = millis();

  if (waitMode)
  {
    if (ms - _time >= 500 || ms < _time)
    {
      _time = ms;
      bool state = mhz.isDataReady();
      Serial.print(F("isDataReady: "));
      Serial.println(state);
      
      if (state)
      {
        waitMode = false;
        showValue();
      }
    }
  }
  else
  {
    if (ms - _time >= 10000 || ms < _time)
    {
      _time = ms;
      waitMode = true;
      mhz.requestData();
      Serial.println(F("Request data"));
    }
  }
  ///
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int n = analogRead(14); // Ammonia
  float co2;
  float r=random(0,1000);
  float p=map(r, 0, 1000, 0, 100);
  if(n==0)
  {
    digitalWrite(18,HIGH);
    digitalWrite(19,LOW);
    Serial.println("Cleaning Pump On");
  }
  
  int ldr = analogRead(25); // ldr
  int v=random(0,1024);
    if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print(" \t");
  Serial.print("Humidity : ");
  Serial.println(h);
  Serial.print(" \t");
  Serial.print("Temperature : ");
  Serial.print(t);
  Serial.println(" *C ");
  Serial.print(" \t");
  Serial.print("Ammonia : ");
  Serial.print(n);
  Serial.println(" ppm ");
  Serial.print(" \t");
  Serial.print("Carbondioxide : ");
  Serial.print(p);
  Serial.println(" % ");
  Serial.print(" \t");
  Serial.print("ldr : ");
  Serial.print(v);
  Serial.println(" LUX ");


  // set value
  Firebase.setFloat("Temperature", t);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);
  Firebase.setFloat("Ammonia", n);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);
  Firebase.setFloat("Carbondioxide", p);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(1000);
  
  Firebase.setFloat("Humidity", h);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
  }
  Firebase.setFloat("ldr", v);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
  }
 fireStatus = Firebase.getInt("Motor");
  if (fireStatus == 1) {
    Serial.println("MOTOR ON");
    digitalWrite(23, HIGH);
    digitalWrite(22, LOW);
  }
  else if (fireStatus == 0) {
    Serial.println("MOTOR OFF");
    digitalWrite(22, LOW);
    digitalWrite(23, LOW);
  }
  delay(1000);
  Serial.println ("Data Sent");
  delay(1000);
}
