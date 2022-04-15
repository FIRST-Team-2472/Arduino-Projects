const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

void setup() {
  
  Serial.begin(9600);
  Serial.println("Online");
}

void loop() {



  //myStringMaker();
  recvMessage();
  showNewData();

}

//Can't get this to work
/*void myStringMaker() {
  int index = 0;
  // send data only when you receive data:
  while(Serial.available() && newData == false){
    if((char)Serial.read() == '\n') {
      receivedChars[index] = '\0';
      index = 0;
      newData = true;
    }else {
      // read the incoming byte:
      int input = Serial.read();
      receivedChars[index] = input;
      index++;
    }
  }
}*/

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
            //believe this is to get rid of overflow from prevuse message
            receivedChars[index] = '\0'; // terminate the string
            index = 0;
            newData = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
}
