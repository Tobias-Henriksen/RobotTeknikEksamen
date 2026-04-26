#include <AccelStepper.h>i 
#include <Stepper.h>
#include <Wire.h>

//int i;

//Definer on motore på køre. Bliver brugt af interrupt når switch bliver skiftet.
volatile bool runMotors = true;

// Opret array der skal bruges til nedvande upræcise målinger fra sonar sensor.
const int numSamples = 5;
int samples[numSamples];
int sampleIndex = 0;

unsigned long lastRead = 0;

//
int stableCount = 0;
int requiredStable = 3;
int lastDistance = 0;
int threshold = 4; // cm ændring før ændring bliver registreret som kant af pakken

//Definer pins til input fra knapper og til at kontrollere sug
const int gulButtonPin1 = 18;
const int gronButtonPin = 19;
const int gulButtonPin2 = 20;
const int switchPin = 21;

const int sugePin = 3;

const int trigPin = 10;
const int echoPin = 8;

const int knapPinX = 17;
const int knapPinY = 16;
bool knapStatus;

//Definer variable brugt til at udregne kanter og motor
int stepsX;
int stepsY;
int stepsZ;
int targetX;
int targetY;
int targetZ;
int distanceX;
int distanceY;
int centerX;
int centerY;
int lenghtX;
int lenghtY;
int edgeY1 = 0;
int edgeY2 = 0;
int edgeX1 = 0;
int edgeX2 = 0;


// Motorhastighed i steps pr sekund. 1 rpm er 3,33 sps. 
int MotorMaxSpeedX = 400; 
int MotorMaxSpeedY = 600;
int MotorMaxSpeedZ = 600;

// Motoracceleration bestemmer hvor hurtigt motor går fra stop til fuldhastighed eller fra fuldhastighed til stop.
int MotorAccelX = 1000; 
int MotorAccelY = 1000;
int MotorAccelZ = 1000;

//Opretter 3 steppere og navngiver dem hhv. xAkse, yAkse og zAkse.
AccelStepper xAkse(AccelStepper::FULL4WIRE, 53, 51, 50, 52);
AccelStepper yAkse(AccelStepper::FULL4WIRE, 38, 40, 41, 39);
AccelStepper zAkse(AccelStepper::FULL4WIRE, 47, 45, 46, 44);


void setup() {
  Serial.begin(9600);


  //Definer switch pin som et input og tilkobl interrupt så motorer kan stoppes.
  pinMode(switchPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(switchPin), stopMotors, HIGH);

// Definer suge pin som et output
  pinMode(sugePin, OUTPUT);

// Definer sonar pins som hhv. output og input.
  pinMode(trigPin, OUTPUT); // Skal være PWM
  pinMode(echoPin, INPUT);

// Definer limit knapper på akserne til at være inputs og anvend arduinos indbyggede pullup
  pinMode(knapPinX, INPUT_PULLUP);
  pinMode(knapPinY, INPUT_PULLUP);


  //Delay for at undgå start hvor motorer kører lidt, stopper og begynder igen.
  delay(1000);


// // Kør kalibrering for hver akse.

  stepsZ = calibrateZ();
  stepsX = calibrate('x');
  stepsY = calibrate('y');



// Sæt motor hastighed i steps pr sekund. 1 rpm er 3,33..
  xAkse.setMaxSpeed(MotorMaxSpeedX);
  yAkse.setMaxSpeed(MotorMaxSpeedY);
  zAkse.setMaxSpeed(MotorMaxSpeedZ);

// Sæt motor acceleration
  xAkse.setAcceleration(MotorAccelX);
  yAkse.setAcceleration(MotorAccelY);
  zAkse.setAcceleration(MotorAccelZ);



//  scanArea('x');


}

void loop() {

// // Program der køre pakke frem og tilbage
  move(0, 500, 5700);
  delay(500);
  opsamling();
  delay(500);
  move(0, 500, 0);
  move(800,100, 0);
  move(800, 100, 5500);
  delay(500);
  aflevering();
  delay(500);
  move(800, 100, 0);
  delay(200);
  move(800, 100, 5600);
  delay(500);
  opsamling();
  delay(500);
  move(800,100, 0);
  move(0, 500, 5600);
  delay(500);
  aflevering();
  delay(500);
  move(0, 500, 0);




}


// Kalibrer Z aksen ved at kører op indtil den måler 60 cm afstand mellem sugekoppen og jorden. Sæt derefter det som nul punktet.
int calibrateZ() {

  AccelStepper* motor;
  int pin;

  motor = &zAkse;

  motor->setMaxSpeed(600);
  motor->setAcceleration(1000);


  // Kør mod home indtil sonar måler 60 cm rammes
  while (sonarInput() < 60) {
    if (runMotors = true)  {
    motor->moveTo(-10000);
    motor->run();
    }
    else {
      motor->stop();
    }
  }

  // Stop og nulstil position
  motor->stop();
  while (motor->isRunning()) {
    if (runMotors = true)  {
    motor->run();
    }
  }

  motor->setCurrentPosition(0);
  // Serial.println("sonarInput sluttet");

  return 0;
}

// Kør motor på ønskede akse indtil knap er trykket ind og sæt derefter dette punkt som nulpunktet.
int calibrate(char akse) {

  AccelStepper* motor;
  int pin;

  if (akse == 'x') {
    motor = &xAkse;
    pin = knapPinX;
  }
  else if (akse == 'y') {
    motor = &yAkse;
    pin = knapPinY;
  }
  else {
    return -1; // fejl
  }

  motor->setMaxSpeed(600);
  motor->setAcceleration(20000);

  motor->moveTo(-10000);

  // Kør mod home indtil knap rammes
  while (digitalRead(pin) != LOW) {
    if (runMotors == true){
      motor->run();
    }
  


  }

  // Stop og nulstil position
  motor->stop();
  while (motor->isRunning()) {
    motor->run();
  }

  motor->setCurrentPosition(0);



  return 0;
}

// Flyt motorer til ønskede antal steps.
void move(int targetX, int targetY, int targetZ) {

  xAkse.moveTo(targetX);
  yAkse.moveTo(targetY);
  zAkse.moveTo(targetZ);
  Serial.println(zAkse.distanceToGo());


  while (xAkse.distanceToGo() != 0 || yAkse.distanceToGo() != 0 || zAkse.distanceToGo() != 0 ) {
    if (runMotors == true){
    xAkse.run();
    yAkse.run();
    zAkse.run();
    }

  }

  stepsX = targetX;
  stepsY = targetY;
  stepsZ = targetZ;
}


//Læs input fra sonarsensor
float sonarInput() {

  if (millis() - lastRead < 20) return -1;
    lastRead = millis();
  

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(25);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH, 60000); // timeout 30ms

  if (duration == 0) return -2;

  float distance = duration * 0.034 / 2;

  if (distance < 2 || distance > 400) return -3; // filtrer ukorrekte målinger fra

  return distance;

}



// Læg først værdier fra sonarInput ind i samples vælg så median så måling er mere præcise.
void sonarEdge(char akse) {

    // Læs sonarInput
    float d = sonarInput();
    if (d < 0) return;

    // tjek om samples er fyldt. Hvis ikke så fyld samples med nuværende måling
    bool primed = false;
    if (!primed) {
        lastDistance = d;

        // fyld samples med samme måling
        for (int i = 0; i < numSamples; i++)
            samples[i] = d;

        primed = true;
        return;
    }

    // Opdater samples
    samples[sampleIndex] = d;
    sampleIndex = (sampleIndex + 1) % numSamples;

    // Udregn median fra samples. 
    int sorted[5];
    for (int i = 0; i < 5; i++) {
        sorted[i] = samples[i];
    }

    // Sorter array
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            if (sorted[j] < sorted[i]) {
                int temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }

    int filtered = sorted[2];  // median

    // Opdag kant ændring der vare ved. Vigtigt at den vare ved, ellers returnere sensoren for mange kanter.
    static unsigned long changeStart = 0;

    if (abs(filtered - lastDistance) > threshold) {
        if (changeStart == 0)
            changeStart = millis();

        if (millis() - changeStart > 150) {  // 150 ms sustained change

            // Kant fundet
            lastDistance = filtered;
            changeStart = 0;

            // Sæt enten kant 1 eller 2 på gældende akse til at være værdien hvor kanten blev fundet
            if (akse == 'x') {
                if (edgeX1 == 0) edgeX1 = xAkse.currentPosition();
                else if (edgeX2 == 0) edgeX2 = xAkse.currentPosition();
            }
            else if (akse == 'y') {
                if (edgeY1 == 0) edgeY1 = yAkse.currentPosition();
                else if (edgeY2 == 0) edgeY2 = yAkse.currentPosition();
            }
        }
    }
    else {
        changeStart = 0;
    }
}



// Kører metodisk i gennem et område for at finde pakke.
void scanArea(char akse) {

// Ændre til at først køre x. Tag derefter kanterne på x og brug til at begrænse kørte areal for at spare tid og sikre præcise målinger på hver akse. 

  if (akse == 'x'){
    Serial.println("scanArea('x')");

  int xArea = 800;
  int yArea = 800;

  // int x = 400;
  // int y = 400; 

  int scanXStep = 10;
  int scanYStep = 200; // Afstand kørt på y efter hvert fuldførte x akse tur


  for (int y = 0; y <= yArea; y += scanYStep) {
    for (int x = 0; x <= xArea; x += scanXStep) {

      xAkse.moveTo(x);
      while (x != xAkse.currentPosition() && runMotors == true) {
        xAkse.run();
      }
      sonarEdge('x');
      Serial.print("EdgeX1: ");
      Serial.println(edgeX1);
      Serial.print("EdgeX2: ");
      Serial.println(edgeX2);
//      if (edgeX1 != 0 || edgeX2 != 0){
//        return;
//      }


      yAkse.moveTo(y);
      while (y != yAkse.currentPosition() && runMotors == true) {
        yAkse.run();
      }
    }
  }
  }
  if (akse == 'y'){
    //Ændre så vores område 

    int xArea = edgeX1 + edgeX2;
    int yArea = 800;

    int x = edgeX1;

    int scanYStep = 10;
    int scanXStep = 200; // Afstand kørt på y efter hvert fuldførte x akse tur

    for ( x ; x <= xArea; x += scanXStep) {      
      for (int y = 0 ; y <= yArea; y += scanYStep) {

        yAkse.moveTo(y);
        while (y != yAkse.currentPosition() && runMotors == true) {
          yAkse.run();
        }
        sonarEdge('y');
        Serial.print("EdgeY1: ");
        Serial.println(edgeY1);
        Serial.print("EdgeY2: ");
        Serial.println(edgeY2);
        if (edgeY1 != 0 || edgeY2 != 0){
          return;
        }


        yAkse.moveTo(x);
        while (x != xAkse.currentPosition() && runMotors == true) {
          yAkse.run();
        }
      }
    }
  }
}


// Udregner center af kasse ved at finde afstand mellem kanter, halvere den og lægge den til værdien tættest på 0,0
void centerPos(int edgeX1, int edgeX2, int edgeY1, int edgeY2){
  lenghtX = edgeX2 - edgeX1;
  centerX = (lenghtX/2) + edgeX1;

  lenghtY = edgeY2 - edgeY1;
  centerY = (lenghtY/2) + edgeY1;

}

// Saml pakke op ved at tænde for sug
void opsamling(){
  digitalWrite(sugePin, HIGH);

}
// Aflever pakke ved at slukke for sug.
void aflevering(){
  digitalWrite(sugePin, LOW);

}

// Stopper motorer ved at ændre runMotors til false. Da runMotors == true er et krav allesteder hvor motoren kører stopper de med det samme. Er med vilje gjort kort så interrupt hurtigt kan afvikle den.
void stopMotors() {
  runMotors = false;
}