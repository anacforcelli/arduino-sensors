#include <Arduino.h>
#include <FHT.h>
#include <Adafruit_BMP085.h> //INCLUSÃO DE BIBLIOTECA

#define soundPin A0
#define turbPin A1
#define cloroPin A2
#define samples FHT_N
 
Adafruit_BMP085 bmp; //OBJETO DO TIPO Adafruit_BMP085 (I2C)

volatile uint16_t count;
volatile bool stop;
volatile char reading;

volatile float turb;
volatile float cloro;

ISR(ADC_vect){
  byte m = ADCL; // fetch adc data
  byte j = ADCH;

  int k = (j << 8) | m; // form into an int
  k -= 0x0200; // form into a signed int
  k <<= 6; // form into a 16b signed int

  if (reading == 'f'){
    fht_input[count++] = k; // put real data into bins
    if (count >= FHT_N){
      ADCSRA = 0;  
      stop = true;
    }
  } else if (reading == 'u') {
    turb = k;
  } else if (reading == 'c') {
    cloro = k;
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
	int maxY = 0;
	int ind = 0;
	//If sampling_frequency = 2 * max_frequency in signal,
	//value would be stored at position samples/2
	for (int i = 1; i < ((samples / 2)); i++) {
		if ((fht_lin_out[i-1] < fht_lin_out[i]) && (fht_lin_out[i] > fht_lin_out[i+1])) {
			if ((int)fht_lin_out[i] > maxY) {
				maxY = fht_lin_out[i];
				ind = i;
			}
		}
	}

	float delta = 0.5 * (fht_lin_out[ind-1] - fht_lin_out[ind+1]) / (fht_lin_out[ind-1] - 2.0 * fht_lin_out[ind] + fht_lin_out[ind+1]);
	float interpolatedX = ((ind + delta)  * samplingFrequency) / (samples-1);
	if (ind == samples >> 1) //To improve calculation on edge values
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

  Serial.begin(115200);
}

void loop() {
  count = 0;
  stop = false;

  ADMUX = (1 << REFS0) | ((soundPin-14) & 0x07);	
  ADCSRB = 0b00000101;
  ADCSRA = 0b10111110;

  while (!stop);  
  fht_reorder(); // reorder the data before doing the fht
  removeDC();
  fht_run(); // process the data in the fht
  fht_mag_lin();

  reading = 'c';
  ADMUX |=  ((cloroPin-14) & 0x07);
  ADCSRA |= (1 << ADSC);
  while (ADSC != 0);

  reading = 'u';
  ADMUX |=  ((turbPin-14) & 0x07);
  ADCSRA |= (1 << ADSC);
  while (ADSC != 0);

  float freq = getMajorFrequency(15037);
  Serial.print("f|");
  Serial.print(freq);
  Serial.print("t|");
  Serial.print(bmp.readTemperature());
  Serial.print("p|");
  Serial.print(bmp.readPressure());
  Serial.print("u|");
  Serial.print(turb);
  Serial.print("c|");
  Serial.print(cloro);
}