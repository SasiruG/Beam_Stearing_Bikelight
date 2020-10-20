/*
   A working version of the beam steering bike light code. It dosen't react to
   enviornment light levels.
*/

#include <MPU6050_tockn.h>
#include <Wire.h>

#define LDR_PIN A3
#define ANALOG_RANGE 1023
#define LED1 3
#define LED2 5
#define LED3 6
MPU6050 mpu6050(Wire);


//Variables
double gyroOut, handleAngle, beamAngle, amp;
int LDRValue, PWMs[2], LED_DC[2];


//________________________________________________________________________________

void setup() {
  Serial.begin(9600);
  gyroSetup();
}

void loop() {
  handleAngle = readAngle();
  beamAngle = amplifyHandleTurn(handleAngle);
  beamSteering(beamAngle);
  PWMgen();

  Serial.print("\tangleZ : ");
  Serial.print(handleAngle);
  Serial.print("\t   amp : ");
  Serial.print(amp);
  Serial.print("\t   LDR Value : ");
  Serial.print(LDRValue);
  Serial.print("\tDC0 : ");
  Serial.println(LED_DC[0]);
  Serial.print("\tDC1 : ");
  Serial.println(LED_DC[1]);
  Serial.print("\tDC2 : ");
  Serial.println(LED_DC[2]);
  delay(10);
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
  PWMs[1] = 100 - ((pow(angle, 2)) / 4);
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
