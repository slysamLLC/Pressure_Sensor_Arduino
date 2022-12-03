/*********************************************************************
 This sketch controls an electronic circuit that can sense 
 the environment using a sensor (electronic components that convert 
 real-world measurements into electrical signals). The device 
 processes the information from the sensors with the behavior 
 described in the sketch. The device will then be able to interact 
 with the world by using actuators, electronic components that can 
 convert an electric signal into physical action.
 
 EETechFix
*********************************************************************/

#include "arduino_secrets.h"
/*
  Sketch generated by the Arduino IoT Cloud 
  https://create.arduino.cc/cloud

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  float batteryVal;
  float psiVal;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // display width, in pixels
#define SCREEN_HEIGHT 64 // display height, in pixels
#define OLED_RESET     -1 // same as system reset
#define SCREEN_ADDRESS 0x3D // OLED I2C address

const int sensorPin = A1;
const float alpha = 0.95; // Low Pass Filter alpha (0 - 1 )
const float aRef = 3.3; // analog reference
float filteredVal = 2048.0; // midway starting point
float sensorVal;
float voltage;

const float alpha_2 = 0.95; // Low Pass Filter alpha (0 - 1 )
float filteredVal_2 = 2048.0; // midway starting point
float sensorVal_2;
float voltage_2;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(4000);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information youâll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  analogReadResolution(12);

  //SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1); // Don't proceed, wait forever
  }
  display.clearDisplay();
  display.display();
}

void loop() {

  ArduinoCloud.update();

  sensorVal = (float)analogRead(sensorPin);
  filteredVal = (alpha * filteredVal) + ((1.0 - alpha) * sensorVal); // Low Pass Filter
  voltage = (filteredVal / 4096.0) * aRef;
  float psi = (37.5939 * voltage) - 13.5338; // y=mx+b
  psiVal = roundoff(psi, 1);

  updateOLED ();

  sensorVal_2 = (float)analogRead(ADC_BATTERY);
  filteredVal_2 = (alpha_2 * filteredVal_2) + ((1.0 - alpha_2) * sensorVal_2); // Low Pass Filter
  voltage_2 = (filteredVal_2 / 4096.0) * aRef;
  float batteryVolts = (1.275 * voltage_2);
  batteryVal = roundoff(batteryVolts, 1);

  Serial.print("raw_adc "); Serial.println(sensorVal, 0);
  Serial.print("filtered_adc "); Serial.println(filteredVal, 0);
  Serial.print("gpsi= "); Serial.println(psi, 1);
  Serial.print("BatteryVoltage "); Serial.println(batteryVal, 1);
  Serial.println("  ");
  
  delay (100);
}

void updateOLED () {
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.print("filtered_adc ");
  display.print(filteredVal, 0);

  display.setCursor(0, 15);
  display.print("voltage ");
  display.print(voltage, 2);
  display.print(" Volts");

  display.setTextSize(3);
  display.setCursor(0, 30);
  display.print(psiVal, 0);
  display.print(" psi");
  display.display();
}

float roundoff(float value, unsigned char prec)
{
  float pow_10 = pow(10.0f, (float)prec);
  return round(value * pow_10) / pow_10;
}















































