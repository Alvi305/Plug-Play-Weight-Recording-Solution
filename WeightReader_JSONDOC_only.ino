#include <WiFi.h>
#include <HTTPClient.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>



// ============================================================= Variables and object(s)====================================================================================================

// Set UART Pins
#define RX_PIN 16
#define TX_PIN 17

#define RX_PIN_2 13 // Change this to the desired RX pin number
#define TX_PIN_2 14 // Change this to the desired TX pin number


// HardwareSerial object for UART
HardwareSerial BarcodeScannerSerial(1); // Use UART1 for barcode scanner
HardwareSerial WeightReaderSerial(2);


// Set LED & Button Pins
#define BUTTON_PIN 15


// Buffer sizes for weight and barcode scanner
static constexpr int bufferSize = 7;
char payload[bufferSize];

static constexpr int scannerbufferSize = 256;
char scannerBuffer[scannerbufferSize];

// Values to be checked against for gettting data from Weight Indicator
static constexpr int targetSize = 12;
byte targetValues[] = {32, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
//char targetValues[] = {32,'.','1','2','3','4','5','6','7','8','9','0'};


// variable for storing the pushbutton status
int buttonState = 0;


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// index for the payload array; will be set to 0 if garbage data is read
int i = 0;

// varible to detect button press
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

// Sends data from the MCU to the  CROW cpp backend

void sendData(StaticJsonDocument<250>& Doc) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Specify the target Crow API endpoint
    http.begin("http://192.168.51.11:80/crow-app/weight"); 
    http.addHeader("Content-Type", "application/json");

    //DEBUG MESSAGE. CAN REMOVE
    Serial.println("API specified");

    // Create JSON data to send
    String jsonData;
    serializeJson(Doc, jsonData);

    //DEBUG MESSAGE. CAN REMOVE
    //Serial.println("JSON data created");


    // Send the POST request and get the response code
    int httpResponseCode = http.POST(jsonData);

    //DEBUG MESSAGE. CAN REMOVE
    Serial.println("Request sent");

    delay(500);

    // Check the response code
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();

    } 
    else {
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

//// Displays weight value specifically. It accounts for showing constantly changing weight values.
//void LCD_DISPLAY_WEIGHT() {
//  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print("ID:");
//  lcd.print(scannerBuffer);
//  
//  lcd.setCursor(0, 1);
//  
//  lcd.backlight();
//
//  // Display the weight
//  lcd.print("W: ");
//  lcd.print(payload);
//  delay(20);
//}


// displays barcode and weight value on the LCD display
void LCD_DISPLAY_BARCODE(int scannerSerialCharLength ) {
  
  if (scannerSerialCharLength) {
    // used to detect end of barcode
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
        // length is 14 as id: occupies first two entries and null occupies last. that's why 16 is chosen.
        scannerBuffer[textLength-1] = '\0';
        
        for (int i = 0; i <= textLength-14; ++i) {
          lcd.setCursor(0, 0);
          lcd.print("ID:");
          lcd.print(scannerBuffer + i);
          lcd.setCursor(0, 1);
          lcd.print("W:");
          lcd.print(payload);
          delay(shiftDelay);
        }
      } else {  
          lcd.setCursor(0, 0);
          lcd.print("ID:");
          
          // Create a new string without the last character
          scannerBuffer[textLength-1] = '\0';
          lcd.print(scannerBuffer);
          
//          lcd.setCursor(0, 1);
//          lcd.print("W: ");
//          lcd.print(payload);

          
      }

//      // Clear the scannerBuffer array for new values
//      scannerBuffer[0] = '\0';
    }
  }
}




// ISR for button press. NOTE : DO NOT ADD TOO MANY PROCESSES IN ISR

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

// getting weight data from weight indicator
void ReadDataFromDevice() {
  if (WeightReaderSerial.available()) 
 {
    delay(2);

    if (WeightReaderSerial.read() == (byte)22) 
    {
      // Add data from Weight Indicator to the created buffer array
      
      //Serial.println("22 found. Filling payload"); // DEBUG MESSAGE

      for (i = 0; i < bufferSize; ++i) {
        byte payloadData = WeightReaderSerial.read();
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
          //Serial.println(temp); // DEBUG MESSAGE
          break;
        }
      }
    }
//    WeightReaderSerial.flush();
  }
}

// to print weight value
void PrintValues() {

 Serial.print(payload);

}






// Global variable to store the JSON object
StaticJsonDocument<250> jsonDoc;

// =======================================================================================================Setup and Loop Functions============================================================================================================

void setup() {
// Initialize the built-in serial connection for debugging
Serial.begin(115200);

// Initialize the connection to the Magic Weight Indicator
WeightReaderSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

// Initialize Barcode Scanner
BarcodeScannerSerial.begin(115200, SERIAL_8N1, RX_PIN_2, TX_PIN_2);



// initialize the pushbutton pin as an input
pinMode(BUTTON_PIN, INPUT_PULLUP);

// Attach interrupt to button pin, call handleButtonPress on falling edge
attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, RISING);

// prevent same readings from filling the RX buffer for the weight indicator
WeightReaderSerial.setRxBufferSize(16);

initWifi();


  
// For bootloader log messages
Serial.println();
Serial.println("-----------Sketch started-----");
Serial.flush();


  
//Initialize the LCD
 
  lcd.begin(35, 36);

  lcd.setCursor(0,0);
  lcd.print("System Loading...");

  // Display progress bar
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    lcd.write(0xFF); // Print a solid block
    delay(125); // Adjust this value to control the speed of the progress bar
    lcd.setCursor(i + 1, 1);
  }

  delay(500);
  lcd.clear();
  lcd.print("ID:");
  lcd.setCursor(0,1);
  lcd.print("W:");
  
}





void loop() {
  // Put your main code here, to run repeatedly:
  
ReadDataFromDevice();

const int barcodeScannerLength = BarcodeScannerSerial.available();
LCD_DISPLAY_BARCODE(barcodeScannerLength);

// assign barcode reader value as field to json object
jsonDoc["barcode"] = scannerBuffer;

// get weight data
  if (i == bufferSize) 
  {

    double weightValue = atof(payload); // Convert the payload to a double value
    jsonDoc["weight"] = weightValue; // Update the JSON object with the double value
    lcd.setCursor(0,1);
    lcd.print("W:");
    lcd.print(payload);
//    LCD_DISPLAY_WEIGHT();   
  }

//jsonDoc["weight"] = 12.78; // Update the JSON object with the double value

  


// send weight and barcode data
  if (buttonPressed) {
        
        serializeJsonPretty(jsonDoc, Serial);
        sendData(jsonDoc); // Pass jsonDoc as an argument
        buttonPressed = false;
        lcd.clear();
        delay(100);
        lcd.setCursor(0, 0);
        lcd.print("ID:");
        
    }

}
