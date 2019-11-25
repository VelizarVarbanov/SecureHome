#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>


#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
#define WIFI_SSID "Redmi"
#define WIFI_PASSWORD ""
#define FIREBASE_HOST "home-77c1d.firebaseio.com"
#define FIREBASE_AUTH "lItfbhEAueAhkJ1nOUuysZp4cxzSVN6dxiapmFDv"
                    // hbd7eGJi7d3hIO0NCffGkMvJnHPD0frWiZTJ7ljt

int LED1 = 4;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


void setup() {
  sensors.begin();
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  wifiConnect();
  delay(1000);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  delay(10);
}

void loop() {
  sensors.requestTemperatures();

  Serial.print("Temperature1: ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.print(" ");
  Serial.print("C | ");
  Serial.print("\n");

   Serial.print("Temperature2: ");
  Serial.print(sensors.getTempCByIndex(1));
  Serial.print(" ");
  Serial.print("C | ");
  Serial.print("\n");

  if(WiFi.status() != WL_CONNECTED)
  {
    wifiConnect();
  }
  
  Firebase.pushString("Temp1", String(sensors.getTempCByIndex(0)));
  Firebase.pushString("Temp2", String(sensors.getTempCByIndex(1)));

  delay(7000);
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
    Serial.print(++teller); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}
