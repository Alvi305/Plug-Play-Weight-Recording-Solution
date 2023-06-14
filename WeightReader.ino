#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <UUID.h>
#include <Arduino.h>


UUID uuid;

// Set UART Pins
#define RX_PIN 16
#define TX_PIN 17


// Set LED & Button Pins
#define BUTTON_PIN 5
#define LED_PIN 4


// Buffer size
static constexpr int bufferSize = 7;
char payload[bufferSize];

// Values to be checked against
static constexpr int targetSize = 12;
byte targetValues[] = {32, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
//char targetValues[] = {32,'.','1','2','3','4','5','6','7','8','9','0'};


// variable for storing the pushbutton status
int buttonState = 0;

// HardwareSerial object
HardwareSerial MySerial(2);


// index for the payload array
int i = 0;

void setup() {
  // Initialize the built-in serial connection for debugging
  Serial.begin(115200);

  // Initialize the connection to the Magic Weight Indicator
  MySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);


  // initialize the pushbutton pin as an input
  pinMode(BUTTON_PIN, INPUT);
  // initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);

  // set the seed for generating random uuid
  uint32_t seed1 = random(999999999);
  uint32_t seed2 = random(999999999);

  uuid.seed(seed1, seed2);

  MySerial.setRxBufferSize(16);
  // For bootloader log messages
  Serial.println();
  Serial.println("-----------Sketch started-----");
  Serial.flush();
}

bool FindValueinTarget(char target, byte targetArray[], int arrSize) {
  for (int j = 0; j < arrSize; ++j) {
    if (targetArray[j] == target) 
    {
      return true;
    }
   
  }
  return false;

}

void ReadDataFromDevice() {
  if (MySerial.available()) 
 {
    delay(2);

    if (MySerial.read() == (byte)22) 
    {
      // Add data from Weight Indicator to the created buffer array
      //Serial.println("22 found. Filling payload"); // DEBUG MESSAGE

      for (i = 0; i < bufferSize; ++i) {
        byte payloadData = MySerial.read();
        if (FindValueinTarget(payloadData, targetValues, targetSize)) {
         // Serial.println("Value matched. Adding to payload array"); // DEBUG MESSAGE
          
          // Change [SPACE] to zero otherwise add it to array
          if (payloadData == 32) 
          {
            payload[i] = '0';
          } 
          else 
          {
            payload[i] = payloadData;
          }
        } 
        else 
        { // If value does not match, then sequence is ruined. Start refilling buffer again
         // Serial.println("Value does not match. Abort filling payload array"); // DEBUG MESSAGE
          //Serial.println(temp);
          break;
        }
      }
    }
  }
}

void PrintValues() {

 Serial.print(payload);

}

void sendPayload( int state) {
  // CALL FUNCTION HERE TO SEND PAYLOAD TO WEBSERVER
  
}

JsonObject payloadData(StaticJsonDocument<200>& jsonDoc, char* id, char jsonarray[]) {
  JsonObject jsonObj = jsonDoc.to<JsonObject>();

  String weight(jsonarray);
//  weight.remove(weight.length() - 1); // Remove the last character
  jsonObj["id"] = id;
  jsonObj["weight"] = weight;

  return jsonObj;
}

char* generateUUID() {
  // Generate a new UUID
  uuid.generate();

  return uuid.toCharArray();
}

// Global variable to store the JSON object
StaticJsonDocument<200> jsonDoc;

void loop() {
  // Put your main code here, to run repeatedly:
  ReadDataFromDevice();
  char* id = generateUUID();

  JsonObject* objPtr = nullptr;

  

  if (i == bufferSize) {
    JsonObject obj = payloadData(jsonDoc, id, payload);
    serializeJsonPretty(obj, Serial);
    Serial.println();
    
    MySerial.flush();
   // objPtr = &obj;
  }


// READ BUTTON PRESS
  buttonState = digitalRead(BUTTON_PIN);
  //Serial.println(buttonState);


// TURN LED ON IF BUTTON IS PRESSED
  if (buttonState == 1) { 
    /Serial.println("DATA SENT");    
  // CALL FUNCTION HERE TO SEND PAYLOAD TO WEBSERVER 
  }
  
  else {
  // turn LED off
  digitalWrite(LED_PIN, LOW);
  
  //    sendPayload(buttonState);
  //    if (objPtr) {
  //      serializeJsonPretty(*objPtr, Serial);
  //      Serial.println();
  //    }
  }
}
