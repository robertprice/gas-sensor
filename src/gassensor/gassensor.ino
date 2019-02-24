// Read Data from Grove - Multichannel Gas Sensor
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "MutichannelGasSensor.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10


#define I2C_Gas 0x04
#define I2C_BME 0x76

#define WIFI_Network "Your network name"
#define WIFI_Password "Your network password"
/*
 * Add your WiFi details in the settings.h file
 */
#include "settings.h"


/*
 * Wiring the Grove Multichannel Gas Sensor and BME280 to the NodeMCU
 * 
 * Both sensors are connected using the I2C bus, so share
 * pins D1 and D2 on the NodeMCU.
 * 
 * Grove -> NodeMCU
 * SCL   -> D1
 * SDA   -> D2
 * VCC   -> 3V
 * GND   -> GND
 * 
 * BME280 -> NodeMCU
 * SCL    -> D1
 * SDA    -> D2
 * VCC    -> 3V
 * GND    -> GND
 */


Adafruit_BME280 bme; // I2C

void setup()
{
  Serial.begin(9600);  // start serial for output
  Serial.println("power on!");

  // start gas sensor
  gas.begin(I2C_Gas);//the default I2C address of the gas sensor slave is 0x04
  gas.powerOn();
  Serial.print("gas sensor firmware ");
  Serial.println(gas.getVersion());

  // start bme280
  bool status;
  status = bme.begin(I2C_BME); // clean air eastbourne addition - our bme280 from AliExpress is on 0x76
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  setupWifi();
  
  delay(5000);
}

void loop()
{
  StaticJsonBuffer<2048> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["esp8266id"] = ESP.getChipId();

  // get temperature (c), pressure (hPa), and humidity (%)
  root["temperature"] = bme.readTemperature();
  root["pressure"] = (bme.readPressure() / 100.0F);
  root["humidity"] = bme.readHumidity();


  // get gas readings
  float c;   // used to store the gas reading

  // gases are all measured in PPM
  c = gas.measure_NH3();
  if (c >= 0) {
    root["nh3"] = c;
  } else {
    root["nh3"] = NULL;
  }

  c = gas.measure_CO();
  if (c >= 0) {
    root["co"] = c;
  } else {
    root["co"] = NULL;
  }

  c = gas.measure_NO2();
  if (c >= 0) {
    root["no2"] = c;
  } else {
    root["no2"] = NULL;
  }

  c = gas.measure_C3H8();
  if (c >= 0) {
    root["c3h8"] = c;
  } else {
    root["c3h8"] = NULL;
  }

  c = gas.measure_C4H10();
  if (c >= 0) {
    root["c4h10"] = c;
  } else {
    root["c4h10"] = NULL;
  }

  c = gas.measure_CH4();
  if (c >= 0) {
    root["ch4"] = c;
  } else {
    root["ch4"] = NULL;
  }

  c = gas.measure_H2();
  if (c >= 0) {
    root["h2"] = c;
  } else {
    root["h2"] = NULL;
  }

  c = gas.measure_C2H5OH();
  if (c >= 0) {
    root["c2h5oh"] = c;
  } else {
    root["c2h5oh"] = NULL;
  }

  //root.prettyPrintTo(Serial);

  String jsonString = "";
  root.printTo(jsonString);
  senddata(jsonString);

  Serial.flush();
  delay(60000); // wait for 1 minute
}

void senddata(String jsonString)
{
  Serial.println("senddata called...");
  Serial.println(jsonString);
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    String host = "api.eastbourneair.com";
    
    if (client.connect(host, 80)) {
      Serial.println("Connected to " + host);
      client.print(String("POST /gaslogger.php") + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "Connection: close\r\n" +
                          "Accept: */*\r\n" +
                          "User-Agent: GasMonitor/0.1\r\n" +
                          "Content-Type: application/json\r\n" +
                          "Content-Length: " + String(jsonString.length()) + "\r\n" +
                          "\r\n" +
                          jsonString + "\r\n" 
                  );
    } else {
      Serial.println("Unable to connect to " + host);
    }

  } else {
    Serial.println("Unable to connect to network");
    setupWifi();
  }
}

void setupWifi()
{
  Serial.print("Connecting to wifi...");
  WiFi.begin(WIFI_Network, WIFI_Password);
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to wifi ");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin("gassensor")) {             // Start the mDNS responder for gassensor.local
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started");
  }  
}
