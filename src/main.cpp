#include <Arduino.h>
#include <arduinoFFT.h>
//#include <FHT.h>

#define micPin A0
#define samples 128

volatile uint8_t adcSelector;
double vReal[samples];
double vImag[samples];
volatile uint8_t i;
volatile bool stop;

 arduinoFFT FFT;

ISR(ADC_vect){/* 
  byte m = ADCL; // fetch adc data
  byte j = ADCH;
  int k = (j << 8) | m; // form into an int
  k -= 0x0200; // form into a signed int
  k <<= 6; // form into a 16b signed int
  fht_input[i] = k; // put real data into bins */
  vReal[i++] = (double)ADCL + (ADCH << 8);
  if (i >= samples){
    ADCSRA = 0;  
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
}

void loop() {
  for(uint8_t j = 0; j < samples; j++)
    vImag[j] = 0.0;

  i = 0;
  stop = false;

	ADMUX = (1 << REFS0) | ((micPin-14) & 0x07);	
  ADCSRB = 0b00000101;
  /*         RARRRAAA
              C   DDD
              M   TTT
              E   SSS
                  012
  ACME = ?
  ADTS = TRIGGER SOURCE (101 = timer 1 compare B)    */

  //Serial.println("Start converting");
  ADCSRA = 0b10111110;
  /*         AAAAAAAA
             DDDDDDDD
             ESAIIPPP
             NCTFESSS
               E  012
  ADEN = ENABLE
  ADSC = SINGLE CONVERSION (MUST BE SET ON FIRST FREE-RUNNING CONVERSION)
  ADATE = AUTO TRIGGER ENABLE (TRIGGER ON SOURCE GIVEN BY ADCSRB'S ADTS BITS)
  ADIF = INTERRUPT FLAG
  ADIE = INTERRUPT ENABLE
  ADPS[2:0] = PRESCALER (111 DIVIDES CLOCK SOURCE BY 128 -> 16M/128 = 125KHz ADC CLOCK) -> 9.615KS/s */

  /* WAITS FOR CONVERSIONS TO COMPLETE*/
  while (!stop);
  
/* 
  fht_reorder(); // reorder the data before doing the fht
  fht_run(); // process the data in the fht

  fht_mag_log();
  Serial.print("start\n");
  for (int g = 0; g < samples/2; g++){
    Serial.print(fht_log_out[g]);
    Serial.print('\n');
  } */

  FFT.DCRemoval(vReal, samples);
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD); 
  FFT.ComplexToMagnitude(vReal, vImag, samples);

  Serial.println('A');
  for(int g = 0; g < samples; g++)
    Serial.println(hypot(vReal[g], vImag[g]));

}