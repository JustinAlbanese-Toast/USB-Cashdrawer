#include <Arduino.h>
const unsigned int MAX_MESSAGE_LENGTH = 12;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(0,OUTPUT);
  pinMode(2,INPUT);
  pinMode(4,OUTPUT);
  digitalWrite(0, HIGH);
  digitalWrite(4, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(4, LOW);
  while(Serial.available() > 0){
      static char message[MAX_MESSAGE_LENGTH];
      static unsigned int message_pos = 0;

      char inByte = Serial.read();


      if(inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH -1))
      {
        message[message_pos] = inByte;
        message_pos++;
      }
      else
      {
        //Add null character to string
        message[message_pos] = '\0';

        //Print the message (or do other things)
        Serial.println(message);
        int(number) = atoi(message);
        if(number == 1984){
          Serial.println("open command sent");
          digitalWrite(4,HIGH);
          delay(1000);
        }else if(number == 1985){
          if(digitalRead(2)==LOW){
            Serial.println("the drawer is open");
          }
          else{
            Serial.println("the drawer is closed");
          }
        }

        //Reset for the next message
        message_pos = 0;
      }
      

  }
}