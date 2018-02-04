#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <HX711.h>
//==============================================================================================
// Scale Settings
const int SCALE_DOUT_PIN = D2;
const int SCALE_SCK_PIN = D3;
// constants won't change. They're used here to set pin numbers:
const int hallPin = D0;     // the number of the hall effect sensor pin
const int ledPin =  D7;     // the number of the LED pin
// variables will change:
int hallState = 0;          // variable for reading the hall sensor status
float value = 0;
HX711 scale(SCALE_DOUT_PIN, SCALE_SCK_PIN);

//==============================================================================================
 
void setup() {
 
  Serial.begin(115200);                            //Serial connection
  //============================================================================================
  scale.set_scale(2230);// <- set here calibration factor!!!
  scale.tare();
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);      
  // initialize the hall effect sensor pin as an input:
  pinMode(hallPin, INPUT);
  //============================================================================================
  WiFi.begin("DataSoft_WiFi", "support123");   //WiFi connection
 
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
 
    delay(500);
    Serial.println("Waiting for connection");
 
  }
  Serial.println("WiFi Connected!");
 
}
//==============================================================================================
//float HallEffect(){
//  hallState = digitalRead(hallPin);   // read the state of the hall effect sensor:
//  // Magnet near == LOW
//  if (hallState == LOW) {     
//    // turn LED on:  
//    Serial.println("!!! LID ON !!!");  
//    digitalWrite(ledPin, LOW);  
//  } 
//  else if (hallState == HIGH) {
//    // turn LED off:
//    digitalWrite(ledPin, HIGH);
//    Serial.println("!!! LID OFF !!!");
//    value = LoadCell(); // Call LoadCell function
//    Serial.print("In HallEffect ");
//    Serial.println(value);
//  }
//}

float LoadCell(){
    float weight = scale.get_units(1); //loadcell
    //weight = weight + 90;
    //value = String((weight), 5);
    value = weight;
    Serial.print("In loadcell ");
    Serial.println(value);
    return value;
    delay(5000);
}

 
void loop() {
  
  hallState = digitalRead(hallPin);   // read the state of the hall effect sensor:
  //Magnet near == LOW
  if (hallState == LOW){
    Serial.println("!!! LID ON, Time to take the reading and send it to server!!!");  
    digitalWrite(ledPin, HIGH);
    value = LoadCell(); 
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status      
      StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
      JsonObject& JSONencoder = JSONbuffer.createObject(); 
   
      JSONencoder["sensorType"] = "LoadCell";
   
      JsonArray& values = JSONencoder.createNestedArray("Gram"); //JSON array
      values.add(value); //Add value to array
      //==========================================================================================
      char JSONmessageBuffer[300];
      JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      Serial.println(JSONmessageBuffer);
   
      HTTPClient http;    //Declare object of class HTTPClient
   
      http.begin("http://192.168.6.77:8081/value");      //Specify request destination
      http.addHeader("Content-Type", "application/json");  //Specify content-type header
   
      int httpCode = http.POST(JSONmessageBuffer);   //Send the request
      String payload = http.getString();                                        //Get the response payload
   
      Serial.println(httpCode);   //Print HTTP return code
      Serial.println(payload);    //Print request response payload
   
      http.end();  //Close connection
   
    } else {
   
      Serial.println("Error in WiFi connection");
   
    }
 
  delay(5000);  //Send a request every 30 seconds
  }
  else if (hallState == HIGH){
    Serial.println("!!! LID OFF, Time to take my timely MEDICINE!!!");  
    digitalWrite(ledPin, LOW);
  }

}
