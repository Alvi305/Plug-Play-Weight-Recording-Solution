#include <HardwareSerial.h>

// Set UART Pins
#define RX_PIN 16
#define TX_PIN 17

// Buffer size
static constexpr int payloadSize = 7;
char payload[bufferSize];

// Values to be check against

byte targetValues[] = {32,48,49,50,51,52,53,54,55,56,57}

bool stopChar = false;

// HardwareSerial object
HardwareSerial MySerial(2);

void setup() {
  // Initialize the built-in serial connection for debugging
  
  // Initialize the connection to the Magic Weight Indicator
  MySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

// for bootloader log messages
  Serial.println();
  Serial.println("-----------Sketch started-----");
  Serial.flush();

void ReadDataFromDevice() {
 

  if (MySerial.available()) {
    delay(2);

   if (MySerial.read() == (byte) 22)
   {
      int i = 0;
   
      for ( i = 0; i < payLoad
   }








    
    for (int i = 0; i < payLoadSize; i++) {
      delay(3);
      payLoadBuffer[i] = MySerial.read();
      //MySerial.flush();
     // Serial.println((char)readBuffer[i]); // debugging
    }
  }
}


  

void loop() {
  // put your main code here, to run repeatedly:

}
