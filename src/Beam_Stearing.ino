/*
   Beam steering for a bike light

   This code uses input from an accelerometer and a gyroscope (mpu6050) to steer
   the light beam of a bike light towards which way the bike is intending to travel.
   Amplifies the turn of the handlebar to turn the light to see where you are going.
   Written for the Design & build project of ENEL300 course of University of
   Canterbury, Electrical & Computer Engineering Department.

   Author:       Sasiru Goonatillake
   Date:         Aug 29th 2019
   Last Updated: Sep  6th 2019
*/

#include <MPU6050_tockn.h>
#include <Wire.h>

#define LDR_PIN A3
#define ANALOG_RANGE 1023
#define LUX_CONTROL 70   // a value between 0 and 1 to decrease the overall intensity as a percentage
#define LED1 3
#define LED2 5
#define LED3 6
MPU6050 mpu6050(Wire);

//Variables
double gyroOut, handleAngle, beamAngle, amp, lux, invLux, holder;
int LDRValue, PWMs[2], LED_DC[2];

//________________________________________________________________________________

void setup() {
  Serial.begin(9600);
  //  TCCR0B = TCCR0B & B11111000 | B00000100;    // set timer 0 divisor to   256 for PWM frequency of   244.14 Hz of pin 5 & 6
  //  TCCR2B = TCCR2B & B11111000 | B00000101;    // set timer 2 divisor to   128 for PWM frequency of   245.10 Hz of pin 3
  gyroSetup();
}

void loop() {
  handleAngle = readAngle();
  readLDR();
  beamAngle = amplifyHandleTurn(handleAngle);
  beamSteering(beamAngle);
  PWMgen();
  dataOut();
  delay(50);
}

void dataOut() {
  Serial.print("\tangleZ : ");
  Serial.print(handleAngle);
  Serial.print("\t   amp : ");
  Serial.print(amp);
  Serial.print("\t   LDR Value : ");
  Serial.print(LDRValue);
  Serial.print("\t   invLUX : ");
  Serial.println(invLux);
  Serial.print("\tDC0 : ");
  Serial.println(LED_DC[0]);
  Serial.print("\tDC1 : ");
  Serial.println(LED_DC[1]);
  Serial.print("\tDC2 : ");
  Serial.println(LED_DC[2]);
}

/*
   Sets up I2C communcicaitons with mpu6050 and calibrates the gyro for each reset
*/
void gyroSetup() {
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

/*
   Read the Z axis angle change
*/
double readAngle() {
  mpu6050.update();
  gyroOut = mpu6050.getAngleZ();
  return gyroOut;
}

/*
   Read the resistance value given by the Light Dependent Resistor
*/
void readLDR() {
  LDRValue = analogRead(LDR_PIN);
  lux = LDRValue / ANALOG_RANGE;
  holder = 1 - lux;
  invLux = (map(holder, 0, 1, LUX_CONTROL, 1)) / 100;
  invLux = 1;
}

/*
   Uses the predetermined 4th order polynomial to estimate the amplification needed
*/
double amplifyHandleTurn(double x) {
  //  amp =  -0.0001*pow(x,3) + (2*pow(10,-20))*pow(x,2) + 0.7407*x + 4*pow(10,-12);
  amp = (40 / (1 + (exp(-0.1 * x)))) - 20;
  return amp;
}

/*
   Creates 3 pwm waves for the LEDs based on handle bar position
*/
double beamSteering(double angle) {
  PWMs[0] = 10 - (1.235939 / 0.1098612) * (1 - exp(0.1098612 * angle));
  PWMs[1] = 100 - ((pow(angle, 2)) / 8);
  PWMs[2] = 10 - (1.235939 / 0.1098612) * (1 - exp(-0.1098612 * angle));

}

/*
   Creates 3 PWM waves for the 3 LEDs to control the intestiy of each according to PWMs[] array
*/
void PWMgen() {
  LED_DC[0] = map(PWMs[0], 0, 100, 5, 255);
  LED_DC[1] = map(PWMs[1], 0, 100, 5, 255);
  LED_DC[2] = map(PWMs[2], 0, 100, 5, 255);

  analogWrite(LED1, LED_DC[0]);
  analogWrite(LED2, LED_DC[1]);
  analogWrite(LED3, LED_DC[2]);
}
