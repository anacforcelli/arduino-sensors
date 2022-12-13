#define BLYNK_DEVICE_NAME "Lab Embarcados"
#define BLYNK_AUTH_TOKEN "aTBeF3LuSot_3SZcaq_E6oe5IMUcsxRi"

#define BLYNK_PRINT Serial

#include <LoRa.h>
#include <ESP32WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "LAB_DIGITAL";
char pass[] = "C1-17*2018@labdig";

BlynkTimer timer;

BLYNK_CONNECTED() {}

void onReceive(int packetSize)
{
	float freq = LoRa.parseFloat();
	float temp = LoRa.parseFloat();
	int pres = LoRa.parseInt();
	int turb = LoRa.parseInt();
	int cloro = LoRa.parseInt();

	Serial.println("Recebeu pacote: ");
	Serial.println(freq);
	Serial.println(temp);
	Serial.println(pres);
	Serial.println(turb);
	Serial.println(cloro);
	Serial.print("RSSI: ");
	Serial.println(LoRa.packetRssi());

	// codigo visor

	Blynk.virtualWrite(V2, temp);
	Blynk.virtualWrite(V6, pres);
	Blynk.virtualWrite(V8, freq);
	Blynk.virtualWrite(V1, freq);
	Blynk.virtualWrite(V7, cloro);
	Blynk.virtualWrite(V4, turb);
}

void setup()
{
	Serial.begin(9600);

	if(!LoRa.begin(433e6))
		Serial.println("LoRa nao encontrado");

	Blynk.begin(auth, ssid, pass);
	LoRa.onReceive(onReceive);
	LoRa.receive();
}

void loop()
{
	Blynk.run();
	Blynk.virtualWrite(V5, 1); // turned on
}
