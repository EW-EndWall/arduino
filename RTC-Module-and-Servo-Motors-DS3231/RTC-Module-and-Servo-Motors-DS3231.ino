// #include <Wire.h> // print
#include <RTClib.h>
#include <Servo.h>
#include <LowPower.h>
// obj
RTC_DS3231 rtc; // DS3231 RTC module object
volatile bool wakeUpFlag = false; // For interrupt from SQW pin

Servo servo;  // servo motor object
// pins
const int servoPin = 9;  // Servo pin number
const int buttonPin = 2;  // Button pin number
const int ledPin = 4;  // led pin number
const int sqwPin = 3;  // sqwPin pin number
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
int checkInterval = 1; // 15 minute 
// status
int servoPosition = closeAngle;  // Servo motor position
// start
void setup() {
  Serial.begin(9600);
  // start RTC
  if (!rtc.begin()) { 
    // RTC connection failed
    Serial.println("RTC connection failed.");
    while (1);
  }

  if (rtc.lostPower()) { 
    // If RTC is not working, adjust the clock
    // Sets the clock based on Arduino compile time
    Serial.println("RTC reset.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // attach pins
  servo.attach(servoPin);  // Connect servo motor to pin
  pinMode(buttonPin, INPUT);  // Button pin input
  pinMode(ledPin, OUTPUT); // led pin
  attachInterrupt(digitalPinToInterrupt(buttonPin), wakeUpButton, FALLING); // Define interrupt for button
  pinMode(sqwPin, INPUT_PULLUP); // Set SQW Pin as interrupt pin
  attachInterrupt(digitalPinToInterrupt(sqwPin), wakeUp, FALLING);

  // first test
  Serial.println("First test.");
  servo.write(servoPosition); //Initial servo position
  delay(1000); // 1sn wait
  openServo(); // open
  delay(1000); // 1sn wait
  closeServo(); // close
  delay(1000); // 1sn wait

  // Set RTC alarm (every 15 minutes)
  rtc.writeSqwPinMode(DS3231_OFF); // Disable SQW output
  rtc.clearAlarm(1);               // Clear alarm 1
  rtc.clearAlarm(2);               // Clear alarm 2
  rtc.disableAlarm(1);             // Disable alarm 1
  rtc.disableAlarm(2);             // Disable alarm 2

  // Alarm 1: Every 15 minutes
  rtc.setAlarm1(rtc.now() + TimeSpan(checkInterval * 60), DS3231_A1_Minute);
  Serial.println("15 minute alarm set.");
}

void loop() {
  Serial.println("Loop!");
  checkTimeServo();
  sleep();
}

void sleep() {
  delay(1000);
  // Sleep mode
  if (wakeUpFlag) {
    wakeUpFlag = false; // Reset flag
    Serial.println("Wake Up Flag!");
    delay(1000);
  }

  // Enter sleep mode
  Serial.println("Sleep mode.");
  delay(150); // wait
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  delay(150); // wait
}

void openServo() {
  Serial.println("Servo open.");
  // servo is open
  if (servoPosition == closeAngle){
    servo.write(openAngle);
    // Reverse servo position
    servoPosition = openAngle;
  }
}

void closeServo() {
  Serial.println("Servo close.");
  // servo is close
  if (servoPosition == openAngle){
    servo.write(closeAngle);
    // Reverse servo position
    servoPosition = closeAngle;
  }
}

// Interrupt function (wake up by button)
void wakeUpButton() {
  Serial.println("Wake up button!");
  buttonCheck();
}
void wakeUp() {
  Serial.println("Wake Up!");
  wakeUpFlag = true;
}

void buttonCheck() {
  Serial.println("Button click.");
  // LED indicator
  digitalWrite(ledPin, HIGH);
  // Change servo position
  if (servoPosition == closeAngle){
    openServo();
  }else{
    closeServo();
  }
  // Button delay
  delay(500);
  digitalWrite(ledPin, LOW);
}

void checkTimeServo() {
  Serial.println("Check time servo.");
  DateTime now = rtc.now(); // get RTC data
  // Servo on/off at specific time interval
  if (now.hour() == openServoDate_Hour && now.minute() == openServoDate_Minute)
    openServo();
  else if (now.hour() == closeServoDate_Hour && now.minute() == closeServoDate_Minute)
    closeServo();
}