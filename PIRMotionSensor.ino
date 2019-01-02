//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 5;

//the time when the sensor outputs a low impulse
long unsigned int lowIn;

//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
long unsigned int pause = 5000;

boolean lockLow = true;
boolean takeLowTime;

int pirPin = 2;    //the digital pin connected to the PIR sensor's output
int ledPin = 13;

int pinSpeaker = 10;           //Set up a speaker on a PWM pin (digital 9, 10, or 11)


//I2C LCD:
#include <Wire.h> // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>

// Set the LCD I2C address
LiquidCrystal_I2C lcd(0x3F, 16, 2); // set the LCD address to 0x3F for a 16 chars and 2 line display

/////////////////////////////
//SETUP
void setup() {
  lcd.begin(16, 2);
  lcd.init(); // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Capteur de");
  lcd.setCursor(1, 1);
  lcd.print("Mouvement");

  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(pinSpeaker, OUTPUT);

  digitalWrite(pirPin, LOW);
  //-----------------------------------------------
  lcd.clear();
  // set the cursor to (0,0):
  lcd.setCursor(0, 0);
  lcd.print("calibrating");
  lcd.setCursor(0, 1);
  lcd.print("sensor");
  //-----------------------------------------------
  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
  //-----------------------------------------------
  lcd.clear();
  // set the cursor to (0,0):
  lcd.setCursor(0, 0);
  lcd.print("done");
  lcd.setCursor(0, 1);
  lcd.print("SENSOR ACTIVE");
  //-----------------------------------------------

}

////////////////////////////
//LOOP
void loop() {

  if (digitalRead(pirPin) == HIGH) {
    digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
    playTone(300, 160);
    delay(300);

    if (lockLow) {
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;
      Serial.println("---");
      Serial.print("motion detected at ");
      Serial.print(millis() / 1000);
      Serial.println(" sec");
      delay(5);
      //-----------------------------------------------
      lcd.clear();
      // set the cursor to (0,0):
      lcd.setCursor(0, 0);
      lcd.print("motion detected");
      lcd.setCursor(0, 1);
      lcd.print("at  ");
      lcd.print(millis() / 1000);
      lcd.print(" sec");
      //-----------------------------------------------
    }
    takeLowTime = true;
  }

  if (digitalRead(pirPin) == LOW) {
    digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state
    playTone(0, 0);
    delay(300);
    if (takeLowTime) {
      lowIn = millis();          //save the time of the transition from high to LOW
      takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
    //if the sensor is low for more than the given pause,
    //we assume that no more motion is going to happen
    if (!lockLow && millis() - lowIn > pause) {
      //makes sure this block of code is only executed again after
      //a new motion sequence has been detected
      lockLow = true;
      Serial.print("motion ended at ");      //output
      Serial.print((millis() - pause) / 1000);
      Serial.println(" sec");
      delay(5);
      //-----------------------------------------------
      lcd.clear();
      // set the cursor to (0,0):
      lcd.setCursor(0, 0);
      lcd.print("motion ended");
      lcd.setCursor(0, 1);
      lcd.print("at");
      lcd.print((millis() - pause) / 1000);
      lcd.print(" sec");
      //-----------------------------------------------
    }
  }
}

// duration in mSecs, frequency in hertz
void playTone(long duration, int freq) {
    duration *= 1000;
    int period = (1.0 / freq) * 1000000;
    long elapsed_time = 0;
    while (elapsed_time < duration) {
        digitalWrite(pinSpeaker,HIGH);
        delayMicroseconds(period / 2);
        digitalWrite(pinSpeaker, LOW);
        delayMicroseconds(period / 2);
        elapsed_time += (period);
    }
}
