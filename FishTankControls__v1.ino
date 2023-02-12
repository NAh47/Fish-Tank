// Monitor and regulate salinity and temperature of a system
// Include LCD libraries
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for I2C communication
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // Change to (0x27,16,2) for 16x2 LCD.

int salinity_power_pin = 4; // Digital I/O pin, Global variable

void setup() {
  Serial.begin(9600); 
  pinMode(salinity_power_pin, OUTPUT);
  
  // Initiate the LCD:
  lcd.init();
  lcd.backlight();

}
void loop() {
  //Salinity measurement
  int salinity_input_pin = A0; // Analog input pin
  int n = 15; // number of readings to average
  float salinity; // Average reading is a float
  salinity = sensor_reading_ave(salinity_power_pin, salinity_input_pin, n);
  Serial.println (salinity);
  
  //LCD Display
  lcd.setCursor(0, 0); // Set the cursor on the first column and first row.
  lcd.print("Salinity = "); // Print the string "Hello World!"
  lcd.setCursor(4, 1); //Set the cursor on the third column and the second row (counting starts at 0!).
  lcd.print(salinity);

  delay(100);
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
