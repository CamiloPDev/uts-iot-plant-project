#include <WiFi.h>
#include <PubSubClient.h>

const int SENSOR_HUMEDAD = 33;
const int SENSOR_NIVEL_AGUA = 32;
const int MOTOR = 12;
const int LED_RED = 14;
const int LED_GREEN = 26;
const int LED_BLUE = 25;


// Credenciales WiFi
const char* ssid = "redWifi";  // Nombre de la red
const char* password = "1234";

// MQTT server
const char* mqtt_server = "3.148.108.101";  // url servidor

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Recibir mensajes MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Create a string from the payload
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Check if the message is for the bomba topic
  if (strcmp(topic, "/ThinkIOTProject/bomba") == 0) {
    if (message == "1") {
      digitalWrite(MOTOR, HIGH);  // Turn the LED on
    } else if (message == "0") {
      digitalWrite(MOTOR, LOW);  // Turn the LED off
    }
  }

  // Check if the message is for the led topic
  if (strcmp(topic, "/ThinkIOTProject/led") == 0) {
    if (message == "1") {
      digitalWrite(LED_RED,LOW);
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_BLUE,LOW);
    } else if (message == "2") {
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_RED,HIGH);
      digitalWrite(LED_BLUE,LOW);
    } else if (message == "3") {
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED,HIGH);
      digitalWrite(LED_BLUE,LOW);
    }
  }
}

// Conexion MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected");
      client.publish("/ThinkIOTProject/Publish", "Welcome");
      client.subscribe("/ThinkIOTProject/Subscribe");
      client.subscribe("/ThinkIOTProject/bomba");  // Subscribe to the bomba topic
      client.subscribe("/ThinkIOTProject/led");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(SENSOR_HUMEDAD,INPUT);
  pinMode(SENSOR_NIVEL_AGUA,INPUT);
  pinMode(MOTOR,OUTPUT);
  pinMode(LED_RED,OUTPUT);
  pinMode(LED_GREEN,OUTPUT);
  pinMode(LED_BLUE,OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {  // Publish data every 2 seconds
    lastMsg = now;

    String humedad = String(analogRead(SENSOR_HUMEDAD));
    client.publish("/ThinkIOTProject/hum", humedad.c_str());
    Serial.print("Humidity: ");
    Serial.println(humedad);

    String nivelAgua = String(analogRead(SENSOR_NIVEL_AGUA));
    client.publish("/ThinkIOTProject/nivelAgua", nivelAgua.c_str());
    Serial.print("Nivel del agua: ");
    Serial.println(nivelAgua);
  }
}
