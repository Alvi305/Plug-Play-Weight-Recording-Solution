#include <HardwareSerial.h>
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define RX_PIN 13 // Change this to the desired RX pin number
#define TX_PIN 14 // Change this to the desired TX pin number

HardwareSerial BarcodeScannerSerial(1); // Use UART1 for barcode scanner

void setup() {
  Serial.begin(115200); // Start the serial monitor
  BarcodeScannerSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  //Initialize the LCD
  lcd.begin(35, 36);

  lcd.setCursor(0,0);
  lcd.print("System Loading....");
  delay(2000);
  lcd.clear();
  lcd.print("ID:");
  lcd.setCursor(0,1);
  lcd.print("W:");
  
}

void loop() {
  const int scannerSerialCharLength = BarcodeScannerSerial.available();
  if (scannerSerialCharLength) {
    const size_t bufferSize = 256;
    char scannerBuffer[bufferSize];
    char terminator = '\0';

    size_t bytesRead = BarcodeScannerSerial.readBytesUntil(terminator, scannerBuffer, scannerSerialCharLength);
    scannerBuffer[bytesRead] = '\0'; 

    if (scannerBuffer[0] == static_cast<char>(0x80)) {
      scannerBuffer[0] = '\0'; // Clear the buffer by setting the first byte to the null terminator
    } else {
//      // Clear the LCD and set the cursor position to the beginning
      lcd.clear();
      lcd.setCursor(0, 0);

      // Turn on the backlight and print the message from the buffer.
      lcd.backlight();
      
       // Slide text animation
      int textLength = strlen(scannerBuffer);
      int shiftDelay = 300; // Adjust this value to control the speed of the sliding animation

      if (textLength > 16) {
        for (int i = 0; i <= textLength - 16; ++i) {
          lcd.setCursor(0, 0);
          lcd.print("ID:");
          lcd.print(scannerBuffer + i);
          lcd.setCursor(0, 1);
          lcd.print("W:12.54 kg");
          delay(shiftDelay);
        }
      } else {
        lcd.print("ID:");
        lcd.print(scannerBuffer);
        lcd.setCursor(0, 1);
        lcd.print("W:12.54 kg");
      }

      // Clear the scannerBuffer array
      scannerBuffer[0] = '\0';
    }
  }
}
