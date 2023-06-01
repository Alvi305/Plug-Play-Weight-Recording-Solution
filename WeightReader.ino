#include <HardwareSerial.h>

// Set UART Pins
#define RX_PIN 16
#define TX_PIN 17

// Buffer size
static constexpr int bufferSize = 11;
unsigned char readBuffer[bufferSize];

bool stopChar = false;

// HardwareSerial object
HardwareSerial MySerial(2);

void setup() {
  // Initialize the built-in serial connection for debugging
  Serial.begin(115200);
  // Initialize the connection to the Magic Weight Indicator
  MySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

// for bootloader log messages
  Serial.println();
  Serial.println("-----------Sketch started-----");
  Serial.flush();
}

void ReadDataFromDevice() {
  if (MySerial.available()) {
    delay(10);
    for (int i = 0; i < bufferSize; i++) {
      delay(3);
      readBuffer[i] = MySerial.read();
      //MySerial.flush();
     // Serial.println((char)readBuffer[i]); // debugging
    }
  }
}

void PrintBufferValues() {
  for (int i = 0; i < bufferSize; i++) {
    Serial.print((char)readBuffer[i]);
  }
  Serial.println();
  Serial.flush();
  delay(1000);
}

void loop() {

  
ReadDataFromDevice();

PrintBufferValues();
}
