#include <Servo.h>
#include <LiquidCrystal.h>

const int TRIGGER_ESTERNO = 2;
const int ECHO_ESTERNO = 3;

const int TRIGGER_INTERNO = 4;
const int ECHO_INTERNO = 5;

const int PIN_LED_VERDE = 8;
const int PIN_LED_ROSSO = 9;
const int PIN_BUZZER = 10;

const int PIN_SERVO = 6;

const int LCD_RS = 12;
const int LCD_E  = 11;
const int LCD_D4 = 7;
const int LCD_D5 = A0;
const int LCD_D6 = A1;
const int LCD_D7 = A2;

const int postiTotali = 50;
int postiOccupati = 0;

Servo sbarra;
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

long misuraDistanza(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
  long durata = pulseIn(echoPin, HIGH, 30000);
  long distanza = durata * 0.034 / 2;
  return distanza;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_ROSSO, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  pinMode(TRIGGER_ESTERNO, OUTPUT);
  pinMode(ECHO_ESTERNO, INPUT);
  pinMode(TRIGGER_INTERNO, OUTPUT);
  pinMode(ECHO_INTERNO, INPUT);
  
  sbarra.attach(PIN_SERVO);
  sbarra.write(0);  

  lcd.begin(16, 2);
  
  aggiornaDisplay();
}

void loop() {
  long distanzaEsterno = misuraDistanza(TRIGGER_ESTERNO, ECHO_ESTERNO);
  long distanzaInterno = misuraDistanza(TRIGGER_INTERNO, ECHO_INTERNO);

  Serial.print("Distanza Esterno: ");
  Serial.print(distanzaEsterno);
  Serial.print(" cm | Distanza Interno: ");
  Serial.print(distanzaInterno);
  Serial.println(" cm");

  if (distanzaEsterno > 0 && distanzaEsterno < 20) {  
    Serial.println("Auto in ingresso!");
    if (postiOccupati < postiTotali) {
      postiOccupati++;
      aggiornaDisplay();
      azionaSbarra();
    } else {
      Serial.println("Parcheggio pieno!");
      digitalWrite(PIN_LED_ROSSO, HIGH);
      digitalWrite(PIN_LED_VERDE, LOW);
    }
    delay(2000);
  }

  if (distanzaInterno > 0 && distanzaInterno < 20) {  
    Serial.println("Auto in uscita!");
    azionaSbarra();
    if (postiOccupati > 0) {
      postiOccupati--;
      aggiornaDisplay();
    }
    delay(2000);
  }

  delay(500);
}

void aggiornaDisplay() {
  lcd.clear();
  lcd.print("Occupati: ");
  lcd.print(postiOccupati);
  lcd.setCursor(0, 1);
  lcd.print("Liberi: ");
  lcd.print(postiTotali - postiOccupati);

  if (postiOccupati < postiTotali) {
    digitalWrite(PIN_LED_VERDE, HIGH);
    digitalWrite(PIN_LED_ROSSO, LOW);
  } else {
    digitalWrite(PIN_LED_VERDE, LOW);
    digitalWrite(PIN_LED_ROSSO, HIGH);
  }
}

void azionaSbarra() {
  sbarra.write(90);  
  for(int i = 0; i < 4; i++) {
    tone(PIN_BUZZER, 500);
    delay(500);
    noTone(PIN_BUZZER);
    delay(500);
  }
  sbarra.write(0); 
  noTone(PIN_BUZZER);  
}
