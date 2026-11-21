#include <WiFi.h>
#include <PubSubClient.h>

#define LEDPIN 26

const char* ssid   = "Wokwi-GUEST";
const char* broker = "broker.hivemq.com";
const char* topicoComando = "airguard-upm/d2/comando";
const char* topicoEstado  = "airguard-upm/d2/estado";

WiFiClient espClient;
PubSubClient mqtt(espClient);
bool ledLigado = false;

void publicaEstado() {
  mqtt.publish(topicoEstado, ledLigado ? "ON" : "OFF", true);
}

void callback(char* topic, byte* payload, unsigned int len) {
  String m;
  for (unsigned int i = 0; i < len; i++) m += (char)payload[i];
  m.trim();
  m.toUpperCase();
  Serial.println("Comando recebido: " + m);
  if (m == "ON") {
    ledLigado = true;
    digitalWrite(LEDPIN, HIGH);
    publicaEstado();
  } else if (m == "OFF") {
    ledLigado = false;
    digitalWrite(LEDPIN, LOW);
    publicaEstado();
  }
}

void conectaWiFi() {
  WiFi.begin(ssid, "", 6);
  while (WiFi.status() != WL_CONNECTED) delay(250);
  Serial.println("Wi-Fi OK");
}

void conectaMQTT() {
  while (!mqtt.connected()) {
    String id = "airguard-upm-d2-" + String(random(0xffff), HEX);
    if (mqtt.connect(id.c_str())) {
      mqtt.subscribe(topicoComando, 1);
      publicaEstado();
      Serial.println("MQTT OK (" + id + ")");
    } else {
      Serial.print("Falha MQTT, estado: ");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  conectaWiFi();
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(callback);
  mqtt.setKeepAlive(20);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) conectaWiFi();
  if (!mqtt.connected()) {
    Serial.print("Desconectado, estado=");
    Serial.println(mqtt.state());
    conectaMQTT();
  }
  mqtt.loop();
}