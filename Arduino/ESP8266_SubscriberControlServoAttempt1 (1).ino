/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-mqtt-publish-ds18b20-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <Servo.h>

#define WIFI_SSID "Pixel_5890"
#define WIFI_PASSWORD "bust@123"

// Raspberri Pi Mosquitto MQTT Broker
#define MQTT_HOST IPAddress(192, 168, 186, 75)
// For a cloud MQTT broker, type the domain name
//#define MQTT_HOST "example.com"
#define MQTT_PORT 1883
const int LEDPIN = 4;
int move_servo;
String messageTemp;
// Temperature MQTT Topics
//#define MQTT_PUB_TEMP "esp/ds18b20/temperature"
#define MQTT_SUB_Output "esp32/OutputControl"

// GPIO where the DS18B20 is connected to
//const int oneWireBus = 4;          
// Setup a oneWire instance to communicate with any OneWire devices
//OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
//DallasTemperature sensors(&oneWire);
// Temperature value
//float temp;

Servo myservo;                                                // create servo object to control a servo
// twelve servo objects can be created on most boards
int pos = 0;                                                           // variable to store the servo position
AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 10000;        // Interval at which to publish sensor readings

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  uint16_t packetIdSub = mqttClient.subscribe(MQTT_SUB_Output, 2);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

/*void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}
*/
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("\n Publish received.");
  Serial.print("topic: ");
  Serial.println(topic);
  
  for (int i = 0; i < len; i++) {
    messageTemp += (char)payload[i];
  }
  //messageTemp = (char)payload;
    Serial.print("Message: ");
    Serial.println(messageTemp);

  if (messageTemp == "1"){
  Serial.println("LED is now ON!");
  move_servo = 1;
  Serial.println("Message Check");
  Serial.println(move_servo);
   
  }


  else if(messageTemp == "0"){ 
  Serial.println("LED is now OFF");
  move_servo = 0;
 
  }
}


void setup() {
  //sensors.begin();
  Serial.begin(115200);
  Serial.println();
  
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
 
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  //mqttClient.onSubscribe(onMqttSubscribe);
  //mqttClient.onUnsubscribe(onMqttUnsubscribe);
  //mqttClient.onPublish(onMqttPublish);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  //mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");
  
  connectToWifi();
  
  myservo.attach(2);  // attaches the servo on pin 4 to the servo object
  myservo.write(0);
  delay(2000);
  //Serial.println("Servo initial");
}

void loop() {

  Serial.println(move_servo);
                                    // attaches the servo on pin 4 to the servo object
  if (move_servo == 0) {
         myservo.write(0);
         delay(1000);
      
      
         Serial.println("Servo position 0");
  }
   else  {
      
         myservo.write(90);
         delay(1000);
      
      
         Serial.println("Servo position 90");
      
    }
  messageTemp = "";

  /*myservo.write(180);
  Serial.println("First position");
  delay(1000);
  // Servo is stationary for 1 second.
  myservo.write(90);
  Serial.println("Stationary");
  delay(1000);
  // Servo spins in reverse at full speed for 1 second.
  myservo.write(0);
  Serial.println("Reverse");
  delay(1000);
  // Servo is stationary for 1 second.
  myservo.write(90);
  delay(1000);*/
 }
  
