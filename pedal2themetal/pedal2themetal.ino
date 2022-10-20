#include <HX711.h> //https://github.com/RobTillaart/HX711
#include <Joystick.h> //https://github.com/MHeironimus/ArduinoJoystickLibrary/
//Define Inputs for HX711 outputs
HX711 scale;
uint8_t dataPin = 2;
uint8_t clockPin = 3;

//Define joystick type
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_JOYSTICK,0,0,false,false,false,true,false,false,false,true,false,true,false);

int lastButtonState[4] = {0,0,0,0};

float calibration_factor = -7050;
float brake;

void setup() {
  //Create an additional VCC using PIN 4
  pinMode(4,OUTPUT); //define a digital pin as output
  digitalWrite(4, HIGH); // set the above pin as HIGH so it acts as 5V

  //Create an additional VCC using PIN A2
  pinMode(A2,OUTPUT); //define an analog pin as output
  digitalWrite(A2, HIGH); // set the above pin as HIGH so it acts as 5V
  
  //Throttle output analog pin
  pinMode(A7,OUTPUT);
  //Break output analog pin
  pinMode(A8,OUTPUT);
  //Clutch output analog pin
  pinMode(A9,OUTPUT);

  //Initialize LoadCell
  ////LoadCell.begin();
  ////LoadCell.start(2500); // tare preciscion can be improved by adding a few seconds of stabilising time
  ////LoadCell.setCalFactor(150.0); // user set calibration factor (float) 150.0
  scale.begin(dataPin, clockPin);
  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average();

  //Initialize Joystick
  Joystick.begin();
  //Set pedal ranges
  Joystick.setThrottleRange(102,570);
  Joystick.setBrakeRange(3,85);
  Joystick.setRxAxisRange(10,190);

  //Initialize serial comm
  Serial.begin(28800);
}
  
void loop() {
  //Read values for brake (LoadCell+HX711)
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  brake = scale.get_units(), 10;
  //If brake value is lower than 3, set it to zero (deadzone). Else, increase (almost logarithmic).
  //The scale values are between 0 and 50, but we're setting the limit to 30
  //The brake values are between 0 and 1024
  //  if(brake < 3) { brake = 0; }
  //  else { brake = brake * 12; } // 1024/30=34.1
  Joystick.setBrake(brake);
  analogWrite(A8,brake);

  //Read values for throttle (HAL sensor)
  unsigned int throttle=(analogRead(A0));
  Joystick.setThrottle(throttle);
  analogWrite(A7,throttle);

  //Read values for clutch (Potentiometer)
  unsigned int clutch=(analogRead(A1));
  Joystick.setRxAxis(clutch);
  analogWrite(A9,clutch);
}
