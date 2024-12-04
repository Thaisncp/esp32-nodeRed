#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <PubSubClient.h>

/* Credenciales de WiFi */
const char* ssid = "DAYSI";
const char* password = "1106042474001";

/* Configuración del broker MQTT */
const char* mqtt_server = "20.205.17.176";
const int mqtt_port = 1883;
const char* topic = "canal1";
//const char* ssid = "Internet_UNL";
//const char* password = "UNL1859WiFi";

/* Inicialización del cliente MQTT */
WiFiClient espClient;
PubSubClient client(espClient);

/* Configuración del sensor BME280 */
Adafruit_BME280 bme;

/* Función para conectar al broker MQTT */
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    if (client.connect("ESP32-BME280")) {
      Serial.println("Conectado.");
    } else {
      Serial.print("Error, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 10 segundos.");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Conexión a WiFi
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" WiFi conectado.");

  // Configuración del broker MQTT
  client.setServer(mqtt_server, mqtt_port);

  // Configuración del sensor BME280
  if (!bme.begin(0x76)) {
    Serial.println("No se encontró el sensor BME280. Revisa la conexión.");
    while (1);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leer datos del sensor
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;
  float altitude = bme.readAltitude(1013.25); 

  // Crear el mensaje en formato JSON
  String payload = "{\"temperature\":" + String(temperature) + 
                   ",\"humidity\":" + String(humidity) +
                   ",\"pressure\":" + String(pressure) +
                   ",\"altitude\":" + String(altitude) + "}";

  // Publicar datos al broker MQTT
  client.publish(topic, payload.c_str());

  // Imprimir datos en consola
  Serial.println("Datos enviados a MQTT:");
  Serial.println(payload);

  delay(30000);
}