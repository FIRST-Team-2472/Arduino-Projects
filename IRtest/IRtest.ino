#include <IRremote.h>
 
#define first_key  48703 
#define second_key  58359 
#define third_key  539 
#define fourth_key  25979 
int receiver_pin = 8;   

IRrecv receiver(receiver_pin); 
decode_results output;
 
void setup()
{
  Serial.begin(9600);
  receiver.enableIRIn();  

}
 
void loop() {
  if (receiver.decode(&output)) {
    unsigned int value = output.value;
    switch(value) {
       case first_key:    
        Serial.println("key 1!"); 
          break; 
       case second_key:
        Serial.println("key 2!"); 
         
          break;
       case third_key:
        Serial.println("key 3!"); 
          break;   
      case fourth_key:
        Serial.println("key 4!"); 
          break;        
    }
    Serial.println(value); 
    receiver.resume(); 
  }
}
