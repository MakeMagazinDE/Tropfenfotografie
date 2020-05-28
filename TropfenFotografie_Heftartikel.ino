#define BLITZ_PIN 4
#define SCHALTER_PIN 3
#define VENTIL_PIN 2

void setup() {
  pinMode(VENTIL_PIN, OUTPUT);
  pinMode(BLITZ_PIN, OUTPUT);
  pinMode(SCHALTER_PIN, INPUT_PULLUP);
}

void loop() {
  if(digitalRead(SCHALTER_PIN) == LOW){
    tropfen(30);
    delay(75);
    tropfen(30);
    delay(190);
    blitzen();
  }
}

void tropfen(int ms){
  digitalWrite(VENTIL_PIN, HIGH);
  delay(ms);
  digitalWrite(VENTIL_PIN, LOW);
}

void blitzen(){
  digitalWrite(BLITZ_PIN, HIGH);
  delay(100);
  digitalWrite(BLITZ_PIN, LOW);
}
