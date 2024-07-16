# Health-Monitoring-With-Cloud-Integration-using-NodeMCU-Anedya-IoT-Cloud
This project involves developing a system that monitors real-time health metrics, including heart rate, SpO2, and body temperature, and uploads the data to the cloud for remote access and analysis.

Overview
<div>
This repository contains the code and documentation for a comprehensive Health Monitoring and Reporting System. The system uses a NodeMCU (ESP8266) to collect Hartbeat,SPO2 and body temperature data from various sensors, stores the data on Anedya IoT Cloud, and visualizes it in real-time using a Streamlit dashboard.
Features Real-time Monitoring: Measures Hartbeat,SPO2 and body temperature
Cloud Storage: Stores data on Anedya IoT Cloud for remote access and long-term storage.
Interactive Dashboard: Visualizes data using a Streamlit-based web application.

</div>

Components
<div>
NodeMCU (ESP8266)</div>
<div>
MAX30100 Sensor (Hartbeat,SPO2)</div>
<div>
DS18B20 Sensor(Body temperature)</div>



Setup Instructions
<br>
Hardware Connections

Max30100 sensor

VCC to +5V<br>
GND to GND<br>
SCL pin D1<br>
SDA pin D2<br>

DS18B20 Sensor:
VCC to +5V<br>
GND to GND<br>
Data pin D3<br>




Install Required Libraries
#include <ESP8266WiFi.h> #include <PubSubClient.h> #include <WiFiClientSecure.h> #include <ArduinoJson.h> #include <TimeLib.h> #include <OneWire.h> #include <DallasTemperature.h> #include <Wire.h> #include "MAX30100_PulseOximeter.h"
<br>

Upload Code to NodeMCU
<div>

Open the Health Monitering System.ino file in the Arduino IDE. Configure your WiFi credentials and Anedya IoT Cloud API endpoint in the code. Then upload the code to your NodeMCU. This file is available in Firmware folder.
</div>


