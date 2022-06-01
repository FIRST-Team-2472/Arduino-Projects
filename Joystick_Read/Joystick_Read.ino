#include <Servo.h>

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <Dabble.h>

int yDirPin = 5;
int xDirPin = 4;

int leftSparkPin = 5;
int rightSparkPin = 6;

const int GreenLED = 8;  //Digital pin 8
bool GreenMode = false;   // Start out disabled

Servo sparkLeft, sparkRight;

void setup() {
  // put your setup code here, to run once:

  Dabble.begin(9600);       //Enter baudrate of your bluetooth.Connect bluetooth on Bluetooth port present on evive.

  pinMode (GreenLED, OUTPUT);
  digitalWrite(GreenLED, HIGH);
  delay(1000);
  digitalWrite(GreenLED, LOW);

  sparkLeft.attach(leftSparkPin);
  sparkRight.attach(rightSparkPin);

  sparkLeft.write(90);
  sparkRight.write(90);
}

void loop() {

  dabbleStuff();
  readJoy();
  if (GreenMode) {
    if (!Dabble.isAppConnected())
      HardStop("Not Connected");
      
    double yDir = mapf(analogRead(yDirPin), 0.0, 1023.0, -1.0, 1.0);
    double xDir = mapf(analogRead(xDirPin), 0.0, 1023.0, -1.0, 1.0);
    arcadeDrive(yDir, xDir);
  } else {
    double forwardSpeed = mapf(GamePad.getYaxisData(), -7, 7, -1, 1);
    double turnSpeed = mapf(GamePad.getXaxisData(), -7, 7, -1, 1);
    arcadeDrive(forwardSpeed, turnSpeed);
  }
  delay(10);

}





void readJoy() {
  //max value 660 3v, 1014 5v
  //yDir = analogRead(yDirPin);
  //xDir = analogRead(xDirPin);
  

  
}

void arcadeDrive(double y, double x) {
  // y is the y axis of the joystick
  // x is the x axis of the SAME joystick
  if (absf(x) < .3)
    x = 0;
  if (absf(y) < .3)
    y = 0;

  if (absf(x) + absf(y) < 1) {
    tankDrive(y + x, y - x);

  } else {
    double betterX = (x / (absf(x) + absf(y)));
    double betterY = (y / (absf(x) + absf(y)));

    tankDrive(betterY + betterX, betterY - betterX);
  }
}

void tankDrive(double left, double right) {
  left = mapf(left,-1,1,0,180);
  right = mapf(right,-1,1,0,180);

  sparkLeft.write(left);
  sparkRight.write(right);
}

void dabbleStuff() {
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
}

void HardStop(String error)
{
  sparkLeft.detach();
  sparkRight.detach();
  Serial.println(error);
  while (true)
  {
    digitalWrite(GreenLED, HIGH);
    delay(500);
    digitalWrite(GreenLED, LOW);
    delay(500);
  }
}

//agussted the range for a number
double mapf(double x, double in_min, double in_max, double out_min, double out_max) {
  double result;
  result = (x - in_min) / (in_max - in_min) * (out_max - out_min) + out_min;
  return result;
}

double absf(double x) {
  if(x < 0)
    return -x;
  return x;
}
