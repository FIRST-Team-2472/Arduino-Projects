#include <Servo.h>


int yDirPin = 0;
int xDirPin = 1;
float yDir = 0, xDir = 0;

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
boolean newData = false, debugSpeed = false, debugJoy = false;

Servo spark1, spark2;

void setup() {
  // put your setup code here, to run once:
  spark1.attach(0);
  spark2.attach(1);
  Serial.begin(9600);
  Serial.println("Online");
}

void loop() {

  recvMessage();
  readNewData();
  readJoy();
  arcadeDrive(yDir,xDir,1);
  delay(10);
  
}





void readJoy() {
  //max value 660 3v, 1014 5v
  yDir = mapf((float)analogRead(yDirPin), 0, 1014, -1, 1);
  xDir = mapf((float)analogRead(xDirPin), 0, 1014, -1, 1);

  yDir*= abs(yDir); //makes the input exponnatial instead of linear
  xDir*= abs(xDir);

  if(debugJoy) {
    Serial.print("Y Dir: ");
    Serial.print(yDir);
    Serial.print(", X Dir: ");
    Serial.println(xDir);
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
    // limits the motors from ever going over 75% speed
    double betterX = (x / (abs(x) + abs(y))) * maxSpeed;
    double betterY = (y / (abs(x) + abs(y))) * maxSpeed;

    tankDrive(betterY + betterX, betterY - betterX);
  }
}

void tankDrive(double left, double right) {
  if(debugSpeed) {
    Serial.print("Left Speed: ");
    Serial.print(left);
    Serial.print(", Right Speed: ");
    Serial.println(right);
    Serial.println();
  }
  
  spark1.write(left);
  spark2.write(right);
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

//agussted the range for a number
float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  float result;
  result = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  return result;
}
