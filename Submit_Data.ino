#include <Arduino.h>

// Emulate Hardware Sensor?
bool virtual_sensor = true;

#include <ESP8266WiFi.h>      // Ensure to include the ESP8266Wifi.h library, not the common library WiFi.
#include <PubSubClient.h>     // Include PubSubClient library to handle MQTT
#include <WiFiClientSecure.h> // Include WiFiClientSecure to establish a secure connection (Anedya only allows secure connections)
#include <ArduinoJson.h>      // Include the ArduinoJson library to make JSON or extract values from JSON
#include <TimeLib.h>          // Include the Time library to handle time synchronization with ATS (Anedya Time Services)
#include <DHT.h>              // Include the DHT library for humidity and temperature sensor handling

String regionCode = "ap-in-1";                   // Anedya region code (e.g., "ap-in-1" for Asia-Pacific/India) | For other country codes, visit [https://docs.anedya.io/device/intro/#region]
const char *deviceID = "25b8c097-590f-4d84-a734-776ac7c0fb1b"; // Fill in your device ID, which you can get from your node description
const char *connectionkey = "45c168dc4b8fc2d085de0c3a547b557b";  // Fill in your connection key, which you can get from your node description

// WiFi credentials
const char *ssid = "Jatin";     // Replace with your WiFi name
const char *pass = "jatin123"; // Replace with your WiFi password

// MQTT connection settings
String str_broker = "mqtt." + String(regionCode) + ".anedya.io";
const char *mqtt_broker = str_broker.c_str();                              // MQTT broker address
const char *mqtt_username = deviceID;                                      // MQTT username
const char *mqtt_password = connectionkey;                                 // MQTT password
const int mqtt_port = 8883;                                                // MQTT port
String responseTopic = "$anedya/device/" + String(deviceID) + "/response"; // MQTT topic for device responses
String errorTopic = "$anedya/device/" + String(deviceID) + "/errors";      // MQTT topic for device errors

long long submitTimer;     // Timer to handle request delay
String timeRes, submitRes; // Variables to handle responses

// Define the type of DHT sensor (DHT11, DHT21, DHT22, AM2301, AM2302, AM2321)
#define DHT_TYPE DHT11
// Define the pin connected to the DHT sensor
#define DHT_PIN 5 // Pin marked as D1 on the NodeMCU board

// Define the sensor variables
int Hartbeat;
float temperature;
int SPO2;

// Function Declarations
void connectToMQTT();                                               // Function to connect with the Anedya broker
void mqttCallback(char *topic, byte *payload, unsigned int length); // Function to handle callbacks
void setDevice_time();                                              // Function to configure the device time with real-time from ATS (Anedya Time Services)
void anedya_submitData(String datapoint, float sensor_data);        // Function to submit data to the Anedya server

// WiFi and MQTT client initialization
WiFiClientSecure esp_client;
PubSubClient mqtt_client(esp_client);

// Create a DHT object
DHT dht(DHT_PIN, DHT_TYPE);

void setup()
{
  Serial.begin(115200); // Initialize serial communication with your device compatible baud rate
  delay(1500);          // Delay for 1.5 seconds

  // Connect to WiFi network
  WiFi.begin(ssid, pass);
  Serial.println();
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  submitTimer = millis();

  esp_client.setInsecure();
  mqtt_client.setServer(mqtt_broker, mqtt_port); // Set the MQTT server address and port for the MQTT client to connect to the Anedya broker
  mqtt_client.setKeepAlive(60);                  // Set the keep-alive interval (in seconds) for the MQTT connection to maintain connectivity
  mqtt_client.setCallback(mqttCallback);         // Set the callback function to be invoked when MQTT messages are received
  connectToMQTT();                               // Attempt to establish a connection to the Anedya broker
  mqtt_client.subscribe(responseTopic.c_str());  // Subscribe to get responses
  mqtt_client.subscribe(errorTopic.c_str());     // Subscribe to get errors

  setDevice_time(); // Function to sync the device time

  // Initialize the DHT sensor
  dht.begin();
}

void loop()
{
  if (!virtual_sensor)
  {
    // Read the temperature and humidity from the DHT sensor
    Serial.println("Fetching data from the Physical sensor");
    temperature = dht.readTemperature();
    if (isnan(Hartbeat) || isnan(SPO2) || isnan(temperature))
    {
      Serial.println("Failed to read from DHT !"); // Output error message to the serial console
      delay(10000);
      return;
    }
  }
  else
  {
    Serial.println("Fetching data from the Virtual sensor");
    
    Hartbeat = random(60, 120); // Random heart rate between 60 to 100 bpm
    SPO2 = random(90, 100);      // Random SpO2 between 90% to 100%
    temperature = random(200, 300) / 10.0; // Random temperature between 20.0 to 30.0 degrees Celsius
  }

  Serial.print("Hartbeat : ");
  Serial.println(Hartbeat);
  Serial.print("SPO2 : ");
  Serial.println(SPO2);
  Serial.print("Temperature : ");
  Serial.println(temperature);

  // Submit sensor data to Anedya server
  anedya_submitData("temperature", temperature); // Submit data to the Anedya
  anedya_submitData("Hartbeat",Hartbeat);
  anedya_submitData("SPO2", SPO2);

  Serial.println("-------------------------------------------------");
  delay(5000);
}

void connectToMQTT()
{
  while (!mqtt_client.connected())
  {
    const char *client_id = deviceID;
    Serial.print("Connecting to Anedya Broker....... ");
    if (mqtt_client.connect(client_id, mqtt_username, mqtt_password)) // Checks to check MQTT connection
    {
      Serial.println("Connected to Anedya broker");
    }
    else
    {
      Serial.print("Failed to connect to Anedya broker, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" Retrying in 5 seconds.");
      delay(5000);
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  char res[150] = "";

  for (unsigned int i = 0; i < length; i++)
  {
    res[i] = payload[i];
  }
  String str_res(res);
  if (str_res.indexOf("deviceSendTime") != -1)
  {
    timeRes = str_res;
  }
  else
  {
    submitRes = str_res;
    Serial.println(str_res);
  }
}

void setDevice_time()
{
  String timeTopic = "$anedya/device/" + String(deviceID) + "/time/json"; // Time topic will provide the current time from the Anedya server
  const char *mqtt_topic = timeTopic.c_str();

  if (mqtt_client.connected())
  {
    Serial.print("Time synchronizing......");

    boolean timeCheck = true; // Iteration to re-sync to ATS (Anedya Time Services), in case of failed attempt
    long long deviceSendTime;
    long long timeTimer = millis();

    while (timeCheck)
    {
      mqtt_client.loop();

      unsigned int iterate = 2000;
      if (millis() - timeTimer >= iterate) // Time to hold publishing
      {
        Serial.print(".");
        timeTimer = millis();
        deviceSendTime = millis();

        StaticJsonDocument<200> requestPayload;            // Declare a JSON document with a capacity of 200 bytes
        requestPayload["deviceSendTime"] = deviceSendTime; // Add a key-value pair to the JSON document
        String jsonPayload;                                // Declare a string to store the serialized JSON payload
        serializeJson(requestPayload, jsonPayload);        // Serialize the JSON document into a string

        const char *jsonPayloadLiteral = jsonPayload.c_str();
        mqtt_client.publish(mqtt_topic, jsonPayloadLiteral);
      }

      if (timeRes != "")
      {
        String strResTime(timeRes);
        DynamicJsonDocument jsonResponse(100);     // Declare a JSON document with a capacity of 100 bytes
        deserializeJson(jsonResponse, strResTime); // Deserialize the JSON response from the server into the JSON document

        long long serverReceiveTime = jsonResponse["serverReceiveTime"];
        long long serverSendTime = jsonResponse["serverSendTime"];

        long long deviceRecTime = millis();
        long long currentTime = (serverReceiveTime + serverSendTime + deviceRecTime - deviceSendTime) / 2;
        long long currentTimeSeconds = currentTime / 1000;

        setTime(currentTimeSeconds); // Set the device time based on the computed current time
        Serial.println("\n synchronized!");
        timeCheck = false;
      }
    }
  }
  else
  {
    connectToMQTT();
  }
}

void anedya_submitData(String datapoint, float sensor_data)
{
  boolean check = true;

  String strSubmitTopic = "$anedya/device/" + String(deviceID) + "/submitdata/json";
  const char *submitTopic = strSubmitTopic.c_str();
  while (check)
  {
    if (mqtt_client.connected())
    {
      if (millis() - submitTimer >= 2000)
      {
        submitTimer = millis();
        long long current_time = now();                     // Get the current time
        long long current_time_milli = current_time * 1000; // Convert current time to milliseconds

        String jsonStr = "{\"data\":[{\"variable\": \"" + datapoint + "\",\"value\":" + String(sensor_data) + ",\"timestamp\":" + String(current_time_milli) + "}]}";
        const char *submitJsonPayload = jsonStr.c_str();
        mqtt_client.publish(submitTopic, submitJsonPayload);
      }
      mqtt_client.loop();
      if (submitRes != "")
      {
        DynamicJsonDocument jsonResponse(100);    // Declare a JSON document with a capacity of 100 bytes
        deserializeJson(jsonResponse, submitRes); // Deserialize the JSON response from the server into the JSON document

        int errorCode = jsonResponse["errCode"];
        if (errorCode == 0)
        {
          Serial.println("Data pushed to Anedya!!");
        }
        else if (errorCode == 4040)
        {
          Serial.println("Failed to push data!!");
          Serial.println("Unknown variable Identifier");
          Serial.println(submitRes);
        }
        else
        {
          Serial.println("Failed to push data!!");
          Serial.println(submitRes);
        }
        check = false;
        submitTimer = 5000;
      }
    }
    else
    {
      connectToMQTT();
    }
  }
}