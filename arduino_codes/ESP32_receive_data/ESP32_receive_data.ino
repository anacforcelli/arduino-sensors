#define BLYNK_TEMPLATE_ID "TMPL99tvUYhG"
#define BLYNK_DEVICE_NAME "TCC"
#define BLYNK_AUTH_TOKEN "HIexcU1Lnm0I9jxBV4SLzzEvgnZ6eyf7"

#define BLYNK_PRINT Serial

#include <LoRa.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "ANA";
char pass[] = "AbEF_1CGst9";


void setup() {
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);

  LoRa.setPins(5, 17, 16);

  if (!LoRa.begin(433e6)) {
    Serial.println("LoRa não encontrado");
    while (1);
  }

  LoRa.setCodingRate4(5);
  LoRa.setSignalBandwidth(62.5e3);
  LoRa.setSyncWord(42);
  LoRa.setSpreadingFactor(8);
  LoRa.setPreambleLength(8);
  LoRa.setGain(6);
  LoRa.enableCrc();
}

void loop() {
  Blynk.run();
  Blynk.virtualWrite(V5, 1);

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    float freq = LoRa.parseFloat();
    float temp = LoRa.parseFloat();
    int pres   = LoRa.parseInt();
    int turb   = LoRa.parseInt();
    int cloro  = LoRa.parseInt();

    Serial.println("Recebeu pacote! (freq, temp, pres, turb, clor)");
    Serial.println(freq);
    Serial.println(temp);
    Serial.println(pres);
    Serial.println(turb);
    Serial.println(cloro);
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
    Serial.print("SNR:  ");
    Serial.println(LoRa.packetSnr());

    Blynk.virtualWrite(V2, temp);
    Blynk.virtualWrite(V6, pres);
    Blynk.virtualWrite(V8, freq);
    Blynk.virtualWrite(V1, freq);
    Blynk.virtualWrite(V7, cloro);
    Blynk.virtualWrite(V4, turb);
  }
}
