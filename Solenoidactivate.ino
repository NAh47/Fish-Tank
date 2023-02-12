void setup() {
Serial.begin(9600);
 
}

void loop() {
  int NaSolenoidpin = 5;
  int H2OSolenoidpin = 6;
  pinMode(NaSolenoidpin, OUTPUT);
  pinMode(H2OSolenoidpin, OUTPUT);
  int timeopen = 1000;
  int count = 1;
  if(count = 1){
    Solenoidactivate(NaSolenoidpin, timeopen);
    count = count + 1;
  }
  if(count = 2){  
    Solenoidactivate(H2OSolenoidpin, timeopen);
    count = count - 1;
  }

}


void Solenoidactivate(int powerpin, int(duration)) {
  digitalWrite(powerpin, HIGH);
  delay (duration);
  digitalWrite(powerpin, LOW);

}
