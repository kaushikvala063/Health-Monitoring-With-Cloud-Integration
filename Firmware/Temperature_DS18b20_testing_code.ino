#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is connected to GPIO 4 (D2 on NodeMCU)
#define ONE_WIRE_BUS D2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

void setup(void) {
  // Start serial communication for debugging
  Serial.begin(115200);
  Serial.println("Dallas Temperature Sensor Test");

  // Start the DS18B20 sensor
  sensors.begin();
}

void loop(void) {
  // Request temperature readings
  sensors.requestTemperatures();

  // Fetch and print the temperature in Celsius
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" °C");

  // Wait for a second before taking another reading
  delay(1000);
}