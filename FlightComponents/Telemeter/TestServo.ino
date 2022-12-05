#include <Servo.h>

Servo mainservo;
int pos = 0; 
void setup()
{
    mainservo.attach(5);
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) { 
    
    mainservo.write(pos);             
    delay(40);                       
  }
  for (pos = 180; pos >= 0; pos -= 1) { 
    mainservo.write(pos);              
    delay(40);                  
  }
}