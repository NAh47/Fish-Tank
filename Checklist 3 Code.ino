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
  lcd.setCursor(4, 0); // Set the cursor on the first column and first row.
  lcd.print("NaCl Percent"); // Prints the text label for below print
  lcd.setCursor(7, 1); //Set the cursor on the third column and the second row (counting starts at 0!).
  lcd.print(salinity, 3);
  lcd.setCursor(12, 1);
  lcd.print("%");
 
  //Team name label
  lcd.setCursor(8, 2);
  lcd.print("Team");
  lcd.setCursor(2, 3);
  lcd.print("Against All Odds");
}
