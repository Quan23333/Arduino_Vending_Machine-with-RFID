// Define pin connections & motor's steps per revolution
#include <SPI.h>
#include <MFRC522.h>

//RFID
#define SS_PIN 49
#define RST_PIN 48
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

//LCD define
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

//Keypad define
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {22, 23, 24, 25}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 27, 28, 29}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

bool cardStatu;
#define button 12
int buttonStatu = 0;

void setup()
{
  lcd.init();               // initialize the lcd
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Pls Insert card");

  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();          // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
}

void loop()
{
  itemSelect('1', 1);
  itemSelect('2', 2);
  itemSelect('3', 3);
  itemSelect('4', 4);

  /*if (customKey){
    Serial.println(customKey);
    }*/

  /*if (customKey == '1') {
    Serial.println("Insert card to the reader...");
    lcd.init();
    lcd.setCursor(0, 0);
    lcd.print("Item 1 selected");
    lcd.setCursor(0, 1);
    lcd.print("Pls Insert card");
    while (cardStatu == false) {
      rfidScaning("F0 BB 19 19", 1);
    }
    Serial.println("Break");
    }*/

}

// Define pin connections & motor's steps per revolution
void stepperMotorRun(int dirPinMotor, int stepPinMotor, int rotate, bool runDir, int delayTime) {
  const int dirPin = dirPinMotor;
  const int stepPin = stepPinMotor;
  const int stepsPerRevolution = rotate;

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  // Set motor direction clockwise
  digitalWrite(dirPin, runDir);

  // Spin motor slowly
  for (int x = 0; x < stepsPerRevolution; x++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);
  }
  delay(delayTime); // Wait a second
}

bool rfidScaning(String cardId, int var) {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;

  //print
  //lcd.setCursor(0, 0);
  //lcd.print("Please, insert your card");

  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  if (content.substring(1) == cardId) //"00 4C 3C 1A" change here the UID of the card/cards that you want to give access
  {
    //lcd print
    Serial.println("Authorized access");
    lcd.init();
    lcd.setCursor(0, 0);
    lcd.print("Processing");
    switch (var) {
      case 1:
        motorLift(false);
        stepperMotorRun(2, 3, 1000, false, 1000);
        motorLift(true);
        break;
      case 2:
        stepperMotorRun(4, 5, 1000, false, 1000);
        break;
      case 3:
        stepperMotorRun(6, 7, 1000, false, 1000);
        break;
      case 4:
        stepperMotorRun(8, 9, 1000, false, 1000);
        break;
    }
    cardStatu = true;
  }

  else   {
    //lcd print
    Serial.println(" Access denied");
    lcd.init();
    lcd.setCursor(0, 0);
    lcd.print("Access denied");
    cardStatu = false;
  }
}

void itemSelect(char var, int num) {
  char customKey = customKeypad.getKey();
  cardStatu = false;
  if (customKey == var) {
    Serial.println("Insert card to the reader...");
    lcd.init();
    lcd.setCursor(0, 0);
    lcd.print("Item   selected");
    lcd.setCursor(5, 0);
    lcd.print(num);
    lcd.setCursor(0, 1);
    lcd.print("Pls Insert card");
    while (cardStatu == false) {
      rfidScaning("F0 BB 19 19", num);
    }
    Serial.println("Break");
    lcd.init();
    lcd.setCursor(0, 0);
    lcd.print("Pls Select Item");
  }
}

void motorLift(bool condition) {
  while (true) {
    buttonStatu = digitalRead(button);
    if (buttonStatu == LOW) {
      break;
    }
    stepperMotorRun(10, 11, 10, condition, 0);
  }
  Serial.println("Escape");
}
