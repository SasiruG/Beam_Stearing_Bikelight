/*
 * PID controlled beam steering for a bike light
 * 
 * This code uses input from a 5k Potentiometer to steer the light beam of a bike
 * light towards which way the bike is intending to travel. Amplifies the turn of 
 * the handlebar to turn the light to see where you are going.
 * Written for the Design & build project of ENEL300 course of University of 
 * Canterbury, Electrical & Computer Engineering Department.
 * 
 * Author:       Sasiru Goonatillake
 * Date:         Aug 29th 2019
 * Last Updated: Aug 29th 2019
 */

#define POT_PIN 2;


//PID tuning constants
double Kp = 1;  
double Ki = 1;
double Kd = 1;

//Variables
unsigned long currTime, PrevTime; // Time related 
double deltaTime;
double error, prevError, intError, dervError;
double potVal, setAngle = 0, beamAngle;   // potVal = input, setAngle = refference,
                                      // beamAngle = output
int potVal;   // Potentiometer value read by uno


//________________________________________________________________________________

void setup() {
  Serial.begin(57600);
}

void loop() {
  potVal = analogRead(potPin);    // input from potentiometer
  Serial.println(potVal);     // remove after testing----------------------------

  beamAngle = BeamPID(potVal);
}
