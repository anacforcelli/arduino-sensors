//Receiver (Rx) Arduino Board Code
 
String serialinput  = "t2205|p101325|f7013|c708|u208|s111459|";
struct Sensor_data {
  long int temp;
  long int pres;
  long int freq;
  long int cloro;
  long int tur;
  bool check;
};

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
void setup()
{
   pinMode(LED_BUILTIN, OUTPUT);
   Serial.begin(9600);
}
void loop()
{

  Sensor_data raw;
  
  delay(1000);

   if(Serial.available())
   {
      String serialinput = Serial.readString();
      //Serial.println(serialinput);
      //Serial.println(serialinput.length());
      serialinput.trim();
      //serialinput = Serial.readString(); 
      raw = parcer_string(serialinput);
      Serial.println(raw.temp);
      Serial.println(raw.pres);
      Serial.println(raw.freq);
      Serial.println(raw.cloro);
      Serial.println(raw.tur);
      Serial.println(raw.check);
        
      if (serialinput == "1") { digitalWrite(LED_BUILTIN, HIGH); }
      //Serial.println(serialinput);
   }
}
