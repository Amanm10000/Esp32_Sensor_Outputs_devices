//Arduino Code for ESP32-AI Thinker CAM... with [Ultrasonic sensor/ DHT sensor/ Output devices]. 
//Yup.
#include <HCSR04.h>


#include <DHT.h>
#include <DHT_U.h>
DHT dht(13, DHT11);

#include <common.h>
#include <FirebaseESP32.h>

const byte triggerPin = 12;
const byte echoPin = 13;
const byte maxDistanceCm = 190;
UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin, maxDistanceCm);


#define FIREBASE_HOST "espdatabase-sciencekidz-default-rtdb.firebaseio.com"
#define Firebase_AUTH "WBBILVFZOXPGWMk0Vg3z9Fy6qCzpJZ7OzLf5q85s"
#define WIFI_SSID "Skyworth_0A9383"
#define WIFI_PASSWORD "00000000"
#define POT 1
#define LED 14
#define sndSpd 0.034
//#define MOIST 4

#define one "1"


FirebaseData frtdb;
FirebaseJson json;
FirebaseJson aman;
int dhtsensor = 13;
int dhtdata = 0;

//int pin = 13;
//int value = 0;
int counts = 0;

int trig = 12;
int echo = 14;
long duration;
//unsigned int distance;
float t;
float distance;

void setup() {
  Serial.begin(115200);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);



  pinMode(dhtsensor, INPUT);


  dht.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connection to WiFI....");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected with IP : ");
  Serial.println(WiFi.localIP());
  Serial.println();


  Firebase.begin(FIREBASE_HOST, Firebase_AUTH);
  Firebase.reconnectWiFi(true);

  Serial.println("Coneccted to Firebase.");
}
void dht_sensor_func(float temps) {
   if(isnan(temps)){
    Serial.println("There's some error with  your dht. :( ");
    
    json.set("DHT_Value/Temp", "Er_404");
 //   Firebase.updateNode(frtdb, "/DHT_Value", json);
    delay(150);
   }
   else {
   json.set("DHT_Value/Temp", temps);

   Serial.println("data pasted to Firebase");
    Serial.println();
   }
  
}
void ultrasonic_func(float distu) {
    if(isnan(distu) || distu < 0){
  Serial.println("Nothing is in Sight");
  json.set("Ultra_value/Distance", "nothing");
  
}
else {
  Serial.print("object detected at : ");
  Serial.print(distu);
  json.set("Ultra_value/Distance", distu);
  Serial.println(" cm");
}
//Firebase.updateNode(frtdb, "/Ultra_value", jsontwo);
  
}

void loop() {



if(Firebase.getString(frtdb, "/LEDState")){

  if (frtdb.dataType() == "string"){
    String state = frtdb.to<const char *>();

    if (state == one){
      digitalWrite(LED, HIGH);

    }
    else if (state != one){
      digitalWrite(LED, LOW);    
    }
 //  Serial.println(digitalRead(LED));
  }
  
}
else {
Serial.println(frtdb.errorReason());
}

float tpre = dht.readTemperature();
float dispre = distanceSensor.measureDistanceCm();
Serial.println(t);
if(!isnan(tpre) && tpre != t){
  t = tpre;
  dht_sensor_func(t);
}
delay(90);
if(dispre != distance){
  distance = dispre;
  ultrasonic_func(distance);
}
delay(90);
counts++;
json.set("DHT_Value/count", counts);
json.set("Ultra_value/count", counts);
delay(50);
Firebase.updateNode(frtdb, "", json);

}
