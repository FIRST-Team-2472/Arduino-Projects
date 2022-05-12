#include <Servo.h>

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <Dabble.h>

int yDirPin = 5;
int xDirPin = 4;
double yDir = 0, xDir = 0;

int leftSparkPin = 5;
int rightSparkPin = 6;

const int GreenLED = 8;  //Digital pin 8
bool GreenMode = false;   // Start out disabled

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
boolean newData = false, debugSpeed = false, debugJoy = false;

Servo sparkLeft, sparkRight;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(57600);      // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin(9600);       //Enter baudrate of your bluetooth.Connect bluetooth on Bluetooth port present on evive.
  Serial.println("Online");

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

  recvMessage();
  dabbleStuff();
  readNewData();
  readJoy();
  if (GreenMode) {
    if (!Dabble.isAppConnected())
      HardStop("Not Connected");

    arcadeDrive(yDir, xDir);
  } else {
    double forwardSpeed = mapf(GamePad.getYaxisData(), -7, 7, -1, 1);
    double turnSpeed = mapf(GamePad.getXaxisData(), -7, 7, -1, 1);
    arcadeDrive(forwardSpeed, turnSpeed);
  }
  delay(50);

}





void readJoy() {
  //max value 660 3v, 1014 5v
  //yDir = analogRead(yDirPin);
  //xDir = analogRead(xDirPin);
  yDir = mapf(analogRead(yDirPin), 0.0, 1023.0, -1.0, 1.0);
  xDir = mapf(analogRead(xDirPin), 0.0, 1023.0, -1.0, 1.0);

  if (debugJoy) {
    Serial.print("Y Dir: ");
    Serial.print(yDir);
    Serial.print(",");
    Serial.print(analogRead(yDirPin));
    Serial.print("; X Dir: ");
    Serial.print(xDir);
    Serial.print(",");
    Serial.print(analogRead(xDirPin));
    Serial.println();
  }
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

void newArcadeDrive(double rotate, double drive) {
  double maximum = max(abs(drive), abs(rotate));
  double total = drive + rotate;
  double difference = drive - rotate;

  if (drive >= 0) {
    if (drive >= 0) {
      if (rotate >= 0)
        tankDrive(maximum, difference);
      else
        tankDrive(total, maximum);
    } else {
      if (rotate >= 0)
        tankDrive(total, -maximum);
      else
        tankDrive(-maximum, difference);
    }
  }
}

void tankDrive(double left, double right) {
   /*if (abs(left) < .1)
    left = 0;
  if (abs(right) < .1)
    right = 0;*/
  left = mapf(left,-1,1,0,180);
  right = mapf(right,-1,1,0,180);
  if (debugSpeed) {
    Serial.print("Left Speed: ");
    Serial.print(left);
    Serial.print(", Right Speed: ");
    Serial.println(right);
    Serial.print("Left Real: ");
    Serial.print(sparkLeft.read());
    Serial.print(", Right Real: ");
    Serial.println(sparkRight.read());
  }

  sparkLeft.write(left);
  sparkRight.write(right);
}

//no idea how this works
void recvMessage() {
  static int index = 0;
  char enter = '\n';
  char input;

  // send data only when you receive data:
  while (Serial.available() > 0 && newData == false) {
    input = Serial.read();

    //knows when message is over because arduino always puts '\n' or enter at end
    if (input != enter) {
      //adds indvudal chars it receices and adds that to result
      receivedChars[index] = input;
      index++;
      if (index >= numChars) {
        index = numChars - 1;
      }
    }
    else {
      //'\0' marks the end of a string so "I like \0 vape" would show "I like "
      receivedChars[index] = '\0'; // terminate the string
      index = 0;
      newData = true;
    }
  }
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

void readNewData() {

  if (newData == true) {
    String message = receivedChars;
    newData = false;

    if (message.equalsIgnoreCase("Debug Speed"))
      debugSpeed = true;
    else if (message.equalsIgnoreCase("Debug Joystick"))
      debugJoy = true;
    else if (message.equalsIgnoreCase("Debug Hide")) {
      debugSpeed = false;
      debugJoy = false;
    }
    else if (message.equalsIgnoreCase("Help")) {
      Serial.println();
      Serial.println("Debug Speed: shows speeds sending to mortor");
      Serial.println("Debug Joystick: shows vaules recived by joystick");
      Serial.println("Debug Hide: hides any debug info");
      Serial.println("Help: your just used it moron");
    }
    else
      Serial.println("ERROR 104: " + message + " not a regonized command");

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
