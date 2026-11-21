#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <time.h>

#define DHTPIN 15
#define POTPIN 34
DHT dht(DHTPIN, DHT22);

const char* ssid   = "Wokwi-GUEST";
const char* broker = "broker.hivemq.com";

const char* topicoTelem  = "airguard-upm/d1/telemetria";
const char* topicoStatus = "airguard-upm/d1/status";

WiFiClient espClient;
PubSubClient mqtt(espClient);
unsigned long ultimo = 0;

void conectaWiFi() {
  WiFi.begin(ssid, "", 6);
  while (WiFi.status() != WL_CONNECTED) delay(250);
  Serial.println("Wi-Fi OK");
}

void conectaMQTT() {
  while (!mqtt.connected()) {
    String id = "airguard-upm-d1-" + String(random(0xffff), HEX);
    // Last Will: se o dispositivo cair, o broker publica "offline"
    if (mqtt.connect(id.c_str(), nullptr, nullptr, topicoStatus, 1, true, "offline")) {
      mqtt.publish(topicoStatus, "online", true);
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
  dht.begin();
  conectaWiFi();
  configTime(-3 * 3600, 0, "pool.ntp.org");
  mqtt.setServer(broker, 1883);
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

  if (millis() - ultimo >= 10000) {
    ultimo = millis();
    float t = dht.readTemperature();
    float u = dht.readHumidity();
    int gas = analogRead(POTPIN);   // 0 a 4095 (potenciômetro simula o sensor de gás)

    if (isnan(t) || isnan(u)) {
      Serial.println("Falha no DHT22");
      return;
    }

    char msg[128];
    snprintf(msg, sizeof(msg),
             "{\"temperatura\":%.1f,\"umidade\":%.1f,\"gas\":%d,\"ts\":%lu}",
             t, u, gas, (unsigned long)time(nullptr));

    bool ok = mqtt.publish(topicoTelem, msg);
    Serial.print(ok ? "Publicado: " : "ERRO ao publicar: ");
    Serial.println(msg);
  }
}