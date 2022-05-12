// This sketch is used to test and run the scooter for Team 2472

/*
   Gamepad module provides three different mode namely Digital, JoyStick and Accerleometer.

   You can reduce the size of library compiled by enabling only those modules that you want to
   use. For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

   Explore more on: https://thestempedia.com/docs/dabble/game-pad-module/
*/
// The dabble app can be downloaded from the Play Store under the name
// "Arduino & ESP32 Bluetooth Controller App - Dabble"
// Searching on "Dabble" should also work

// Setting up to build the code for the Arduino can be learned from the website
//   https://thestempedia.com/docs/dabble/getting-started-with-dabble/
//   This site points to a zip file which contains both the libraries needed
//     and example applications for different dabble interfaces.

// These statements set up Dabble to use the GamePad.
//   Once in the gamepad screen, the controls are as follows:
//    1. Change the gamepad into joystick mode
//    2. The Triangle Key enables "Green Mode" In Green Mode,
//      the joystick on the scooter is enabled and the green LED is lit
//    3. Green Mode continues until one of 2 things happens: First, the "X"
//      on the gamepad is hit, Green Mode is exited and the scooter stops. 
//      Secondly, if Bluetooth communications stops for more than 1 second,
//      the Green Mode is exited and the scooter stops. 
//    4. When not in Green Mode, the joystick on the gamepad is 
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <Dabble.h>

#include <Servo.h>

// These paramgers are related to the analog joystick
//  The X and Y (Left/Right and Forward/Reverse) are
//    brought in to analog pins.
//  At startup, the 'center' values are noted - the joystick
//    does not actually rest at 0,0 
//  JoyX and JoyY are normalized intermediate values
//    in the range -500 to +500

const int JoyLRPin = 5;   // Joystick Left/Right pin
const int JoyFRPin = 4;   // Joystik Forward/Reverse pin

int centerX;    // Center value for X axis
int centerY;    // Center value for Y axis
int JoyX;       // Normalized joystick X value
int JoyY;       // Normalized joystick Y value

// The PWM signals to the Spark controllers 

const int LeftSparkPin = 5;  // Spark for left wheel
const int RightSparkPin = 6; // Spark for right wheel

// Two millisecond timers are used in the code;
//  'watchdog' is used to disable the scooter if bluetooth is lost
//  'timer' is used to time debugging output to the serial monitor

long watchdog;  // 
long timer;     // Timer for 2 second reporting


Servo ServoLeft;   //Left wheel servo  (Spark)
Servo ServoRight;  //Right wheel servo (Spark)

// Green LED - indicator that joystick is active

const int GreenLED = 8;  //Digital pin 8

// GreenMode flag - True if joystick active

bool GreenMode = false;   // Start out disabled

//**********************************************************************
// setup
//
// Executed once at startup

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);      // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin(9600);       //Enter baudrate of your bluetooth.Connect bluetooth on Bluetooth port present on evive.
  Serial.println("Started");
  timer = millis();

  // Initialize the green LED and flash it for 1 second
  
  pinMode (GreenLED, OUTPUT);
  digitalWrite(GreenLED, HIGH);
  delay(1000);
  digitalWrite(GreenLED, LOW);

  

  // At startup, we assume that the joystick is inactive -
  //  For safety, if the joystick is not centered, hard stop
  centerX = analogRead(JoyLRPin);   // zero value
  centerY = analogRead(JoyFRPin);   // zero value
  Serial.print("X = ");
  Serial.print(centerX);
  Serial.print(" Y = ");
  Serial.println(centerY);
  if ((abs(centerX - 512) > 150) || (abs(centerY - 512) > 150))
    HardStop("Joystick");

  ServoLeft.attach(LeftSparkPin, 1000, 2000);   //1000-2000 microsecond pulses
  ServoRight.attach(RightSparkPin, 1000, 2000); //1000-2000 microsecond pulses

  ServoLeft.write(90);    // All off to start
  ServoRight.write(90);   // All off to start
  
  

}

void loop() {

  float LeftSpeed;    // intermediate speed value (centered on 0)
  float RightSpeed;   // intermediate speed value (centered on 0)

  // These are parameters used to modulate the speed for 
  //  controllability.  Smaller values limit forward and turn speeds
  
  float ScaleSpeed = .8;  // 1 = full speed; 0 = disable
  float ScaleTurn = .6;   // 1 = full turn; 0 = disable
  
  // These are the actual values output to the PWM (Spark)
  //  The values will be 0-180 with 90 being off

  int LeftServo;        // Left servo value (0-180)
  int RightServo;       // Right servo value (0-180)

  // These values are used in case the Sparks output at 90 degrees
  //   change these from 0 until the scooter is still at neutral joystick

  const int LeftServoBias = 0;
  const int RightServoBias = 0;
  

  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.

  if ((GamePad.isTrianglePressed()) || (GamePad.isStartPressed()))
  {
    GreenMode = true;
    digitalWrite(GreenLED, HIGH);
    Serial.println("GreenMode ON");
  }

  if (GamePad.isCrossPressed())
  {
    GreenMode = false;
    digitalWrite(GreenLED, LOW); 
    Serial.println("GreenMode OFF");
  }

  //  In Green Mode, use the joystick to control the scooter
  if (GreenMode)
  {
    // This code doesn't seem to work - contacting Dabble for answer
    if (!Dabble.isAppConnected())
    {
      HardStop("Not Connected");
    }

    // Now process the joystick input
    // Pick up the joystick values
    // Joystick values will be 0-1023
    // The center values were saved at startup
    //   Subtracting out the center values
    //      will give us values around -500 to +500
    //    Then we elimiate the deadband in the middle
    //    The wheel servos want values 0-180 degrees
    //      with 90 degrees being off - negative backward
    //      and positive forward. 
    //    The forward/back values go to both wheels
    //    The left/right get added/subtracted
    
    // Read the joystick - normalize on the center values
    
    JoyX = analogRead(JoyLRPin) - centerX; 
    JoyY = analogRead(JoyFRPin) - centerY;
  
    // Eliminate the deadband area
    
    if (abs(JoyX) < 20) JoyX = 0;  // enforce deadbanc on X
    if (abs(JoyY) < 20) JoyY = 0;  // enforce deadbanc on Y
  
    // Calculate the left/right values for speed
    // Speeds will be in the range -500 to +500 or so
    // ScaleSpeed and ScaleTurn are parameters to 
    //   cap the speed and  turn rate. They should have 
    //   values between 0 and 1
  
    LeftSpeed = JoyY * ScaleSpeed + JoyX * ScaleTurn;  
    RightSpeed = JoyY * ScaleSpeed - JoyX * ScaleTurn;
  
    // scale the servo paraters into the range of 0 - 180
  
    LeftServo = map(LeftSpeed, -500.0, +500.0, 0, 180.0);
    RightServo = map (RightSpeed, -500.0, +500.0, 0, 180.0);
  }

  //*********************************************************
  //  If not in green mode, control via the gamepad on phone
  
  else 
  {
    LeftSpeed = GamePad.getYaxisData() * ScaleSpeed + GamePad.getXaxisData() * ScaleTurn;  
    RightSpeed = GamePad.getYaxisData() * ScaleSpeed - GamePad.getXaxisData() * ScaleTurn;
  
    // scale the servo paraters into the range of 0 - 180
    // Full range on the gamepad is about -7 to +7
    
    LeftServo = map(LeftSpeed, -7.0, +7.0, 0, 180.0);
    RightServo = map (RightSpeed, -7.0, +7.0, 0, 180.0);

  }
  // Output the values to the servos

  ServoLeft.write(LeftServo - LeftServoBias);
  ServoRight.write(RightServo - RightServoBias);
  
  // Every 2 seconds write parameters to serial monitor
  
  if (millis() > timer + 2000)
  {
    timer = millis();
    Serial.print (" X = ");
    Serial.print (JoyX);
    Serial.print (" Y = ");
    Serial.print (JoyY);
    Serial.print (" Left = ");
    Serial.print (LeftServo);
    Serial.print (" Right = ");
    Serial.println (RightServo);
    
    
  }
  

}

// ******************************************************
//  HardStop - Emergency stop in safe mode
//    detach wheel servos (disable)
//    Flash green LED forever
//    Do Not Return

void HardStop(char* error)
{
  ServoLeft.detach();
  ServoRight.detach();
  Serial.println(error);
  while (true)
  {
    digitalWrite(GreenLED, HIGH);
    delay(500);
    digitalWrite(GreenLED, LOW);
    delay(500);
  }
}
