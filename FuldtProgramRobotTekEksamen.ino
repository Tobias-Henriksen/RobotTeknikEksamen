#include <AccelStepper.h>
#include <Stepper.h>
#include <Wire.h>

int i;

const int numSamples = 5;
int samples[numSamples];
int sampleIndex = 0;

unsigned long lastRead = 0;

int stableCount = 0;
int requiredStable = 3;
int lastDistance = 0;
int threshold = 4; // cm ændring før ændring bliver registreret som kasse kant

const int sugePin = 3;

const int trigPin = 10;
const int echoPin = 8;


const int knapPinX = 21;
const int knapPinY = 20;
const int knapPinZ = 19;
bool knapStatus;

int stepsX;
int stepsY;
int stepsZ;
int targetX;
int targetY;
int targetZ;
int distanceX;
int distanceY;


// Motorhastighed i steps pr sekund. 1 rpm er 3,33..
int MotorMaxSpeed1 = 700; 
int MotorMaxSpeed2 = 700;
int MotorMaxSpeed3 = 700;

// Motorhastighed acceleration
int MotorAccel1 = 1500; 
int MotorAccel2 = 1500;
int MotorAccel3 = 1500;


int stepsPerMM = 1; // 100 steps er 4,8 cm. 200 er en omgang

AccelStepper xAkse(AccelStepper::FULL4WIRE, 53, 51, 50, 52);
AccelStepper yAkse(AccelStepper::FULL4WIRE, 47, 45, 46, 44);
AccelStepper zAkse(AccelStepper::FULL4WIRE, 38, 40, 41, 39);

//GAMMEL DEFINATION AF STEPPERE
// // Definer Stepper Motorer på akserne og sæt step rækkefølge.

// Stepper xAkse(200, 53, 51, 50, 52);
// Stepper yAkse(200, 47, 45, 46, 44);
// Stepper zAkse(200, 38, 40, 41, 39);



void setup() {
  Serial.begin(9600);

// Definer suge pin
  pinMode(sugePin, OUTPUT);

// Definer sonar pins.
  pinMode(trigPin, OUTPUT); // Skal være PWM
  pinMode(echoPin, INPUT);

// Definer knap pins
  pinMode(knapPinX, INPUT_PULLUP);
  pinMode(knapPinY, INPUT_PULLUP);
  pinMode(knapPinZ, INPUT_PULLUP);


// Sæt motor hastighed i steps pr sekund. 1 rpm er 3,33..
  xAkse.setMaxSpeed(MotorMaxSpeed1);
  yAkse.setMaxSpeed(MotorMaxSpeed2);
  zAkse.setMaxSpeed(MotorMaxSpeed3);

// Sæt motor acceleration
  xAkse.setAcceleration(MotorAccel1);
  yAkse.setAcceleration(MotorAccel2);
  zAkse.setAcceleration(MotorAccel3);


// Kør kalibrering for hver akse.
  stepsX = calibrate("x");
  stepsY = calibrate("y");

 
// stepsZ = calibrate("z");


//  move(20/4.85*100, 50); //Kør 20 cm (20/4.85*100)

  // move(1200,800);

  // move(0,0);

  scanArea();



}

void loop() {
//Serial.println(sonarInput());

}




int calibrate(int akse) {

  AccelStepper* motor;
  int pin;

  if (akse == "x") {
    motor = &xAkse;
    pin = knapPinX;
  }
  else if (akse == "y") {
    motor = &yAkse;
    pin = knapPinY;
  }
  else if (akse == "z") {
    motor = &zAkse;
    pin = knapPinZ;
  }
  else {
    return -1; // fejl
  }

  motor->setMaxSpeed(600);
  motor->setAcceleration(20000);

  motor->moveTo(-10000);

  // Kør mod home indtil knap rammes
  while (digitalRead(pin) != LOW) {

  
    motor->run();
  }

  // Stop og nulstil position
  motor->stop();
  while (motor->isRunning()) {
    motor->run();
  }

  motor->setCurrentPosition(0);

  motor->setMaxSpeed(600);
  motor->setAcceleration(1000);

  return 0;
}

void move(int targetX, int targetY) {

  xAkse.moveTo(targetX);
  yAkse.moveTo(targetY);

  while (xAkse.distanceToGo() != 0 || yAkse.distanceToGo() != 0) {
    xAkse.run();
    yAkse.run();

  }

  stepsX = targetX;
  stepsY = targetY;

  Serial.print("Ny position: ");
  Serial.print(stepsY);
  Serial.print("; ");
  Serial.println(stepsX);
}





long sonarInput() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms

  if (duration == 0) return -1;

  long distance = duration * 0.034 / 2;

  if (distance < 2 || distance > 400) return -1; // filtrer ukorrekte målinger fra

  return distance;


}

void sonarAverage(){ // Gem koordinator (stepsX/Y) når en ændring er dedekteret. 

  // Begræns målefrekvens
  if (millis() - lastRead < 50) return;
  lastRead = millis();

  int distance = sonarInput();

  if (distance == -1) return;

  // Gem i buffer
  samples[sampleIndex] = distance;
  sampleIndex = (sampleIndex + 1) % numSamples;

  // Beregn gennemsnit
  int sum = 0;
  for (int i = 0; i < numSamples; i++) {
    sum += samples[i];
  }
  int filtered = sum / numSamples;

  // Stabil ændring (anti-støj)
  if (abs(filtered - lastDistance) > threshold) {
    stableCount++;
  } else {
    stableCount = 0;
  }

  if (stableCount >= requiredStable) {

    Serial.println("Kant fundet!");

    Serial.print("Position X: ");
    Serial.print(stepsX);
    Serial.print(" Y: ");
    Serial.println(stepsY);

    stableCount = 0;
  }

  lastDistance = filtered;
}



void scanArea() {
  int x = 400;
  int y = 400; 

  int scanXStep = 10;
  int scanYStep = 100; // Afstand kørt på y efter hvert fuldførte x akse tur


for (int y = 0; y <= 400; y += scanYStep) {
  for (int x = 0; x <= 400; x += scanXStep) {

    xAkse.moveTo(x);
    while (x != xAkse.currentPosition()) {
      xAkse.run();
      Serial.println(sonarInput());   
    }
 

    yAkse.moveTo(y);
    while (y != yAkse.currentPosition()) {
      yAkse.run();
    }
  }
}

  }



