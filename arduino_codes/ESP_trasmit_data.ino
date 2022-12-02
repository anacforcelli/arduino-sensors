/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLzdKe3VAJ"
#define BLYNK_DEVICE_NAME "Lab Embarcados"
#define BLYNK_AUTH_TOKEN "h2Vg_olm70X1hJ7qmQ81gUM2GQFu2f4H"

struct Sensor_data {
  long int temp;
  long int pres;
  long int freq;
  long int cloro;
  long int tur;
  bool check;
};


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

// Include
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "WIFI";
char pass[] = "PASSWORD";
int incomingByte = 0;
char buffer[10];
float temperature;
Sensor_data raw;

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes

BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  //int value = param.asInt();

  // Update state
  //Blynk.virtualWrite(V1, value);
  //Serial.write(V0);
  //digitalWrite(LED_BUILTIN, V0);
}
// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
}
int parcer_data(String raw_data, char charac){
  int data = 0;
  char test = ' ';
  int i = 0;
  long int temp = 0; 
  int sum = 0;
  while(i < raw_data.length()){
    if(raw_data[i] == charac){
      temp = 0;
      i = i + 1;
      while(raw_data[i] != '|'){
        temp = temp*10 + int(raw_data[i]) - '0';
        i = i + 1; 
      }
      data = temp;
    }
    i = i + 1;
  }
  return(data);
}

Sensor_data parcer_string(String raw_data){
  long int sum = 0;
  long int local_sum = -1;
  Sensor_data sensors = {0,0,0,0,0,false};

  
  sensors.temp = parcer_data(raw_data,'t');
  sensors.pres = parcer_data(raw_data,'p');
  sensors.freq = parcer_data(raw_data,'f');
  sensors.cloro = parcer_data(raw_data,'c');
  sensors.tur = parcer_data(raw_data,'u');
  sum = parcer_data(raw_data,'s');
  
  local_sum = sensors.temp + sensors.freq + sensors.pres + sensors.cloro + sensors.tur;
  sensors.check = local_sum == sum;
  return(sensors);
} 
// This function sends Arduino's uptime every 5 seconds with Virtual Pin.
void myTimerEvent()
{

  if(Serial.available())
   {
      String serialinput = Serial.readString();
      //Serial.println(serialinput);
      //Serial.println(serialinput.length());
      serialinput.trim();
      //serialinput = Serial.readString(); 
      raw = parcer_string(serialinput);
      //Serial.println(raw.temp);
      if(raw.check){
        Blynk.virtualWrite(V2, raw.temp/100.0);
        Blynk.virtualWrite(V6, raw.pres);
        Blynk.virtualWrite(V8, raw.freq);
        Blynk.virtualWrite(V1, raw.freq);
        Blynk.virtualWrite(V7, raw.cloro);
        Blynk.virtualWrite(V4, raw.tur);
      }
        
      if (serialinput == "1") { digitalWrite(LED_BUILTIN, HIGH); }
      //Serial.println(serialinput);
   }
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT); 

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(5000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  Blynk.virtualWrite(V5, 1);
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
