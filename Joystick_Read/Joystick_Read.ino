#include <Servo.h>

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <Dabble.h>

int yDirPin = 5;
int xDirPin = 4;
float yDir = 0, xDir = 0;

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
  if(GreenMode) {
     if(!Dabble.isAppConnected())
      HardStop("Not Connected");
  
    arcadeDrive(yDir,xDir,1);
  }else {
    double forwardSpeed = mapf(GamePad.getYaxisData(),-7,7,-1,1);
    double turnSpeed = mapf(GamePad.getXaxisData(),-7,7,-1,1);
    arcadeDrive(forwardSpeed,turnSpeed,1);
  }
  //delay(500);
  
}





void readJoy() {
  //max value 660 3v, 1014 5v
  //yDir = analogRead(yDirPin);
  //xDir = analogRead(xDirPin);
  yDir = mapf((float)analogRead(yDirPin), 0, 1023, -1.0, 1.0);
  xDir = mapf((float)analogRead(xDirPin), 0, 1023, -1.0, 1.0);

  if(debugJoy) {
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

void arcadeDrive(double y, double x, double maxSpeed) {
  // y is the y axis of the joystick
  // x is the x axis of the SAME joystick
  if (abs(x) < .1)
    x = 0;
  if (abs(y) < .1)
    y = 0;

  if (abs(x) + abs(y) < maxSpeed) {
    tankDrive(y + x, y - x);
    
  } else {
    double betterX = (x / (abs(x) + abs(y))) * maxSpeed;
    double betterY = (y / (abs(x) + abs(y))) * maxSpeed;

    tankDrive(betterY + betterX, betterY - betterX);
  }
}

void tankDrive(double left, double right) {
  left = mapf(left,-1.0,1.0,0,180.0);
  right = mapf(right,-1.0,1.0,0,180.0);
  if(debugSpeed) {
    Serial.print("Left Speed: ");
    Serial.print(left);
    Serial.print(", Right Speed: ");
    Serial.println(right);
    Serial.println();
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
        
        if(message.equalsIgnoreCase("Debug Speed"))
          debugSpeed = true;
        else if(message.equalsIgnoreCase("Debug Joystick"))
          debugJoy = true;
        else if(message.equalsIgnoreCase("Debug Hide")){
          debugSpeed = false;
          debugJoy = false;
        }
        else if(message.equalsIgnoreCase("Help")){
          Serial.println();
          Serial.println("Debug Speed: shows speeds sending to mortor");
          Serial.println("Debug Joystick: shows vaules recived by joystick");
          Serial.println("Debug Hide: hides any debug info");
          Serial.println("Help: your just used it moron");
        }
        else
        Serial.println("ERROR 104: "+ message + " not a regonized command");

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
  result = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  return result;
}
