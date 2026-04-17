#include <Stepper.h>
#include <Wire.h>



int error = 0;
int step = 0;
int taget = 0;
int MotorSpeed1 = 20;
int MotorSpeedZ = 100;
int DirectionX = 1;
int DirectionY = 1;
int DirectionZ = 1;

int stepsPerMM = 200;


const byte interruptPin = 21;
volatile bool state = LOW;
volatile bool buttonPressed = false;
bool lastButtonState1 = HIGH;





Stepper yAkse(stepsPerMM, 4, 5, 6, 7);
Stepper zAkse(stepsPerMM, 8, 9, 10, 13);
Stepper xAkse(stepsPerMM, 2, 3, 11, 12);

//Stepper xAkse(stepsPerMM, M1, E1, E2, M2);

void setup() {
  Serial.begin(115200);

// pinMode(stepperFartPin1, OUTPUT);
// pinMode(stepperFartPin2, OUTPUT);
// pinMode(stepperRetningPin1, OUTPUT);
// pinMode(stepperRetningPin2, OUTPUT);


pinMode( interruptPin, INPUT_PULLUP );
attachInterrupt(digitalPinToInterrupt(interruptPin), ISR_button_pressed, FALLING);

xAkse.setSpeed(MotorSpeed1);
yAkse.setSpeed(MotorSpeed1);
zAkse.setSpeed(MotorSpeedZ);

}

void loop() {
  Serial.print("DirectionX:");
  Serial.println(DirectionX);

  Serial.print("State:");
  Serial.println(state);
  

  if (buttonPressed) {
    DirectionX = -DirectionX;  // skift retning
    buttonPressed = false;
  }


  yAkse.step(DirectionX);

}
  // digitalWrite(stepperRetningPin1, HIGH); 
  // digitalWrite(stepperRetningPin2, HIGH);

  // analogWrite(stepperFartPin1, 600);
  // analogWrite(stepperFartPin2, 600);

  // error = taget - step;

  // if (error > 1) {
  //   //myStepper.setSpeed(MotorSpeed);
  //   myStepper.step(1);
  //   //step =- 1;
  //   step = step + 1;
  // }
  // if (error < -1) {
  //   //myStepper.setSpeed(-MotorSpeed);
  //   myStepper.step(-1);
  //   step =+ 1;
  // }
  // if (error == 0) {
  //   //myStepper.setSpeed(MotorSpeed);
  //   myStepper.step(0);
  // }


  // yAkse.step(-stepsPerMM);
  // zAkse.step(stepsPerMM); // Rotate other way
  // xAkse.step(stepsPerMM); // Rotate one way
  // delay(1000);
  // yAkse.step(-stepsPerMM);
  // xAkse.step(-stepsPerMM); // Rotate other way
  // zAkse.step(-stepsPerMM); // Rotate one way
  // delay(1000);


// if (state)//if an interrup has occured
//     {
//         yAkse.step(-stepsPerMM);
//         DirectionX = -DirectionX; //change direction of x when button is pressed


//         if (digitalRead(interruptPin))//if button is released let ISR set flag.
//         {
//             attachInterrupt(digitalPinToInterrupt(interruptPin), ISR_button_pressed, FALLING );
//             state = false;
//         }
//            else {
//       yAkse.step(DirectionX * stepsPerMM);
//     }




void ISR_button_pressed(void) {
  buttonPressed = true;
}
