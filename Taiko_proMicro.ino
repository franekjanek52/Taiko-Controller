#include <Joystick.h>

#define DON_L A0         // Analog pin for the left drum "DON" pad
#define D0N_R A1         // Analog pin for the right drum "DON" pad
#define KA_L  A2         // Analog pin for the left drum "KA" pad
#define KA_R  A3         // Analog pin for the right drum "KA" pad

unsigned int pressedTime[] = {0,0,0,0};
bool buttonState[] = {false,false,false,false};
Joystick_ Joystick;     // Create a Joystick object

void setup() {
  //Serial.begin(115200);
  Joystick.begin(false); //joystick initialisation with auto update turned off
                         // using pinMode is not nessesary because analog pins are inputs by default
// interrupt magic:
    noInterrupts(); //disabling interrupts during timer1 configutation
    TCCR1A = 0;  //setting timer1 registers
    TCCR1B = 0;  //setting timer1 registers
    TCNT1 = 0;   //Reset timer
    OCR1A = 1249;   // 5ms interrupt (16MHz clock, 64 prescaler)
    TCCR1B |= (1 << WGM12); // CTC mode
    TCCR1B |= (1 << CS11) | (1 << CS10); // 64 prescaler 
    //TIMSK1 &= ~(1 << OCIE1A); // Disable timer interrupt
    TIMSK1 |= (1 << OCIE1A); // Enable timer interrupt
    interrupts(); //enabling interrupts
}

void loop() {                         //DL..DR..KL..KR
 static unsigned int threshold_min[] = {15, 15, 15, 15};// Adjust threshold values based on your sensor readings
 static unsigned int threshold[] = {10, 10, 10, 10};    // Dynamic threshold array
 static unsigned int maxv = 0; 
 static unsigned int id = 0;
 
 for(byte i = 0; i < 4; i++){
    if(pressedTime[i] >= 25){
    buttonState[i] = false;      // button state flag [OFF]
    pressedTime[i] = 0;         // button press duration counter reset
    Joystick.releaseButton(i);  // Coresponding button turned off
    Joystick.sendState();       // updating controller output
             
    }
 }
// reading inputs. using analogRead isn`t optimal for this but it works good enough, and dynamic threshold depends on this (bad)
 unsigned int sensorValue[] = {analogRead(DON_L), analogRead(D0N_R), analogRead(KA_L), analogRead(KA_R)}; 
 maxv = 0; //max sensor value reset
 for(byte i = 0; i < 4; i++){  // checking for maximum sensor value
    if(sensorValue[i] > maxv){ 
      maxv = sensorValue[i];  
      } 
 }

  if(sensorValue[id] > threshold[id]){ // if highest recoeded value is bigger than threshold
    threshold[id] = sensorValue[id];  // Threshold for this input is changed to the same level as its recorded value 
    Joystick.pressButton(id);         // Coresponding button is pressed
    buttonState[id] = true;           // button state flag [ON]
    //checking for other control inputs
    // thresholds for other inputs depends on index of triggered input 
    switch(id){                       
       case(0):
           threshold[1] = threshold_min[1] + sensorValue[id]/2.5; // Threshold for input of the same type (second center drumpad or rim) is lower than threshold for oposing input type
           threshold[2] = threshold_min[2] + sensorValue[id]/2;   // Threshold for opposing type is set to 1/2 value of triggered input
           threshold[3] = threshold_min[3] + sensorValue[id]/2;   // Threshold for opposing type is set to 1/2 value of triggered input
           if(sensorValue[1] >= threshold[1]){                    // Checking if the input of the same type is also triggered (Player played big note/double input )
             Joystick.pressButton(1);                             // Coresponding button is pressed
             buttonState[1] = true;                               // button state flag [ON]
           }
           break;
      
       case(1):
           threshold[0] = threshold_min[0] + sensorValue[id]/2.5;
           threshold[2] = threshold_min[2] + sensorValue[id]/2;
           threshold[3] = threshold_min[3] + sensorValue[id]/2;
           if(sensorValue[0] >= threshold[0]){
             Joystick.pressButton(0);
             buttonState[0] = true;
           }
           break;
      
       case(2):
           threshold[0] = threshold_min[0] + sensorValue[id]/2;
           threshold[1] = threshold_min[1] + sensorValue[id]/2;
           threshold[3] = threshold_min[3] + sensorValue[id]/2.5;
           if(sensorValue[3] >= threshold[3]){
             Joystick.pressButton(3);
             buttonState[3] = true;
           }
           break;
      
       case(3):
           threshold[0] = threshold_min[0] + sensorValue[id]/2;
           threshold[1] = threshold_min[1] + sensorValue[id]/2;
           threshold[2] = threshold_min[2] + sensorValue[id]/2.5;
           if(sensorValue[2] >= threshold[2]){
             Joystick.pressButton(2);
             buttonState[2] = true;
           }
           break;
    }
    Joystick.sendState(); // updating controller output   
 }
   //dynamic threshold magic
   //dependent on analogRead execution time <-- SUBOPTIMAL
   for(byte p = 0; p < 4; p++){      // Subtracting value of decay from tresholds
    if(threshold[p] > threshold_min[p]){
      threshold[p] = threshold[p] - 1;   
    }
   }
//end of main loop  
}

//more interrupt magic
ISR(TIMER1_COMPA_vect) { //interrupt service routine 1
   for(byte i = 0; i < 4; i++){
      if (buttonState[i] == true){
        pressedTime[i] = pressedTime[i] + 5;
      }
   }
}
   

 
/*
 * cool owl o7
#=================================================================================#
#           __________-------____                 ____-------__________           #
#          \------____-------___--__---------__--___-------____------/            #
#           \//////// / / / / / \   _-------_   / \ \ \ \ \ \\\\\\\\/             #
#              \////-/-/------/_/_| /___   ___\ |_\_\------\-\-\\\\/              #
#                --//// / /  /  //|| (O)\ /(O) ||\\  \  \ \ \\\\--                #
#                     ---__/  // /| \_  /V\  _/ |\ \\  \__---                     #
#                          -//  / /\_ ------- _/\ \  \\-                          #
#                            \_/_/ /\---------/\ \_\_/                            #
#                                ----\   |   /----                                #
#                                     | -|- |                                     #
#                                    /   |   \                                    #
#                                    ---- \___|                                   #
#=================================================================================#
*/
