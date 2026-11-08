/************************************************************
 * Simple Test Smart Water Tank + Blynk
 * Fitur:
 * 1. Baca level air pakai HC-SR04
 * 2. Kirim level dan jarak ke Blynk
 * 3. Tombol Blynk V5 bisa buka/tutup solenoid valve manual
 ************************************************************/

#define BLYNK_TEMPLATE_ID "TMPL6UKkpKM1B"
#define BLYNK_TEMPLATE_NAME "SiTandon"
#define BLYNK_AUTH_TOKEN "dxmfYm_ht5y0ToMPJl0cDkryNQVrEr4M"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

/* ===== WiFi ===== */
char ssid[] = "jeko";
char pass[] = "55555555";

/* ===== Pin Hardware ===== */
const int PIN_TRIG  = 14;   // TRIG HC-SR04
const int PIN_ECHO  = 35;   // ECHO HC-SR04 via voltage divider 1K + 2K
const int PIN_VALVE = 12;   // GPIO ke resistor -> base transistor

/* ===== Virtual Pin Blynk ===== */
#define VP_LEVEL   V0   // level air %
#define VP_DIST    V1   // jarak cm
#define VP_VALVE   V2   // status valve
#define VP_BUTTON  V5   // tombol buka/tutup valve

/* ===== Kalibrasi Level =====
   Untuk prototype tanpa air, nilai ini bebas dulu.
   DIST_EMPTY = jarak saat dianggap kosong / 0%
   DIST_FULL  = jarak saat dianggap penuh / 100%

   Contoh:
   - kalau sensor ke objek jauh 30 cm = 0%
   - kalau sensor ke objek dekat 5 cm = 100%
*/
float DIST_EMPTY = 9.5;
float DIST_FULL  = 0.5;

BlynkTimer timer;

bool valveOpen = false;

/* ===== Fungsi Valve ===== */
void setValve(bool open) {
  valveOpen = open;

  digitalWrite(PIN_VALVE, open ? HIGH : LOW);

  Blynk.virtualWrite(VP_VALVE, open ? 1 : 0);

  Serial.print("Valve: ");
  Serial.println(open ? "ON / TERBUKA" : "OFF / TERTUTUP");
}

/* ===== Baca Jarak HC-SR04 ===== */
float readDistanceOnce() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH, 30000UL);

  if (duration == 0) {
    return -1;
  }

  float distance = duration * 0.0343 / 2.0;
  return distance;
}

/* ===== Median Filter Simple ===== */
float readDistanceMedian() {
  const int N = 5;
  float data[N];
  int valid = 0;

  for (int i = 0; i < N; i++) {
    float d = readDistanceOnce();

    if (d > 0 && d < 400) {
      data[valid] = d;
      valid++;
    }

    delay(30);
  }

  if (valid == 0) {
    return -1;
  }

  for (int i = 0; i < valid - 1; i++) {
    for (int j = i + 1; j < valid; j++) {
      if (data[j] < data[i]) {
        float temp = data[i];
        data[i] = data[j];
        data[j] = temp;
      }
    }
  }

  return data[valid / 2];
}

/* ===== Ubah Jarak ke Persen ===== */
float distanceToPercent(float dist) {
  if (dist < 0) return -1;

  float percent = (DIST_EMPTY - dist) / (DIST_EMPTY - DIST_FULL) * 100.0;

  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;

  return percent;
}

/* ===== Kirim Sensor ke Blynk ===== */
void sendSensorToBlynk() {
  float distance = readDistanceMedian();
  float level = distanceToPercent(distance);

  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.print(" cm | Level: ");
  Serial.print(level);
  Serial.print(" % | Valve: ");
  Serial.println(valveOpen ? "ON" : "OFF");

  Blynk.virtualWrite(VP_DIST, distance);
  Blynk.virtualWrite(VP_LEVEL, level);
  Blynk.virtualWrite(VP_VALVE, valveOpen ? 1 : 0);
}

/* ===== Tombol Blynk V5 =====
   V5 = 1 -> buka valve
   V5 = 0 -> tutup valve
*/
BLYNK_WRITE(VP_BUTTON) {
  int value = param.asInt();

  if (value == 1) {
    setValve(true);
  } else {
    setValve(false);
  }
}

/* ===== Saat Blynk Connect ===== */
BLYNK_CONNECTED() {
  Blynk.syncVirtual(VP_BUTTON);
  Blynk.virtualWrite(VP_VALVE, valveOpen ? 1 : 0);
}

/* ===== Setup ===== */
void setup() {
  Serial.begin(115200);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_VALVE, OUTPUT);

  digitalWrite(PIN_VALVE, LOW);

  Serial.println("Simple Blynk Test Smart Water Tank dimulai...");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, sendSensorToBlynk);
}

/* ===== Loop ===== */
void loop() {
  Blynk.run();
  timer.run();
}