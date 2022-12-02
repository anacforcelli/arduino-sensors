//Transmitter (Tx) Arduino Board Code

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}
void send_data(long int temp, long int pres, long int freq, long int cloro, long int tur) {
  Serial.print("t\n");
  Serial.println(temp);
  delay(50);
  Serial.print("p\n");
  Serial.println(pres);
  delay(50);
  Serial.print("f\n");
  Serial.println(freq);
  delay(50);
  Serial.print("c\n");
  Serial.println(cloro);
  delay(50);
  Serial.print("u\n");
  Serial.println(tur);
  delay(50);
  Serial.print("s\n");
  Serial.println(temp + pres + freq + cloro + tur);
  delay(50);
}
void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("t2205|p101325|f7013|c708|u208|s111459|");
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("t2505|p102325|f8013|c808|u908|s114559|");
  delay(2000);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("t2505|p102325|f8013|c808|u908|s114558|");
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("t2105|p101325|f8043|c798|u888|s113159|");
  delay(2000);
}
