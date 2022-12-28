#include <Arduino.h>
#include <LoRa.h>
#include <arduinoFFT.h>
#include <Adafruit_BMP085.h> 

#define soundPin A0
#define turbPin A1
#define cloroPin A2

#define samples 128
 
Adafruit_BMP085 bmp; 

double vReal[samples];
double vImag[samples];

volatile uint16_t count;
volatile bool stop;

int turb;
int cloro;

arduinoFFT FFT;

ISR(ADC_vect){
  uint16_t m = ADCL; // fetch adc data
  uint16_t j = ADCH;

  double k = (double) (j << 8 | m);

  vReal[count++] = k;

  if (count >= samples){
    ADCSRA = 0b10000110; // interrupt enable; prescaler = 64 -> ADC clock = 250KHz
    stop = true;
  }
}

EMPTY_INTERRUPT (TIMER1_COMPB_vect)

void setup() {
  TCCR1A  = 0;
  TCCR1B  = 0;
  TCNT1   = 0;
  TCCR1B  |= (1 << CS11) | (1 << WGM12);  // CTC, prescaler of 8 -> clock = 2MHz
  TIMSK1  = bit (OCIE1B); 
  OCR1A   = 0x85;
  OCR1B   = 0x85; // = 133dec 2MHz/133 = 15.037KHz

  Serial.begin(115200);
  Serial.println("      /\n    _/./\n ,-'    `-:..-'/\n: o )      _  (\n\"`-....,--; `-.\\\n_______________\nScuba Thing v0.1\n");

  if(!bmp.begin())
    Serial.println("BMP180 não encontrado...");

  if(!LoRa.begin(433e6)){
    Serial.println("LoRa não encontrado...");
    while(1);
  }

  LoRa.setCodingRate4(5);
  LoRa.setSignalBandwidth(62.5e3);
  LoRa.setSyncWord(42);
  LoRa.setSpreadingFactor(8);
  LoRa.setPreambleLength(8);
  LoRa.setGain(6);
  LoRa.setTxPower(20);
  LoRa.enableCrc();
}

void loop() {
  count = 0;
  stop = false;

  for (int i = 0; i < samples; i++)
    vImag[i] = 0;

  ADMUX = (1 << REFS0) | ((soundPin-14) & 0x07);	
  ADCSRB = 0b00000101; // auto trigger source = timer compare B
  ADCSRA = 0b10111110; // interrupt enable; prescaler = 64 -> ADC clock = 250KHz

  while (!stop); 
  
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, samples);

  double freq =  FFT.MajorPeak(vReal, samples, 15037);

  float temp = bmp.readTemperature();

  int pres = bmp.readPressure();

  turb = analogRead(turbPin);
  float v = 5.0 * (turb/1023.0);
  if (v < 2.5)
    turb = 3000;
  else if (v > 4.2)
    turb = 0;
  else
    turb = (int) -1120.4*v*v + 5742.3*v - 4352.9;

  cloro = analogRead(cloroPin);

  Serial.println("\nEnviando... (freq, temp, pres, turb, clor)");

  LoRa.beginPacket();

    LoRa.println(freq);
    Serial.println(freq);

    LoRa.println(temp);
    Serial.println(temp);

    LoRa.println(pres);
    Serial.println(pres);

    LoRa.println(turb);
    Serial.println(turb);

    LoRa.println(cloro);
    Serial.println(cloro);

  LoRa.endPacket(false);
  
  delay(5000); 
}