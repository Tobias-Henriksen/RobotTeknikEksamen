#include <Stepper.h>
#include <Wire.h>

int i;

int knapPinX = 21;
int knapPinY = 20;
int knapPinZ = 19;
bool knapStatus;

int stepsX;
int stepsY;
int stepsZ;
int targetX;
int targetY;
int targetZ;
int distanceX;
int distanceY;


int error = 0;
int step = 0;
int taget = 0;

// Motorhastighed i rpm
int MotorSpeed1 = 80; 
int MotorSpeed2 = 80;
int MotorSpeed3 = 80;

int stepsPerMM = 1; // 100 steps er 4,8 cm. 200 er en omgang


// Definer Stepper Motorer på akserne og sæt step rækkefølge.
Stepper xAkse(200, 53, 51, 50, 52);
Stepper yAkse(200, 47, 45, 46, 44);
Stepper zAkse(200, 38, 40, 41, 39);



void setup() {
  Serial.begin(9600);

  pinMode(knapPinX, INPUT_PULLUP);
  pinMode(knapPinY, INPUT_PULLUP);
  pinMode(knapPinZ, INPUT_PULLUP);


  xAkse.setSpeed(MotorSpeed1);
  yAkse.setSpeed(MotorSpeed2);
  zAkse.setSpeed(MotorSpeed3);

// Kør kalibrering for hver akse.
 stepsX = calibrate("x");
 stepsY = calibrate("y");
// stepsZ = calibrate("z");


  move((20/4.85*100)*-1, -300); //Kør 20 cm (20/4.85*100)

  move(0,0);



}

void loop() {

}


int calibrate(int akse) {

  while (true) {
    // Læs hvilken knap der skal læses alt efter akse
    if (akse == "x") {
      knapStatus = digitalRead(knapPinX);
    }
    if (akse == "y") {
      knapStatus = digitalRead(knapPinY);
    }
    if (akse == "z") {
      knapStatus = digitalRead(knapPinZ);
    }

    // Hvis knappen er trykket er motoren i 0.
    if (knapStatus == LOW) {
      Serial.println("Stop!");
      return 0; 
    }

    // Ellers fortsætter den til den rammer 0.
    if (akse == "x") {
      xAkse.step(stepsPerMM);  
    }
    if (akse == "y") {
      yAkse.step(stepsPerMM);
    }
    if (akse == "z") {
      zAkse.step(stepsPerMM);
    }
  }
}


// int move(int stepsX,int targetX) {
//   //tag targetX/Y og fratræk stepsX/Y. Resultat burde være antal steps til mål. Fortegn autoudregnet.

//   distanceX = stepsX - targetX;
// //  distanceY = stepsY - targetY;
//   xAkse.step(distanceX);  
//   stepsX = (stepsX + distanceX)*-1;
//   Serial.println(stepsX);
// //  yAkse.step(distanceY);
//   return stepsX
//   return stepsY

// }

void move(int targetX, int targetY) {

  int distanceX = targetX - stepsX;  // hvor langt vi skal flytte os

  int distanceY = targetY - stepsY;  // hvor langt vi skal flytte os


  int directionX = (distanceX > 0) ? 1 : -1;

  for (int i = 0; i < abs(distanceX); i++) {
    xAkse.step(directionX);
  }

  
  int directionY = (distanceY > 0) ? 1 : -1;

  for (int i = 0; i < abs(distanceY); i++) {
    yAkse.step(directionY);
  }

  stepsY = targetY;

  stepsX = targetX;  // opdater position korrekt

  Serial.print("Ny position: ");
  Serial.print(stepsY);
  Serial.print("; ");
  Serial.println(stepsX);
}



