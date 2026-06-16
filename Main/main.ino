#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

Adafruit_MPU6050 mpu;
Servo meuServo;

const int servoPin = 18;
const int potPin = 4;
int valorPot = 0;

const float limiteInclinacao = 20.0; // graus
const float limiteDeformacao = 70.0;

float anguloX;
float deformacao;


void setup() {
  Serial.begin(115200);

  Wire.begin();

  if (!mpu.begin()) {
    Serial.println("MPU6050 nao encontrado!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 iniciado!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  meuServo.setPeriodHertz(50);
  meuServo.attach(servoPin, 500, 2400);

  // posição inicial
  meuServo.write(90);

  delay(1000);
}

void loop() {

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /// Calcula inclinação da ponte
  anguloX = atan2(a.acceleration.x, a.acceleration.z) * 180.0 / PI;

  // ===== LEITURA DO POTENCIÔMETRO =====
  valorPot = analogRead(potPin);

  // Simula deformação de 0 a 100%
  deformacao = (valorPot / 4095.0) * 100.0;

  // ===== DECISÃO =====
  bool riscoInclinacao = abs(anguloX) > limiteInclinacao;
  bool riscoDeformacao = deformacao > limiteDeformacao;

  if (riscoInclinacao || riscoDeformacao) {
    // Fecha a cancela
    meuServo.write(0);
  } else {
    // Abre a cancela
    meuServo.write(90);
  }
// ===== MONITOR SERIAL =====
  Serial.print("Inclinacao: ");
  Serial.print(anguloX);
  Serial.println(" graus");

  Serial.print("Potenciometro: ");
  Serial.println(valorPot);

  Serial.print("Deformacao simulada: ");
  Serial.print(deformacao);
  Serial.println(" %");

  Serial.print("Status da ponte: ");

  if (riscoInclinacao || riscoDeformacao) {
    Serial.println("RISCO - CANCELA FECHADA");
  } else {
    Serial.println("NORMAL - CANCELA ABERTA");
  }

  delay(500);
}