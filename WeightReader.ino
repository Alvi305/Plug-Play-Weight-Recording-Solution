#include <HardwareSerial.h>

// Set UART Pins

#define RX_PIN 16
#define TX_PIN 17

static constexpr int bufferSize = 12;

unsigned char readBuffer[bufferSize];

// HardwareSerial object

HardwareSerial MySerial(2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  MySerial.begin(9600,SERIAL_8N1,RX_PIN,TX_PIN);

}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println("Read started");
// make this a function (get data from weight indicator)
  if (MySerial.available()) {
  
   delay(1);
   for (int i=0; i < bufferSize; i++)
   {
    readBuffer[i] = MySerial.read();
//    readSize += 1;
   }
  }

 Serial.println("Read Finished");

 Serial.flush();

// print the value (make this a function)

  
 for (int i=0; i < bufferSize; i++) 
 {
  Serial.print((char)readBuffer[i]);
  
 }

Serial.flush();
Serial.println();

  

delay(500);

}
