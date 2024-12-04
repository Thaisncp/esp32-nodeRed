#include <WiFi.h>
#include <PubSubClient.h>

/* Credenciales de WiFi */
const char* ssid = "DAYSI";
const char* password = "1106042474001";
//const char* ssid = "Internet_UNL";
//const char* password = "UNL1859WiFi";

/* Configuración del broker MQTT */
const char* mqtt_server = "20.205.17.176";  // IP pública de tu VM en Azure
const int mqtt_port = 1883;
const char* topic_subscribe = "canal2"; // Tema MQTT para suscribirse y recibir comando de LED

/* Inicialización del cliente MQTT */
WiFiClient espClient;
PubSubClient client(espClient);

/* Pin del LED */
const int LED_PIN = 2;

/* Función para conectar al broker MQTT */
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    if (client.connect("ESP32-LED")) {
      Serial.println("Conectado.");
      client.subscribe(topic_subscribe);  // Suscribir al tema de control de LED
      Serial.print("Suscrito al tema: ");
      Serial.println(topic_subscribe);
    } else {
      Serial.print("Error, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos.");
      delay(5000);
    }
  }
}

/* Función callback para procesar los mensajes entrantes */
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Mensaje recibido en el tema: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(message);

  // Verificar si el mensaje pertenece al tema esperado
  if (String(topic) == topic_subscribe) {
    // Controlar el LED según el valor recibido
    if (message == "encendido") {
      digitalWrite(LED_PIN, HIGH);  // Encender el LED
      Serial.println("LED ENCENDIDO");
    } else if (message == "apagado") {
      digitalWrite(LED_PIN, LOW);   // Apagar el LED
      Serial.println("LED APAGADO");
    } else {
      Serial.println("Mensaje no válido para el control del LED.");
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
  client.setCallback(callback);  // Establecer la función callback para procesar mensajes

  // Configuración del pin del LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Apagar el LED al inicio
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // Procesar mensajes MQTT
}
