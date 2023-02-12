// Monitor and regulate salinity and temperature of a system
// Include LCD libraries
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for I2C communication
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // Change to (0x27,16,2) for 16x2 LCD.

//GLOBAL VARIABLES
int salinity_power_pin = 4; // Digital I/O pin
unsigned long salinity_clock; //Deadtime measurement for salinity
unsigned long heater_clock; //Deadtime measurement for temperature
float saltypoint = .11;
void setup() {
  //Arduino PIN Allocation
  pinMode(salinity_power_pin, OUTPUT);
  const int salinity_input_pin = A0; // Analog input pin
  pinMode(salinity_input_pin, INPUT);

  //Display Control Limits
  float salsetpoint, salstddeviation, saldeadband, tsetpoint, tempstddeviation, tdeadband;
  salsetpoint = saltypoint;
  salstddeviation = 25.1574;
  saldeadband = 3 * salstddeviation;  //deadband variable is 1/2 actual deadband for convenience
  tsetpoint = 18;
  tempstddeviation = 2.5;
  tdeadband = 3 * tempstddeviation;
  
  //Serial monitor for troubleshooting
  Serial.begin(9600);

  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
  lcdsetup(salsetpoint, saldeadband, tsetpoint, tdeadband); //User-defined lcd setup function prints non updating data to lcd
/*
  //Wait for User readiness update sensor readings to monitors
  int button_pin = 6; // Digital input to monitor start
  int startclick = LOW;
  while (!startclick) {
    startclick = digitalRead (button_pin);
    
    //Salinity readings
    int n = 15; // number of readings to average
    float salinityread; // Average reading is a float
    salinityread = sensor_reading_ave(salinity_power_pin, salinity_input_pin, n); // Take an average of n readings and return average
    float salinity;
    salinity = saldigitalconv(salinityread);  //Convert average digital salinity reading of (0-1023) to the salinity % range of (0.0-0.56)
    
    //Temperature readings and heater status
    float temperature;
    temperature = 0;
    int heaterstatus;
    heaterstatus = 0;
    
    //Monitor Updates
    lcdupdate(salinity, temperature, heaterstatus);
    Serial.print("Salinity = ");
    Serial.print(salinity);
    Serial.print("Analog Salinity Output");
    Serial.print(salinityread);
    Serial.println("\twaiting for button press");
  }
  //Start Clocks
  salinity_clock = millis();
  heater_clock = millis();
  */
}
void loop() {
  //Arduino PIN Allocation
  const int salinity_input_pin = A0; // Analog input pin
  const int h2o_add_relay = 7; //Digital output to activate fresh water system
  const int salty_add_relay = 8; //Digital output to activate salt water system
  pinMode(salinity_input_pin, INPUT);
  pinMode(h2o_add_relay, OUTPUT);
  pinMode(salty_add_relay, OUTPUT);
  digitalWrite(h2o_add_relay, LOW);
  digitalWrite(salty_add_relay, LOW);
  
  //Declare Control Limits
  float salsetpoint, salstddeviation, saldeadband, sal_lcl, sal_ucl, salty_tank_salinity, tsetpoint, tempstddeviation, tdeadband;
  int saldeadtime, tdeadtime;
  salsetpoint = saltypoint;
  salstddeviation = 25.1574;
  saldeadband = 3 * salstddeviation;  //deadband variable is 1/2 actual deadband for convenience
  sal_lcl = salpercentconv(salsetpoint) - saldeadband; //salinity lower control limit in analog input voltage
  sal_ucl = salpercentconv(salsetpoint) + saldeadband; //salinity upper control limit in analog input voltage
  tempstddeviation = 2.5;
  tdeadband = 3 * tempstddeviation;  //deadband variable is 1/2 actual deadband for convenience
  saldeadtime = 10500;
  tdeadtime = 15000;
  salty_tank_salinity = 0.15;
  
  //Take Salinity Readings and average them to give stable output
  int sal_n = 15; // number of salinity readings to average
  float salinityread; // Average reading is a float
  salinityread = sensor_reading_ave(salinity_power_pin, salinity_input_pin, sal_n); // Take an average of n readings and return average
  float salinity;
  salinity = saldigitalconv(salinityread);  //Convert average digital salinity reading of (0-1023) to the salinity % range of (0.0-0.56)
  
  //Take Temperature Readings
  float temperature;
  temperature = 0;
  int heaterstatus;
  heaterstatus = 0;

  //LCD Display Update
  lcdupdate(salinity, temperature, heaterstatus);

  //SALINITY CORRECTION CONTROLS
  if ((millis() - salinity_clock) > saldeadtime){ //Check for Deadtime
    if(salinityread < sal_lcl){ //Check for low salinity
      salinity_clock = millis(); //Reset the salinity clock for for solenoid open duration
      int salty_open; //Time for solenoid to be open
      salty_open = salty_duration(salinity, salsetpoint, salty_tank_salinity); //Calculate time for solenoid to be open in user-defined function
      while(millis() - salinity_clock < salty_open){ //Open salty solenoid for salty_open duration
      digitalWrite(salty_add_relay, HIGH);
      }
      digitalWrite(salty_add_relay, LOW); //Close salty solenoid
      salinity_clock = millis(); //Reset the salinity clock for deadtime
    }if(salinityread > sal_ucl){ //Check for high salinity
      salinity_clock = millis(); //Reset the salinity clock for for solenoid open duration
      int h2o_open; //Time for solenoid to be open
      h2o_open = h2o_duration(salinity, salsetpoint); //Calculate time for solenoid to be open in user-defined function
      while(millis() - salinity_clock < h2o_open){ //Open h2o solenoid for h2o_open duration
      digitalWrite(h2o_add_relay, HIGH);
      }
      digitalWrite(h2o_add_relay, LOW); //Close salty solenoid
      salinity_clock = millis(); //Reset the salinity clock for deadtime
  }
}
}
// -------------------------------------------------
// Reading Averaging Function
float sensor_reading_ave(int power_pin, int input_pin, int nave) {
  float ave, sum;
  digitalWrite(power_pin, HIGH); // Turn sensor on
  delay (100); // Wait to settle
  sum = 0.0;
  for (int i = 0; i <= nave; i++) {
    sum += analogRead(input_pin); // Accumulate the sum
    delay(10); // Pause, briefly
  }
  digitalWrite(power_pin, LOW); // Turn sensor off
  ave = sum / float(nave); // Compute average
  return ave;
}
//-----------------------------------------------------
//Digital salinity output conversion to Salinity percentage
float saldigitalconv(float salread) {
  float salpercent;
  if (salread < 638.86) {
    salpercent = salread * 0.0000898761686149 - 0.00741828908132;
  } else if (salread >= 638.86) {
    salpercent = salread * 0.00132661183338 - 0.797519235873;
  } return salpercent;
}

// -------------------------------------------------------
//Salinity percentage to digital output converter
float salpercentconv(float percentread) {
  float salinityread;
  if (percentread < 0.05) {
    salinityread = (percentread + 0.00741828908132) / 0.0000898761686149;
  } else if (percentread >= 0.05) {
    salinityread = (percentread + 0.797519235873) / 0.00132661183338;
  } return salinityread;
}

// ----------------------------------------------------------------
//Calculate open time for salinity tank
long int salty_duration(float salpercent, float salinitysetpoint, float salty){
  //Calculate target final salinity after gain
  float gain, salfinal, massadd, masstank, percentretained, flowrate;
  gain = 0.8;
  percentretained = 0.85;
  masstank = 140;
  flowrate = 5;
  salfinal = salinitysetpoint + ((1 - gain)*(salpercent - salinitysetpoint));
  massadd = (1/percentretained) * ((salpercent-salfinal) / (salpercent-salty)) * masstank;
  long int timeopen;
  timeopen = massadd/flowrate;
  return timeopen;
}

// ----------------------------------------------------------------
//Calculate open time for DI h2o tank
long int h2o_duration(float salpercent, float salinitysetpoint){
  //Calculate target final salinity after gain
  float gain, salfinal, massadd, masstank, percentretained, flowrate;
  gain = 0.8;
  percentretained = 0.85;
  masstank = 140;
  flowrate = 5;
  salfinal = salinitysetpoint + ((1 - gain)*(salpercent - salinitysetpoint));
  massadd = (1/percentretained) * ((salpercent-salfinal) / (salpercent)) * masstank;
  long int timeopen;
  timeopen = massadd/flowrate;
  return timeopen;
}
// ------------------------------------------------
// LCD SETUP Function
void lcdsetup(float salinitysetpoint, float salinitydeadband, float tempsetpoint, float tempdeadband) {

  // Prints sensor values to LCD and labels them
  lcd.setCursor(3, 0); // Set the cursor on the first column and first row.
  lcd.print("LCL"); // Prints the text label for colum
  lcd.setCursor(9, 0); // Set the cursor on the first column and first row.
  lcd.print("SP"); // Prints the text label for column
  lcd.setCursor(15, 0); // Set the cursor on the first column and first row.
  lcd.print("UCL"); // Prints the text label for column
  lcd.setCursor(0, 1);
  lcd.print("S:"); // Print row salinity label
  lcd.setCursor(2, 1);
  lcd.print(saldigitalconv(salpercentconv(salinitysetpoint) - salinitydeadband)); //Print salinity lcl
  lcd.setCursor(9, 1);
  lcd.print(salinitysetpoint); //Print salinity setpoint
  lcd.setCursor(15, 1);
  lcd.print(saldigitalconv(salpercentconv(salinitysetpoint) + salinitydeadband)); //Print salinity ucl
  lcd.setCursor(0, 2);
  lcd.print("T:");  //Print row temperature label
  lcd.setCursor(3, 2);
  lcd.print(tempsetpoint - tempdeadband); //Print temperature lcl
  lcd.setCursor(9, 2);
  lcd.print(tempsetpoint); //Print temperature setpoint
  lcd.setCursor(15, 2);
  lcd.print(tempsetpoint + tempdeadband); //Print temperature ucl
  lcd.setCursor(0, 3);
  lcd.print("S="); //Print current salinity label
  lcd.setCursor(8, 3);
  lcd.print("T="); //Print current temperature label
  lcd.setCursor(15, 3);
  lcd.print("H="); //Print heater status label
}

//-------------------------------------------
// LCD update function
void lcdupdate(float salinitydisplay, float tempdisplay, int heatdisplay) {
  lcd.setCursor(3, 3);
  lcd.print(salinitydisplay);
  lcd.setCursor(10, 3);
  lcd.print(tempdisplay);
  lcd.setCursor(17, 3);
  if (heatdisplay = 1) {
    lcd.print("ON");
  } else {
    lcd.print("OFF");
  }
}
