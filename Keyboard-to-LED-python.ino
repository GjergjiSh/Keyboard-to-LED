
int const LED = 13;
int const LED2 = 12;
int const BUZZER = 10;
int const BEEPER = 8;

String incomingByte ;    

void setup() {

  Serial.begin(460800); //Massive baudrate to handle keyboard spamming because I am speed!
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BEEPER, OUTPUT);
  digitalWrite(LED, HIGH);
  digitalWrite(LED2, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
}
void loop() {

  while (Serial.available()) {

  incomingByte = Serial.readStringUntil('\n');

    if (incomingByte == "1") {

      digitalWrite(LED, HIGH);
      digitalWrite(BUZZER, HIGH);
      digitalWrite(LED2, LOW);
      digitalWrite(BEEPER, HIGH);
      Serial.flush();

    }

    else if (incomingByte == "0") {

      digitalWrite(LED, LOW);
      digitalWrite(BUZZER, LOW);
      digitalWrite(LED2, HIGH);
      digitalWrite(BEEPER, LOW);
      Serial.flush();

    }

    else{

     Serial.flush();

    }

  }

}
