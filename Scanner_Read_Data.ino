#include <HardwareSerial.h>
#include <Arduino.h>

#define RX_PIN 13 // Change this to the desired RX pin number
#define TX_PIN 14 // Change this to the desired TX pin number

HardwareSerial BarcodeScannerSerial(1); // Use UART1 for barcode scanner

void setup() {
  Serial.begin(115200); // Start the serial monitor
  BarcodeScannerSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
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
      Serial.println(scannerBuffer);
    }
  }
}
