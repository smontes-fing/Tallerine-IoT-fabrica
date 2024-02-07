#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "secrets.h"
#include "DHT.h"
#define TIME_ZONE -3

#define LED_PIN 10
#define DHTPIN 4        // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
 
DHT dht(DHTPIN, DHTTYPE);
 
float h ;
// float t;
String t= "test";
unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000;
 
// #define AWS_IOT_PUBLISH_TOPIC   "esp8266/pub"
// #define AWS_IOT_SUBSCRIBE_TOPIC   "esp8266/sub"
#define AWS_IOT_PUBLISH_TOPIC   "tallerine/gr1/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "tallerine/gr1/sub"
 
WiFiClientSecure net;
 
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);
 
PubSubClient client(net);
 
time_t now;
time_t nowish = 1510592825;
 
 
void NTPConnect(void)
{
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}
 
 
void messageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Parse the incoming message
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload, length);

  // Check if parsing was successful
  if (!error) {
    // Extract data from the JSON message
    const char* command = doc["command"];

    // Handle different commands
    if (strcmp(command, "led_on") == 0) {
      // Turn on the LED
      // digitalWrite(LED_PIN, HIGH);
      Serial.println("LED encendido");
    } else if (strcmp(command, "led_off") == 0) {
      // Turn off the LED
      // digitalWrite(LED_PIN, LOW);
      Serial.println("LED apagado");
    } else {
      // Invalid command
      Serial.println("Comando inválido");
    }
  } else {
    // Parsing failed
    Serial.println("Error al analizar JSON");
  }

}
 
 
void connectAWS()
{
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));
 
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
 
  NTPConnect();
 
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
 
  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);
 
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
  }
 
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}
 
 
void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["humidity"] = h;
  doc["temperature"] = t;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
 
 
void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN,OUTPUT); // antes de connectAWS() sino da error por no estar definido
  connectAWS();
  dht.begin();
  digitalWrite(LED_PIN,LOW);
}
 
 
void loop()
{
  // h = dht.readHumidity();
  // t = dht.readTemperature();
  h = 35;
  t = "test_temp"; 
//  if (isnan(h) || isnan(t) )  // Check if any reads failed and exit early (to try again).
  if (isnan(h))  // Check if any reads failed and exit early (to try again).
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
 
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F(" °C "));
  delay(10000);
 
  now = time(nullptr);
 
  if (!client.connected())
  {
    connectAWS();
  }
  else
  {
    client.loop();
    if (millis() - lastMillis > 10000)
    {
      lastMillis = millis();
      publishMessage();
    }
  }
}
