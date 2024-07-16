# Health-Monitoring-With-Cloud-Integration
This project involves developing a system that monitors real-time health metrics, including heart rate, SpO2, and body temperature, and uploads the data to the cloud for remote access and analysis.

Overview
This repository contains the code and documentation for a comprehensive Health Monitoring and Reporting System. The system uses a NodeMCU (ESP8266) to collect Hartbeat,SPO2 and body temperature data from various sensors, stores the data on Anedya IoT Cloud, and visualizes it in real-time using a Streamlit dashboard.
Features Real-time Monitoring: Measures Hartbeat,SPO2 and body temperature
Cloud Storage: Stores data on Anedya IoT Cloud for remote access and long-term storage.
Interactive Dashboard: Visualizes data using a Streamlit-based web application.



Components
NodeMCU (ESP8266)
MAX30100 Sensor (Hartbeat,SPO2)
DS18B20 Sensor(Body temperature)



Setup Instructions
Hardware Connections

Max30100 sensor

VCC to +5V
GND to GND
SCL pin D1
SDA pin D2

DS18B20 Sensor:
VCC to +5V
GND to GND
Data pin D3




Install Required Libraries
#include <ESP8266WiFi.h> #include <PubSubClient.h> #include <WiFiClientSecure.h> #include <ArduinoJson.h> #include <TimeLib.h> #include <OneWire.h> #include <DallasTemperature.h> #include <Wire.h> #include "MAX30100_PulseOximeter.h"



Upload Code to NodeMCU
Open the Health Monitering System.ino file in the Arduino IDE. Configure your WiFi credentials and Anedya IoT Cloud API endpoint in the code. Then upload the code to your NodeMCU. This file is available in Firmware folder.


