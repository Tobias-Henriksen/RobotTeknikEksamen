#include <Stepper.h>
#include <Wire.h>



int error = 0;
int step = 0;
int taget = 0;
int MotorSpeed1 = 100;
int MotorSpeed2 = 100;

int stepsPerMM = 100;


// Definer Stepper Motorer på akserne og sæt step rækkefølge.
Stepper yAkse(stepsPerMM, 47, 45, 46, 44);
Stepper zAkse(stepsPerMM, 38, 40, 41, 39);
Stepper xAkse(stepsPerMM, 53, 51, 50, 52);


void setup() {



xAkse.setSpeed(MotorSpeed1);
yAkse.setSpeed(MotorSpeed1);
zAkse.setSpeed(MotorSpeed2);

}

void loop() {
  xAkse.step(stepsPerMM);







}