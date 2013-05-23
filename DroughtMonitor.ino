/*
  DIY Drought Monitor
 
 This sketch takes input from a DHT sensor and two probes -- galvanized carriage bolts in our prototype -- and measures temperature, humidity and soil moisture.
 After measuring those values, the unit uses a Sparkfun WiFly shield to upload those values to Xively (formerly Cosm/Pachube). 
 	
 This sketch is an amalgamation of sketches from Sparkfun and the Robotic Gardener.
 	 
 */

// Required libraries

#include <SPI.h>
#include <WiFly.h>
#include <WiFlyClient.h>
#include "DHT.h"

// Define the pin for the Temperature and humidity sensor.

#define DHTPIN 2
#define DHTTYPE DHT22

// Instantiate the DHT sensor

DHT dht(DHTPIN, DHTTYPE);

// VoltageFlip Soil Sensor defines
#define voltageFlipPin1 6
#define voltageFlipPin2 7
#define sensorPin 0

int flipTimer = 1000;

// Setting up the wireless network

char ssid[] = "Your-SSID"; //  your network SSID (name) 
int keyIndex = 0;            // your network key Index number (needed only for WEP)

// Setting up Xively account info. Free accounts at Xiveley.com

const char XIVELYFEED[] = "YOUR-FEED-NUMBER";
const char XIVELYKEY[] = "YOUR-KEY";

//Create a WiFly client and point it toward Xiveley

WiFlyClient client = WiFlyClient("api.xively.com", 80);


void setup() {

  Serial.begin(9600);
  
// Start up the WiFly
  WiFly.begin();

  // attempt to connect to Wifi network:
  if (!WiFly.join(ssid)) { 
    Serial.print("Failed to connect to SSID: ");
    Serial.println(ssid);
    
    // wait 10 seconds for connection:
    delay(10000);
  } 
  Serial.println("Connected to wifi");
   
  // Begin Temp and Humidity Sensor
  dht.begin();
  
  // VoltageFlip Soil Sensor Setup
  pinMode(voltageFlipPin1, OUTPUT);
  pinMode(voltageFlipPin2, OUTPUT);
  pinMode(sensorPin, INPUT);
}

void setSensorPolarity(boolean flip){
  if(flip){
    digitalWrite(voltageFlipPin1, HIGH);
    digitalWrite(voltageFlipPin2, LOW);
  }else{
    digitalWrite(voltageFlipPin1, LOW);
    digitalWrite(voltageFlipPin2, HIGH);
  }
}



uint32_t timeLastUpdated = 0;
int i;
char buff[256];

void loop() {
  // Beginning of sensor package entry
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  int h = round(dht.readHumidity());
  float t = dht.readTemperature();
  int f = round((t*1.8)+32);

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(f);
    Serial.println(" *F");
    
  }
  
  // VoltageFlip Soil Sensor reading
  setSensorPolarity(true);
  delay(flipTimer);
  int val1 = analogRead(sensorPin);
  delay(flipTimer);  
  setSensorPolarity(false);
  delay(flipTimer);
  // invert the reading
  int val2 = 1023 - analogRead(sensorPin);

// Average the two values together  
  float avg = (val1 + val2) / 2;
  
/* This converts the values to a percentage, using linear algebra. You should calibrate your sensor like this: Take a reading in air. That's 0 percent saturated. 
Then take a reading in water. That's 100 percent. So the math then goes like this (SaturatedReadingValue-AirReadingValue)/100 to give you the slope. Then use Point
Slope form to find the formula -- (Y-SaturatedReadingValue)=Slope(X-100). That'll give you percent saturation based on the readings your probes provide.
*/

  int moistureValue = round(-.1200480192*avg + 122.8);
  
  Serial.print(moistureValue);
  Serial.println("% Saturated");
 // }
// End of sensor package entry
  
  
  if (millis() - timeLastUpdated > 10000)
  {  // time for the next update
    timeLastUpdated = millis();
    
    // prepare the data to send
    // format (API V2)
    // multiple lines each with <datastreamID>,<datastreamValue>
    // feedID can be the datastream name of the numberic ID
    sprintf(buff,"Temperature,%d\nHumidity,%d\nSoilMoisture,%d",f,h,moistureValue);
    Serial.println("connecting...");
    if (client.connect()) {
      Serial.println("connected");
      client.print("PUT /v2/feeds/");  // APIV2
      client.print(XIVELYFEED);
      client.println(".csv HTTP/1.1");
      client.println("Host: api.xively.com");
      client.print("X-ApiKey: ");
      client.println(XIVELYKEY);
  
      client.println("User-Agent: Arduino (WiFly RN-XV)");
      client.print("Content-Type: text/csv\nContent-Length: ");
      client.println(strlen(buff));
      client.println("Connection: close");
      client.println();
  
      client.print(buff);
      client.println();
  
    } else {
      Serial.println("connection failed");
    }

// Give it a bit to receive the data
    delay(2000);
    while (client.available()) {
      // TODO verify success (HTTP/1.1 200 OK)
      Serial.write(client.read());  // display the result
    }
    Serial.println();
   
    if (client.connected()) {
      Serial.println("disconnecting.");
      client.stop();
      Serial.println("disconnected.");
    }
  }
// Give us enough delay so that we're reporting data every five minutes.
  delay(290000);
}
