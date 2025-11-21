#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// ====== CONFIG WI-FI / MQTT ======
const char* WIFI_SSID     = "Wokwi-GUEST";  // se testar em casa, troque pelo seu
const char* WIFI_PASSWORD = "";             // senha do seu Wi-Fi (no Wokwi-GUEST é vazia)

const char* MQTT_BROKER   = "broker.hivemq.com";
const int   MQTT_PORT     = 1883;
const char* MQTT_CLIENT_ID = "smart-aires-esp32-demo";
const char* MQTT_TOPIC_DATA = "smartaires/ambiente";
const char* MQTT_TOPIC_CMD  = "smartaires/cmd/led";

// ====== PINOS ======
#define DHTPIN   2     // pino 2 (lado direito da placa)
#define DHTTYPE  DHT22

const int GAS_PIN  = 34;   // MQ-2 AO -> 34
const int LED_R_PIN = 12;  // RGB R
const int LED_G_PIN = 13;  // RGB G
const int LED_B_PIN = 14;  // RGB B

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

String statusQualidade = "Boa";

// ====== FUNÇÕES AUXILIARES ======
void setLedColor(bool r, bool g, bool b) {
  digitalWrite(LED_R_PIN, r ? HIGH : LOW);
  digitalWrite(LED_G_PIN, g ? HIGH : LOW);
  digitalWrite(LED_B_PIN, b ? HIGH : LOW);
}

void atualizaLedPorQualidade(float mqPPM) {
  if (mqPPM < 200) {
    statusQualidade = "Boa";
    setLedColor(false, true, false);   // verde
  } else if (mqPPM < 500) {
    statusQualidade = "Moderada";
    setLedColor(true, true, false);    // amarelo
  } else {
    statusQualidade = "Ruim";
    setLedColor(true, false, false);   // vermelho
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida em [");
  Serial.print(topic);
  Serial.print("]: ");

  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);

  // Comandos para LED no tópico smartaires/cmd/led
  if (String(topic) == MQTT_TOPIC_CMD) {
    if (msg == "verde")      setLedColor(false, true, false);
    else if (msg == "amarelo") setLedColor(true, true, false);
    else if (msg == "vermelho") setLedColor(true, false, false);
    else if (msg == "off")     setLedColor(false, false, false);
  }
}

void conectaWiFi() {
  Serial.print("Conectando-se a ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnectMqtt() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect(MQTT_CLIENT_ID)) {
      Serial.println("conectado!");
      client.subscribe(MQTT_TOPIC_CMD);
    } else {
      Serial.print("falhou (rc=");
      Serial.print(client.state());
      Serial.println("). Tentando de novo em 5s...");
      delay(5000);
    }
  }
}

// ====== SETUP / LOOP ======
unsigned long lastMsg = 0;
const long intervaloLeitura = 2000; // 2 segundos

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Smart-Aires...");

  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  setLedColor(false, false, false);

  dht.begin();
  conectaWiFi();

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(mqttCallback);
}

void loop() {
  if (!client.connected()) {
    reconnectMqtt();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > intervaloLeitura) {
    lastMsg = now;

    float temperatura = dht.readTemperature();
    float umidade     = dht.readHumidity();

    int gasRaw        = analogRead(GAS_PIN);
    float mqPPM       = map(gasRaw, 0, 4095, 0, 600); // aproximação simples

    if (isnan(temperatura) || isnan(umidade)) {
      Serial.println("Falha na leitura do DHT22");
      return;
    }

    atualizaLedPorQualidade(mqPPM);

    char payload[200];
    snprintf(payload, sizeof(payload),
             "{\"temperatura\": %.2f, \"umidade\": %.2f, \"qualidade_ar\": %.2f, \"status\": \"%s\"}",
             temperatura, umidade, mqPPM, statusQualidade.c_str());

    Serial.print("Publicando em ");
    Serial.print(MQTT_TOPIC_DATA);
    Serial.print(": ");
    Serial.println(payload);

    client.publish(MQTT_TOPIC_DATA, payload);
  }
}
