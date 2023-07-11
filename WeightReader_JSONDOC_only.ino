#include <WiFi.h>
#include <HTTPClient.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <Arduino.h>


// ============================================================= Variables and object(s)====================================================================================================

// Set UART Pins
#define RX_PIN 16
#define TX_PIN 17



// Set LED & Button Pins
#define BUTTON_PIN 15


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

volatile bool buttonPressed = false;

//  variables used for connecting to the WiFi
const char* ssid = "Aerovision"; 
const char* password =  "W1F1@Aer0visi0n"; 
 
IPAddress staticIP(192, 168, 51, 216);
IPAddress gateway(192, 168, 51, 1);   // Replace this with your gateway IP Addess
IPAddress subnet(255, 255, 0, 0);  // Replace this with your Subnet Mask
IPAddress dns(192, 168, 51, 1);   // Replace this with your DNS




// ================================================ FUNCTIONS =====================================================================================================================


// To connect to wifi
void initWifi() {
  
 if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
   Serial.println("Configuration failed.");
 }
   
 WiFi.begin(ssid, password);
 
 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print("Connecting...\n\n");
 }
 
 Serial.print("Connected. Local IP: ");
 Serial.println(WiFi.localIP());
}

void sendData(StaticJsonDocument<100>& Doc) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Specify the target Crow API endpoint
    http.begin("http://192.168.51.11:18081/crow-app/weight"); 
    http.addHeader("Content-Type", "application/json");

    // Create JSON data to send
    String jsonData;
    serializeJson(Doc, jsonData);

    // Send the POST request and get the response code
    int httpResponseCode = http.POST(jsonData);

    // Check the response code
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.print("Error message: ");
      Serial.println(http.errorToString(httpResponseCode)); // Add this line to print the error message
    }

    // Close the connection
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}






void IRAM_ATTR handleButtonPress() {
  buttonPressed = true;
}


// for parsing data stream received from weight indicator
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
//    MySerial.flush();
  }
}

void PrintValues() {

 Serial.print(payload);

}






// Global variable to store the JSON object
StaticJsonDocument<100> jsonDoc;
// =======================================================================================================Setup and Loop Functions============================================================================================================

void setup() {
  // Initialize the built-in serial connection for debugging
  Serial.begin(115200);

  // Initialize the connection to the Magic Weight Indicator
  MySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);


 // initialize the pushbutton pin as an input
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Attach interrupt to button pin, call handleButtonPress on falling edge
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, RISING);

// prevent same readings from filling the RX buffer
  MySerial.setRxBufferSize(16);

  initWifi();


  
  // For bootloader log messages
  Serial.println();
  Serial.println("-----------Sketch started-----");
  Serial.flush();
}





void loop() {
  // Put your main code here, to run repeatedly:
  
ReadDataFromDevice();



// get weight data
  if (i == bufferSize) 
  {
     double weightValue = atof(payload); // Convert the payload to a double value
    jsonDoc["weight"] = weightValue; // Update the JSON object with the double value
    
  }


// send weight and barcode data
  if (buttonPressed) {
        sendData(jsonDoc); // Pass jsonDoc as an argument
        buttonPressed = false;
    }



 
  
   
  

}
