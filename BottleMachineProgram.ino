#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte row = 4;
const byte col = 4;

long long int time = 0;

byte rowPins[4] = {12, 11, 10, 9};
byte colPins[4] = {8, 7, 6, 5};

char kpadMatrix[4][4]
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

Keypad controller = Keypad(makeKeymap(kpadMatrix), rowPins, colPins, row, col);

String userPass = "";
String savedPass = "123";

bool swtch = true;
bool finish = false;

const int ledPins[4] = {A3, A2, A1, A0};

const int stepPin = 0; 
const int dirPin = 1;
const int relayPin = 2;
const int enPin = 3;
const int buzzPin = 4;


void stepperControl(char rotation);
void programFlow(char rotation);
void waterPumpControl();
void ledControl();
void buzzerControl();
void passwordChecker();

void setup() {
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(enPin, OUTPUT);
  /*
   * led pins
   */
  pinMode(A3, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(buzzPin, OUTPUT);
  
  digitalWrite(enPin, HIGH);
  digitalWrite(relayPin , HIGH);
  lcd.init();
  lcd.backlight();
}


void loop() {
  char key;
  char rotation;

  delay(300);
  lcd.clear();
  lcd.print("Begin Filling?");
  lcd.setCursor(1, 5);
  lcd.print("Yes/No");
  key = controller.waitForKey();
  
  if(key == '1')
  {
    passwordChecker();
    lcd.clear();
    lcd.print("Clockwise: 1");
    lcd.setCursor(0 , 1);
    lcd.print("AntiClockwise: 0");
    rotation = controller.waitForKey();
    while(!finish)
    {
      digitalWrite(relayPin, HIGH);
      lcd.print("Starting Program");
      delay(550);
      lcd.clear();
      lcd.print("Loading...");
      time = millis();
      while(millis() - time <= 2500)
      {
        ledControl();
      }
      delay(1200);
      lcd.clear();
      lcd.print("Begin Filling!");
      delay(700);
      programFlow(rotation);
    }
    finish = false;
  }
  else
  {
    lcd.clear();
    lcd.print("Cancelling...");
    delay(2000);
    lcd.clear();
    lcd.print("Thank You!");
    delay(1500);
    delay(250);
  }
}


void programFlow(char rotation)
{
  int bottleNumber = 1;
  int counter = 1;
  int ledCounter = 0;
  while(counter <= 4)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    String bottle = String(bottleNumber);
    lcd.print("Filling Bottle: ");
    lcd.setCursor(1, 4);
    lcd.print("No - " + bottle);
    buzzerControl();
    digitalWrite(ledPins[ledCounter], HIGH);
    waterPumpControl();
    delay(5500);
    waterPumpControl();
    delay(2500);
    stepperControl(rotation);
    delay(800);
    bottleNumber++;
    counter++;
    digitalWrite(ledPins[ledCounter], LOW);
    ledCounter++;
  }
  finish = true;
}

void stepperControl(char rotation )
{
  digitalWrite(enPin, LOW);
  digitalWrite(dirPin, (rotation == '1' ? HIGH : LOW));
    for(int x = 0; x < 89; x++) {
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(3500);
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(13000); 
  }
  digitalWrite(enPin, HIGH);
  return;
}


void waterPumpControl()
{
  swtch = !swtch;
  digitalWrite(relayPin , (swtch) ? HIGH : LOW);
}

void ledControl()
{
  digitalWrite(A3, HIGH);
  digitalWrite(A2, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A0, LOW);
  delay(350);
  digitalWrite(A3, LOW);
  digitalWrite(A2, HIGH);
  digitalWrite(A1, LOW);
  digitalWrite(A0, LOW);
  delay(350);
  digitalWrite(A3, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A1, HIGH);
  digitalWrite(A0, LOW);
  delay(350);
  digitalWrite(A3, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A0, HIGH);
  delay(350);
  digitalWrite(A0, LOW);
}

void buzzerControl()
{
  digitalWrite(4, HIGH);
  delay(400);
  digitalWrite(4, LOW);
  delay(400);
  digitalWrite(4, HIGH);
  delay(400);
  digitalWrite(4, LOW);
  delay(400);
  digitalWrite(4, HIGH);
  delay(400);
  digitalWrite(4, LOW);
  delay(1200);
}

void passwordChecker()
{
  while(true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press * to enter");
    lcd.setCursor(0, 1);
    lcd.print("password");
    delay(1200);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("or");
    delay(650);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press # to reset");
    lcd.setCursor(0, 1);
    lcd.print("password");
    delay(1200);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("* or #");
    
    char key = controller.waitForKey();
    
    if(key == '*')
    {
      lcd.clear();
      lcd.print("Please enter");
      lcd.setCursor(0 , 1);
      lcd.print("Password");
      delay(1500);
      lcd.clear();
      char button = ' ';
      int userPassSize = 0;
      userPass = "";
      while(true)
      {
        button = controller.waitForKey();
        if(button == '#')
          break;
        userPass += button;
        userPassSize++;
        if(userPassSize == 16)
        {
          userPassSize = 0;
          lcd.clear();
        }
        lcd.write('*');
      }

      lcd.clear();
      if(userPass == savedPass)
      {
        lcd.print("Correct password");
        delay(2000);
        lcd.clear();
        digitalWrite(A3, HIGH);
        digitalWrite(A2, HIGH);
        digitalWrite(A1, HIGH);
        digitalWrite(A0, HIGH);
        delay(1250);
        digitalWrite(A3, LOW);
        digitalWrite(A2, LOW);
        digitalWrite(A1, LOW);
        digitalWrite(A0, LOW);
        break;
      }
      else
      {
        lcd.print("Wrong password");
        delay(2000);
        lcd.clear();
        buzzerControl();
        ledControl();
      }
    }
    else if(key == '#')
    {
      lcd.clear();
      lcd.print("Please enter");
      lcd.setCursor(0 , 1);
      lcd.print("old Password");
      delay(1500);
      lcd.clear();
      char button = ' ';
      int userPassSize = 0;
      userPass = "";
      while(true)
      {
        button = controller.waitForKey();
        if(button == '#')
          break;
        userPass += button;
        userPassSize++;
        if(userPassSize == 16)
        {
          userPassSize = 0;
          lcd.clear();
        }
        lcd.write('*');
      }
      lcd.clear();
      
      if(userPass == savedPass)
      {
        lcd.print("Correct password");
        delay(2000);
        lcd.clear();

        digitalWrite(A3, HIGH);
        digitalWrite(A2, HIGH);
        digitalWrite(A1, HIGH);
        digitalWrite(A0, HIGH);
        delay(1250);
        digitalWrite(A3, LOW);
        digitalWrite(A2, LOW);
        digitalWrite(A1, LOW);
        digitalWrite(A0, LOW);
        
        savedPass = "";
        char button = ' ';
        int userPassSize = 0;
  
      lcd.print("Please enter");
      lcd.setCursor(0, 1);
      lcd.print("desired password");
      delay(1500);
      lcd.clear();
      
      while(true)
      {
        button = controller.waitForKey();
        if(button == '#')
          break;
        savedPass += button;
        userPassSize++;
        if(userPassSize == 16)
        {
          userPassSize = 0;
          lcd.clear();
        }
        lcd.write('*');
      }
      lcd.clear();
      lcd.print("Password changed");
      lcd.setCursor(0, 1);
      lcd.print("Successfully");
      delay(1500);
      lcd.clear();
      break;
      }
      else
      {
        lcd.print("Wrong password");
        delay(2000);
        lcd.clear();
        buzzerControl();
        ledControl();
      }
    }
  }
}
