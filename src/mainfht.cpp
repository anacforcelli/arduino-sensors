#include <Arduino.h>
#include <FHT.h>
#include <Adafruit_BMP085.h> 

#define soundPin A0
#define turbPin A1
#define cloroPin A2
#define samples FHT_N
 
Adafruit_BMP085 bmp; 

volatile uint16_t count;
volatile bool stop;

int turb;
int cloro;

ISR(ADC_vect){
  byte m = ADCL; // fetch adc data
  byte j = ADCH;

  int k = (j << 8) | m; // form into an int
  k -= 0x0200; // form into a signed int
  k <<= 6; // form into a 16b signed int

  fht_input[count++] = k;

  if (count >= samples){
    ADCSRA = 0b10000111;   //ADSC = 0; ADIE = 0 -> disable interrupt; prescaler = 128 -> adc clock = 125kHz
    stop = true;
  }
}

EMPTY_INTERRUPT (TIMER1_COMPB_vect)

void removeDC(){
  int mean = 0;
  for (int i = 0; i < samples; i++)
    mean += fht_input[i];
  mean = mean / samples;
  for (int i = 0; i < samples; i++)
    fht_input[i] -= mean;
}


float getMajorFrequency(int samplingFrequency)
{
	double maxY = 0;
	uint16_t IndexOfMaxY = 0;
	//If sampling_frequency = 2 * max_frequency in signal,
	//value would be stored at position samples/2
	for (uint16_t i = 1; i < ((samples >> 1)); i++) {
		if ((fht_lin_out[i-1] < fht_lin_out[i]) && (fht_lin_out[i] > fht_lin_out[i+1])) {
			if (fht_lin_out[i] > maxY) {
				maxY = fht_lin_out[i];
				IndexOfMaxY = i;
			}
		}
	}
	double delta = 0.5 * ((fht_lin_out[IndexOfMaxY-1] - fht_lin_out[IndexOfMaxY+1]) / (fht_lin_out[IndexOfMaxY-1] - (2.0 * fht_lin_out[IndexOfMaxY]) + fht_lin_out[IndexOfMaxY+1]));
	double interpolatedX = ((IndexOfMaxY + delta)  * samplingFrequency) / (samples-1);
	if(IndexOfMaxY==(samples >> 1)) //To improve calculation on edge values
		interpolatedX = ((IndexOfMaxY + delta)  * samplingFrequency) / (samples);
	// returned value: interpolated frequency peak apex
	return(interpolatedX);
}

void setup() {
  TCCR1A  = 0;
  TCCR1B  = 0;
  TCNT1   = 0;
  TCCR1B  |= (1 << CS11) | (1 << WGM12);  // CTC, prescaler of 8 -> clock = 2MHz
  TIMSK1  = bit (OCIE1B); 
  OCR1A   = 0x85;
  OCR1B   = 0x85; // = 133dec 2MHz/133 = 15.037KHz

  Serial.begin(9600);

  if(!bmp.begin())
    Serial.println("bmp180 não encontrado");
}

void loop() {
  count = 0;
  stop = false;

  ADMUX = (1 << REFS0) | ((soundPin-14) & 0x07);	
  ADCSRB = 0b00000101; // auto trigger source = timer compare B
  ADCSRA = 0b10111110; // interrupt enable; prescaler = 64 -> ADC clock = 250KHz

  while (!stop);  
  fht_reorder(); 
  removeDC();
  fht_run();
  fht_mag_lin();

  float freq = getMajorFrequency(15037);

  float temp = bmp.readTemperature();

  int pressure = bmp.readPressure();

  turb = analogRead(turbPin);
  float v = 5.0 * (turb/1023.0);
  turb = (int) -1120.4*v*v + 5742.3 * v - 4352.9;

  cloro = analogRead(cloroPin);

  Serial.print("t");
  Serial.print(int(temp*100));
  Serial.print("|");
  Serial.print("p");
  Serial.print(pressure*100);
  Serial.print("|");
  Serial.print("f");
  Serial.print((int)freq);
  Serial.print("|");
  Serial.print("c");
  Serial.print(cloro);
  Serial.print("|");
  Serial.print("u");
  Serial.print(turb);
  Serial.print("|");
  Serial.print("s");
  Serial.print(int(temp*100)+pressure*100+(int)freq+cloro+turb);
  Serial.print("|");
  Serial.println("");
  delay(1000); 
}