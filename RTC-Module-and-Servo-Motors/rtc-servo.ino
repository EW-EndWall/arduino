// #include <Wire.h> // print
#include <RTClib.h>
#include <Servo.h>
#include <LowPower.h>
// obj
RTC_DS1307 rtc; // DS1307 RTC module object
Servo servo;  // servo motor object
// pins
const int servoPin = 9;  // Servo pin number
const int buttonPin = 2;  // Button pin number
const int ledPin = 4;  // led pin number
// settings
int openAngle = 0;
int closeAngle = 90;
// open servo
int openServoDate_Hour = 19; // hour
int openServoDate_Minute = 0; // minute
// close servo
int closeServoDate_Hour = 19; // hour
int closeServoDate_Minute = 15; // minute 
// control at intervals
int checkInterval = 900000; // 15 minute 
// status
int servoPosition = closeAngle;  // Servo motor position
volatile bool buttonState = false;  // Button status
bool lastButtonState = false;  // Previous button status
// start
void setup() {
  Serial.begin(9600);
  // start RTC
  if (!rtc.begin()) 
    // RTC connection failed
    while (1);
  if (!rtc.isrunning()) 
    // If RTC is not working, adjust the clock
    // Sets the clock based on Arduino compile time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // attach pins
  servo.attach(servoPin);  // Connect servo motor to pin
  pinMode(buttonPin, INPUT);  // Button pin input
  pinMode(ledPin, OUTPUT); // led pin
  attachInterrupt(digitalPinToInterrupt(buttonPin), wakeUp, FALLING); // Define interrupt for button
  // first test
  servo.write(servoPosition); //Initial servo position
  delay(1000); // 1sn wait
  openServo(); // open
  delay(1000); // 1sn wait
  closeServo(); // close
  delay(1000); // 1sn wait
}

void loop() {
  buttonCheck();
  checkTimeServo();
  sleep();
}

void sleep(){
  // Sleep mode
  // LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
  // Put the device to sleep mode for 8 seconds, then continue checking
  for (int i = 0; i < checkInterval / 8000; i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  
  }
  delay(150); // 1sn wait
}

void openServo() {
  // servo is open
  if (servoPosition == closeAngle){
    servo.write(openAngle);
    // Reverse servo position
    servoPosition = openAngle;
  }
}

void closeServo() {
  // servo is close
  if (servoPosition == openAngle){
    servo.write(closeAngle);
    // Reverse servo position
    servoPosition = closeAngle;
  }
}

// Interrupt function (wake up by button)
void wakeUp() {
  lastButtonState = !lastButtonState; // Update button status
}

void buttonCheck(){
  buttonState = digitalRead(buttonPin); // Button status is read
  // When the button is pressed and was not pressed in the previous state
  if (buttonState && !lastButtonState) {
    // LED indicator
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    // Change servo position
    if (servoPosition == closeAngle) 
      openServo();
      else 
      closeServo();
    // Button delay
    delay(150);
  }
  lastButtonState = buttonState; // Update button status
}

void checkTimeServo(){
  DateTime now = rtc.now(); // get RTC data
  // Servo on/off at specific time interval
  if (now.hour() == openServoDate_Hour && now.minute() == openServoDate_Minute)
    openServo();
  else if (now.hour() == closeServoDate_Hour && now.minute() == closeServoDate_Minute)
    closeServo();
}