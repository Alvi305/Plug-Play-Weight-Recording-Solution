#include <HardwareSerial.h>

// Set UART Pins
#define RX_PIN 16
#define TX_PIN 17

// Buffer size
static constexpr int bufferSize = 12;
unsigned char readBuffer[bufferSize];

// HardwareSerial object
HardwareSerial MySerial(2);

void setup() {
  // Initialize the built-in serial connection for debugging
  Serial.begin(115200);

  // Initialize the connection to the Magic Weight Indicator
  MySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
}

void readDataFromDevice() {
  if (MySerial.available()) {
    delay(1);
    for (int i = 0; i < bufferSize; i++) {
      readBuffer[i] = MySerial.read();
    }
  }
}

void printBufferValues() {
  for (int i = 0; i < bufferSize; i++) {
    Serial.print((char)readBuffer[i]);
  }
  Serial.println();
}

void loop() {
  Serial.println("Read started");

  // Read data from the Magic Weight Indicator
  readDataFromDevice();

  Serial.println("Read Finished");
  Serial.flush();

  // Print the buffer values
  printBufferValues();

  Serial.flush();
  delay(500);
}
