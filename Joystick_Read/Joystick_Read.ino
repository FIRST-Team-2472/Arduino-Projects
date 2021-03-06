#include <Servo.h>

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <Dabble.h>

int yDirPin = 4;
int xDirPin = 5;

int leftSparkPin = 5;
int rightSparkPin = 6;

const int GreenLED = 8;  //Digital pin 8
bool GreenMode = false;   // Start out disabled
int maxSpeed = 45;

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
    if (absf(xDir) < .3)
      xDir = 0;
    if (absf(yDir) < .3)
      yDir = 0;
      
    arcadeDrive(yDir, xDir);
  } else {
    double forwardSpeed = mapf(GamePad.getYaxisData(), -7, 7, -1, 1);
    double turnSpeed = -mapf(GamePad.getXaxisData(), -7, 7, -1, 1);
    arcadeDrive(forwardSpeed, turnSpeed);
  }
  delay(10);

}





void readJoy() {
  //max value 660 3v, 1014 5v
  //yDir = analogRead(yDirPin);
  //xDir = analogRead(xDirPin);



}

void arcadeDrive(double x, double y) {
  // y is the y axis of the joystick
  // x is the x axis of the SAME joystick


  if (absf(x) + absf(y) < 1) {
    tankDrive(y + x, y - x);

  } else {
    double betterX = (x / (absf(x) + absf(y)));
    double betterY = (y / (absf(x) + absf(y)));

    tankDrive(betterY + betterX, betterY - betterX);
  }
}

void tankDrive(double right, double left) {
  left = mapf(left, -1, 1, 90-maxSpeed, 90+maxSpeed);
  right = mapf(right, -1, 1, 90-maxSpeed, 90+maxSpeed);

  if (sparkLeft.read() > left)
    sparkLeft.write(sparkLeft.read() - 1);
  else if (sparkLeft.read() < left)
    sparkLeft.write(sparkLeft.read() + 1);

  if (sparkRight.read() > right)
    sparkRight.write(sparkRight.read() - 1);
  else if (sparkRight.read() < right)
    sparkRight.write(sparkRight.read() + 1);
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

  if (GamePad.isSquarePressed())
  {
    if(maxSpeed < 90) maxSpeed += 1;
  }

  if (GamePad.isCirclePressed())
  {
    if(maxSpeed > 0) maxSpeed-=1;
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
  if (x < 0)
    return -x;
  return x;
}
