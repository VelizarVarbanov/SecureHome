#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//D1 = 5, D2 = 4, D3 = 0, D4 = 2, D5 = 14, D6 = 12, D7=13 
#define DHT11_PIN 5
#define DHTTYPE DHT11
#define ONE_WIRE_BUS_DS18B20 4
#define LED 14
#define pirPin 0
#define buzzerPin 2
#define mq6pin 12
#define mq9pin 13

#define WIFI_SSID "Redmi"
#define WIFI_PASSWORD ""
#define FIREBASE_HOST "home-77c1d.firebaseio.com"
#define FIREBASE_AUTH "lItfbhEAueAhkJ1nOUuysZp4cxzSVN6dxiapmFDv"
                    // hbd7eGJi7d3hIO0NCffGkMvJnHPD0frWiZTJ7ljt


DHT sensorDHT(DHT11_PIN, DHTTYPE);
 
OneWire oneWireDS18B20(ONE_WIRE_BUS_DS18B20);
DallasTemperature sensorsDS18B20(&oneWireDS18B20);

const long utcOffsetInSeconds = 7200;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

bool motionState = false; // We start with no motion detected.
    
int secCounter = 0;
bool bDebug = true;
bool bHotSpot = true;

void setup() {
  Serial.begin(9600);
  
  sensorDHT.begin();
  sensorsDS18B20.begin();

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  noTone(buzzerPin);

  pinMode(mq6pin, INPUT);
  pinMode(mq9pin, INPUT);
    
  if(bHotSpot)
  {
    wifiConnect();
  }
  delay(1000);
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  delay(10);
  
  timeClient.begin();
}

void loop() {

  if(bHotSpot)
  {
    if(WiFi.status() != WL_CONNECTED)
    {
      wifiConnect();
    }
  }

////////////////////////////////////////////////////////////
// Read out the pirPin and store in Firebase:
      timeClient.update();
      
      // If motion is detected (pirPin = HIGH), do the following:
      if ((digitalRead(pirPin) == HIGH) && (Firebase.getInt("Alarm") == 1)) 
      {
        alarm(1500, 1000);  // Call the alarm(duration, frequency) function.
        delay(150);
        //write in firebase 
        String alarmlog = String (daysOfTheWeek[timeClient.getDay()]) + " " + timeClient.getHours() + ":" + timeClient.getMinutes() + ":" + timeClient.getSeconds();        // Change the motion state to true (motion detected):
        Firebase.pushString("AlarmLog", alarmlog);
        if (motionState == false) 
        {
          Serial.println("Motion detected!");
          motionState = true;
        }
      }
      // If no motion is detected (pirPin = LOW), do the following:
      else 
      {
        noTone(buzzerPin); // Make sure no tone is played when no motion is detected.
        delay(150);
        // Change the motion state to false (no motion):
        if (motionState == true) 
        {
          Serial.println("Motion ended!");
          motionState = false;
        }
      }
////////////////////////////////////////////////////////////  

////////////////////////////////////////////////////////////
// Read GAS sensor:
 
 
  /*if(digitalRead(mq6pin)==HIGH) 
  {
     Serial.println("ALARM MQ6"); 
     String gasLog = String (daysOfTheWeek[timeClient.getDay()]) + " " + timeClient.getHours() + ":" + timeClient.getMinutes() + ":" + timeClient.getSeconds() + "GAS Detected in kitchen!";        // Change the gas state to true (gas detected):
        Firebase.pushString("GASLog", gasLog);
      
  }

  if(digitalRead(mq9pin)==HIGH) 
  {
     Serial.println("ALARM MQ9"); 
     String co = String (daysOfTheWeek[timeClient.getDay()]) + " " + timeClient.getHours() + ":" + timeClient.getMinutes() + ":" + timeClient.getSeconds() + "CO Detected in Garage!";        // Change the CO state to true (CO detected):
        Firebase.pushString("GarageLog", co); 
  }
 */
 
////////////////////////////////////////////////////////////
  if(secCounter > 14)
  {
    sensorsDS18B20.requestTemperatures();
    //Temp. Bedroom AND Temp. OutDoor
    String data = String (daysOfTheWeek[timeClient.getDay()]) + " " + timeClient.getHours() + ":" + timeClient.getMinutes() + ":" + timeClient.getSeconds() 
      + "/-" + "Humidity Living Room: " + sensorDHT.readHumidity() + "/-" + "Temp. Living Room: " + sensorDHT.readTemperature()
      + "/-" + "Temp. Bedroom:" + sensorsDS18B20.getTempCByIndex(0) + "/-" + "Temp. OutDoor:" + sensorsDS18B20.getTempCByIndex(1);

    if(bDebug)
    {
      Serial.println(data);
    }

    Firebase.pushString("Data", data);
    
    secCounter = 0;
  }
  else
  {
    secCounter++;
  }
  
  if (Firebase.getInt("Light") == 1) 
  {
    digitalWrite(LED, HIGH);      
  }
  else
  {
    digitalWrite(LED, LOW);
  }
  
  delay(1000);
}

void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID); Serial.println(" ...");

  int teller = 0;
  while (WiFi.status() != WL_CONNECTED)
  {                                       // Wait for the Wi-Fi to connect
    delay(1000);
    if(bDebug)
    {
      Serial.print(++teller); Serial.print(' ');
    }
  }

  if(bDebug)
  {
    Serial.println('\n');
    Serial.println("Connection established!");  
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  }
}

// Function to create a tone with parameters duration and frequency:
void alarm(long duration, int freq) 
{
  tone(buzzerPin, freq);
  delay(duration);
  noTone(buzzerPin);
}
