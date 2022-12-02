#include <Wire.h> //INCLUSÃO DE BIBLIOTECA
#include <Adafruit_BMP085.h> //INCLUSÃO DE BIBLIOTECA
 
Adafruit_BMP085 bmp; //OBJETO DO TIPO Adafruit_BMP085 (I2C)
int temt6000Pin = 3;
int micPin = 2;
int turbidityPin = 0;
int temp = 0;
int pres = 0;


void setup(){
  Serial.begin(9600); //INICIALIZA A SERIAL
  //if (!bmp.begin()){ //SE O SENSOR NÃO FOR INICIALIZADO, FAZ
  //Serial.println("Sensor BMP180 não foi identificado! Verifique as conexões."); //IMPRIME O TEXTO NO MONITOR SERIAL
  //while(1){} //SEMPRE ENTRE NO LOOP
  //}
}
   
void loop(){

    Serial.print("t");
    //temp = bmp.readTemperature() * 100;
    Serial.print(temp);
    Serial.print("|");
    Serial.print("p");
    //pres = bmp.readPressure() * 100;
    Serial.print(pres);
    Serial.print("|");
    Serial.print("t");
    int mic = analogRead(micPin*100); // mic
    Serial.print(mic);
    Serial.print("|");
    Serial.print("c");
    int lumes = analogRead(temt6000Pin)*100; // lumes
    Serial.print(lumes);
    Serial.print("|");
    Serial.print("u");
    int turbidity = analogRead(A0) * (5.0 / 1024.0)*100;
    Serial.print(turbidity);
    Serial.print("|");
    Serial.print("s");
    Serial.print(temp+pres+mic+lumes+turbidity);
    Serial.print("|");
    Serial.println("");
    delay(1000);
}
