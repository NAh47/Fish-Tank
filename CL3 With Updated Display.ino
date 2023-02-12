#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for I2C communication
float salinity;
int SalinityDigitalValue = A0;
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // Change to (0x27,16,2) for 16x2 LCD.
void setup() {
  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
  // Initiate the potentiometer
  pinMode(SalinityDigitalValue, INPUT);
  Serial.begin(9600);
  }
void loop() {

  //Convert digital reading of (0-1023) to the salinity % range of (0.0-0.56)
  if (analogRead(SalinityDigitalValue)<638.86) {
    salinity = ((analogRead(SalinityDigitalValue)*(0.0000898761686149))-(0.00741828908132));
  }
    else if (analogRead(SalinityDigitalValue)>638.86) {
    salinity = ((analogRead(SalinityDigitalValue)*(0.00132661183338))-(0.797519235873));
  }
  
  // Prints pot values to LCD and labels them
  lcd.setCursor(3, 0); // Set the cursor on the first column and first row.
  lcd.print("LCL"); // Prints the text label for below print
  lcd.setCursor(9, 0); // Set the cursor on the first column and first row.
  lcd.print("SP"); // Prints the text label for below print
  lcd.setCursor(15, 0); // Set the cursor on the first column and first row.
  lcd.print("UCL"); // Prints the text label for below print
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.setCursor(3, 1);
  lcd.print("SVal"); //CHANGE THIS TO LCL S OUTPUT
  lcd.setCursor(9, 1);
  lcd.print("Sval"); //CHANGE THIS TO SP S OUTPUT
  lcd.setCursor(15, 1);
  lcd.print("Sval"); //CHANGE THIS TO UCL S OUTPUT
  lcd.setCursor(0, 2);
  lcd.print("T:");
  lcd.setCursor(3, 2);
  lcd.print("TVal"); //CHANGE THIS TO LCL T OUTPUT
  lcd.setCursor(9, 2);
  lcd.print("Tval"); //CHANGE THIS TO LCL T OUTPUT
  lcd.setCursor(15, 2);
  lcd.print("Tval"); //CHANGE THIS TO S= T OUTPUT
  lcd.setCursor(0, 3);
  lcd.print("S=");
  lcd.setCursor(3, 3);
  lcd.print("S=1"); //CHANGE THIS TO THE FIRST S= OUTPUT
  lcd.setCursor(9, 3);
  lcd.print("S=2"); //CHANGE THIS TO THE SECOND S= OUTPUT
  lcd.setCursor(15, 3);
  lcd.print("S=3"); //CHANGE THIS TO THE THIRD S= OUTPUT
  }
  
