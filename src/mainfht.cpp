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
	uint16_t maxY = 0;
	int ind = 0;
	for (int i = 1; i < (samples / 2); i++) {
		if ((fht_lin_out[i-1] < fht_lin_out[i]) && (fht_lin_out[i] > fht_lin_out[i+1])) {
			if (fht_lin_out[i] > maxY) {
				maxY = fht_lin_out[i];
				ind = i;
			}
		}
	}

	float delta = 0.5 * (fht_lin_out[ind-1] - fht_lin_out[ind+1]) / (fht_lin_out[ind-1] - 2.0 * fht_lin_out[ind] + fht_lin_out[ind+1]);
	float interpolatedX = ((ind + delta)  * samplingFrequency) / (samples-1);
	if (ind == (samples >> 1)) //To improve calculation on edge values
		interpolatedX = ((ind + delta)  * samplingFrequency) / samples;
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

  int temp = bmp.readTemperature();
  int pressure = bmp.readPressure();

  ADMUX = (1 << REFS0) | ((cloroPin-14) & 0x07);
  ADCSRA |= (1 << ADSC);
  while (ADSC == 1);
  turb = (int) (ADCL | ADCH << 8);

  ADMUX = (1 << REFS0) | ((turbPin-14) & 0x07);
  ADCSRA |= (1 << ADSC);
  while (ADSC == 1);
  cloro = (int) (ADCL | ADCH << 8);

  Serial.print("t");
  Serial.print(temp*100);
  Serial.print("|");
  Serial.print("p");
  Serial.print(-pressure*100);
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
  Serial.print(temp*100-pressure*100+(int)freq+cloro+turb);
  Serial.print("|");
  Serial.println("");
  delay(5000);
}