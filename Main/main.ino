#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ===== WiFi =====
const char* ssid = "SEU WIFI";
const char* senha = "SUA SENHA";

// ===== MQTT =====
const char* mqtt_broker = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_topic = "fran/sensores";
const char* mqtt_client_id = "esp32_ponte_";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Adafruit_MPU6050 mpu1;
Adafruit_MPU6050 mpu2;
Servo meuServo;

const int servoPin = 18;

const float limiteInclinacao = 4.0; // graus

unsigned long ultimoEnvio = 0;
const unsigned long intervaloEnvio = 1000; // ms

float offset1 = 0;
float offset2 = 0;

void conectarWiFi() {
  Serial.print("Conectando ao WiFi");
  WiFi.begin(ssid, senha);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado! IP: " + WiFi.localIP().toString());
}

void reconectarMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    String clientId = mqtt_client_id + String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println(" conectado!");
    } else {
      Serial.print(" falhou, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" tentando novamente em 2s");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  Wire.begin();

  if (!mpu1.begin(0x68)) {
  Serial.println("Erro ao iniciar MPU6050 1");
  while (1);
}

if (!mpu2.begin(0x69)) {
  Serial.println("Erro ao iniciar MPU6050 2");
  while (1);
}

  Serial.println("MPUs6050 iniciado!");

  mpu1.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu1.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu1.setFilterBandwidth(MPU6050_BAND_21_HZ);

  mpu2.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu2.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu2.setFilterBandwidth(MPU6050_BAND_21_HZ);

  sensors_event_t a1, g1, t1;
  sensors_event_t a2, g2, t2;
  mpu1.getEvent(&a1, &g1, &t1);
  mpu2.getEvent(&a2, &g2, &t2);

  offset1 = atan2(a1.acceleration.x, a1.acceleration.y) * 180.0 / PI;
  offset2 = atan2(a2.acceleration.x, a2.acceleration.y) * 180.0 / PI;
  meuServo.setPeriodHertz(50);
  meuServo.attach(servoPin, 500, 2400);

  // posição inicial
  meuServo.write(90);

  conectarWiFi();
  mqttClient.setServer(mqtt_broker, mqtt_port);

  delay(1000);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }

  if (!mqttClient.connected()) {
    reconectarMQTT();
  }
  mqttClient.loop();

  sensors_event_t a1, g1, t1;
  sensors_event_t a2, g2, t2;

  mpu1.getEvent(&a1, &g1, &t1);
  mpu2.getEvent(&a2, &g2, &t2);

  /// Calcula inclinação da ponte
  float angulo1 = atan2(a1.acceleration.x, a1.acceleration.y) * 180.0 / PI - offset1;
  float angulo2 = atan2(a2.acceleration.x, a2.acceleration.y) * 180.0 / PI - offset2;
  
  // ===== DECISÃO =====
bool risco = abs(angulo1) > limiteInclinacao || abs(angulo2) > limiteInclinacao;
  if (risco) {
    // Fecha a cancela
    meuServo.write(90);
  } else {
    // Abre a cancela
    meuServo.write(180);
  }
// ===== MONITOR SERIAL =====
  Serial.println("Sensor Estrutural – Pilar 1");
  Serial.print("Inclinacao: ");
  Serial.print(angulo1);
  Serial.println(" graus");

  Serial.println("Sensor Estrutural – Pilar 3");
  Serial.print("Inclinacao: ");
  Serial.print(angulo2);
  Serial.println(" graus");

if (risco) {
    Serial.println("ALERTA: Ponte acima do limite! Cancela fechada");
  } else {
    Serial.println("Cancela Aberta");
  }

  // ===== ENVIO MQTT =====
  if (millis() - ultimoEnvio >= intervaloEnvio) {
    ultimoEnvio = millis();

    String payload = "{";
    payload += "\"angulo1\":" + String(angulo1, 2) + ",";
    payload += "\"angulo2\":" + String(angulo2, 2) + ",";
    payload += "\"risco\":" + String(risco ? "true" : "false") + ",";
    payload += "\"cancela\":\"" + String(risco ? "fechada" : "aberta") + "\"";
    payload += "}";

    mqttClient.publish(mqtt_topic, payload.c_str());
    Serial.println("Publicado: " + payload);
  }

  delay(1000);
}