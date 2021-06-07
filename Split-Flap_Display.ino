// Single Split Flap Display
// Written by Thoby Noorhalim for Invok Lab
// 7 June 2021

#include <Stepper.h>
#include <Unistep2.h>

// Defines the number of steps per rotation
const int stepsPerRevolution = 2048;
int sensor = A0;
int val;
int currentPos;
int zeroPos;
int magPos;
bool magDetected = false;
int stepsToA = 1250;
int stepsLeft = 0;
int setSteps = 0;
int homingOffset = 1100;
bool homingDone = false;
int incomingByte = 0;
char letter;
bool printable = false;

// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Unistep2 myStepper(4,5,6,7, 4096, 1000);

void homing(){
    Serial.println("Homing ...");
    val = analogRead(sensor);

    while(val < 590){
        myStepper.move(4096);
        myStepper.run();
        val = analogRead(sensor);
    }

    // Magnetic field detected, set this flap as 0 position
    // Pull down stepper shield
    magDetected = true;
    magPos = myStepper.currentPosition();

    // Move flaps to designated start flaps ( one flap before A)
    myStepper.move(homingOffset);
    stepsLeft = myStepper.stepsToGo();
    // Serial.println(magPos);
    while(stepsLeft != 0){
        myStepper.run();
        stepsLeft = myStepper.stepsToGo();
    }

    myStepper.stop();

    // Set homing done flag
    homingDone = true;
    Serial.println("Homing Done !");
}

void goToLetter(char letter){
    homing();
    setSteps = (letter - 'a' + 1)*137;
    Serial.println("Set steps is "+(String)setSteps);

    myStepper.move(setSteps);
    stepsLeft = myStepper.stepsToGo();
    Serial.println("Steps to go "+(String)stepsLeft);
    while(stepsLeft > 0){
        myStepper.run();
        stepsLeft = myStepper.stepsToGo();
    }

    Serial.println("Function called, steps to go "+(String)stepsLeft);
    myStepper.stop();
}

void setup() {
  Serial.begin(9600);
  pinMode(sensor, INPUT);
  
}

void loop() {
    if(!homingDone){
        homing();
    } 

    // Read serial
    if(Serial.available() > 0){
        // read byte
        incomingByte = Serial.read();
        printable = true;

        // Check if letter entered can be displayed
        // Check based on ASCII table
        if(incomingByte < 'A' || incomingByte > 'z' || 
            (incomingByte >= 91 && incomingByte <= 96)){
            // can't be displayed, ignore letter
            Serial.println("ERROR, letter can't be displayed");
            incomingByte = incomingByte + 'z' + 1;
        } else if(incomingByte >= 'A' && incomingByte <= 'Z'){
            // uppercase detected, convert to lowercase
            incomingByte = incomingByte + 32;
        }
        
        if(printable){
            Serial.println("Incoming byte is "+(String)incomingByte);
            letter = (char)incomingByte;
            Serial.println("Entered letter is "+(String)letter);
            goToLetter(letter);
            delay(500);
        }    
        
    }

}
