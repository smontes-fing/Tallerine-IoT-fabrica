#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* mqtt_server = "your-mqtt-endpoint";
const int mqtt_port = 8883;
const char* mqtt_client_id = "Auto1";

// Rutas a los archivos de certificado
const char* root_ca = 
"-----BEGIN CERTIFICATE-----\n"
// Tu certificado de CA aquí
"-----END CERTIFICATE-----\n";

const char* certificate = 
"-----BEGIN CERTIFICATE-----\n"
// Tu certificado de dispositivo aquí
"-----END CERTIFICATE-----\n";

const char* private_key = 
"-----BEGIN PRIVATE KEY-----\n"
// Tu clave privada aquí
"-----END PRIVATE KEY-----\n";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  espClient.setCACert(root_ca);
  espClient.setCertificate(certificate);
  espClient.setPrivateKey(private_key);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  if (message == "entregar_paquete") {
    // Código para entregar el paquete
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("connected");
      client.subscribe("auto/auto1/estado");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
