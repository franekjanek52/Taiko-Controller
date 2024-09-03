#include <Joystick.h>

#define decay 1
#define DON_L A0         // Analog pin for the left drum "DON" pad
#define D0N_R A1         // Analog pin for the right drum "DON" pad
#define KA_L  A2         // Analog pin for the left drum "KA" pad
#define KA_R  A3         // Analog pin for the right drum "KA" pad

Joystick_ Joystick;     // Create a Joystick object

void setup() {
  Joystick.begin(false); //joystick initialisation with auto update turned off
                        // using pinMode is not nessesary because analog pins are inputs by default
}

void loop() {                //DL..DR..KL..KR
 static unsigned int threshold_min[] = {15, 15, 15, 15};// Adjust threshold values based on your sensor readings
 static unsigned int threshold[] = {10, 10, 10, 10};    // Dynamic threshold array
 static unsigned int sensorValue[] = {0, 0, 0, 0};      // recorded values array 
 static unsigned int maxv = 0; 
 static unsigned int id = 0;
 bool activity = false;
 
   sensorValue[0] = analogRead(DON_L);   // reading inputs. using analogRead isn`t optimal for this but it works good enough :P 
   sensorValue[2] = analogRead(KA_L);
   sensorValue[1] = analogRead(D0N_R);
   sensorValue[3] = analogRead(KA_R);
   

maxv = 0;
for(byte i = 0; i < 4; i++){           // checking for maximum value
    if(sensorValue[i] > maxv){
      maxv = sensorValue[i];
      id = i;
      } 
}

if(sensorValue[id] > threshold[id]){ // if highest recoeded value is bigger than threshold

      threshold[id] = sensorValue[id]; // Threshold for this input is changed to the same level as its recorded value 
      Joystick.pressButton(id);        // Coresponding button is pressed
     
      if(id == 0){                     // thresholds for other inputs depends on index of triggered input 
        threshold[1] = threshold_min[1] + sensorValue[id]/2.5; // Threshold for input of the same type (second center drumpad or rim) is lower than threshold for oposing input type
        threshold[2] = threshold_min[2] + sensorValue[id]/2;   // Threshold for opposing type is set to 1/2 value of triggered input
        threshold[3] = threshold_min[3] + sensorValue[id]/2;
        if(sensorValue[1] >= threshold[1]){                    // Checking if the input of the same type is also triggered (Player played big note/double input )
          Joystick.pressButton(1);                             // Coresponding button is pressed
        }
      }
      if(id == 1){
        threshold[0] = threshold_min[0] + sensorValue[id]/2.5;
        threshold[2] = threshold_min[2] + sensorValue[id]/2;
        threshold[3] = threshold_min[3] + sensorValue[id]/2;
        if(sensorValue[0] >= threshold[0]){
          Joystick.pressButton(0);
          }
       }
      if(id == 2){
        threshold[0] = threshold_min[0] + sensorValue[id]/2;
        threshold[1] = threshold_min[1] + sensorValue[id]/2;
        threshold[3] = threshold_min[3] + sensorValue[id]/2.5;
        if(sensorValue[3] >= threshold[3]){
          Joystick.pressButton(3);
         }
      }
      if(id == 3){
        threshold[0] = threshold_min[0] + sensorValue[id]/2;
        threshold[1] = threshold_min[1] + sensorValue[id]/2;
        threshold[2] = threshold_min[2] + sensorValue[id]/2.5;
        if(sensorValue[2] >= threshold[2]){
          Joystick.pressButton(2);
        }
      }
      
     activity = true; // Input was present
     
    }
  
 
 if(activity == true){                  // if input was present
   Joystick.sendState();             // updating controller output 
   delay(25);                        // Simulate button press duration
   for(byte i = 0; i < 4; i++){
      Joystick.releaseButton(i);      // deciding to release all of the buttons
   }
   Joystick.sendState();             // updating controller output   
 }
     
 for(byte p = 0; p < 4; p++){           // Subtracting value of decay from tresholds
    if(threshold[p] > threshold_min[p]){
      threshold[p] = threshold[p] - decay;
    }
 }
 
} //end of main loop
/*
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
