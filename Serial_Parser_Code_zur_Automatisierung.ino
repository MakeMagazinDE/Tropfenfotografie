/*
Diese Version liest aktiv Werte aus der seriellen Konsole und stellt damit die Tropfenparameter ein.
Die Tropfenparameter werden in zwei Arrays gespeichert: valveTimes für die Zeiten wie lange das Ventil offen sein soll (also Tropfengröße) und waitTimes für die Warteintervalle zwischen Tropfen.
Dabei werden für jeden Wert größer 0 in ValveTimes ein Tropfen fallen gelassen und die Zeit aus waitTimes gewartet und nach dem letzten Warten dann der Blitz ausgelöst.
Für die Initialen Werte 30,30 und 75, 190 heißt dies: 30ms Ventil öffnen, 75ms warten, 30ms Ventil öffnen, 190ms warten, blitzen. Möchte man Tropfen hinzufügen oder löschen kann man einfach den entsprechenden Wert in valveTimes auf 0 setzen.

Verfügbar sind die folgenden Befehle:
"g" für go, lässt die Prozedur einmal laufen (wie beim drücken des Buttons).
"p" für print, zeigt die aktuell eingestellten Parameter in der seriellen Konsole an.
"o 200" für open, öffnet das Ventil für 200ms (zum leeren der Flasche).
"d1 90" für drop(tropfen) eins, setzt beispielhaft die Öffnungszeit des Ventils für den ersten Tropfen auf 90ms 
"w2 20" für wait (warten) zwei, setzt beispielhaft die zweite Wartezeit auf 20 ms.

jeder Befehl wird abegschlossen mit einem \n (Zeilenumbruch) oder einem  "," so dass auch mehrfach Befehle wie
"d1 20,w1 30,g"
einfach per serieller Konsole sendbar sind.
*/


#define BTN_PIN   3
#define FLASH_PIN 4
#define VLV_PIN   2

const byte numChars = 32;
char receivedChars[numChars]; //buffer für die empfangenen Zeichen
boolean newData = false;

int valveTimes[]={30,30,0,0,0,0};
int waitTimes[]={75,190,0,0,0,0};
const byte dropCount = 6;

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(FLASH_PIN, OUTPUT);
  pinMode(VLV_PIN, OUTPUT);

  digitalWrite(FLASH_PIN, LOW);
  digitalWrite(VLV_PIN, LOW);

  Serial.begin(9600);
  Serial.println("Drop Controller v1.1");
  Serial.println("Values:");
  printProcedure();
}

void loop() {
  if (digitalRead(BTN_PIN) == LOW) startProcedure();
  checkSerial();
  if (newData == true) parseNewData();
}

void printProcedure(){
  for (byte i=0; i<dropCount; i++){
    if(valveTimes[i]!=0){
      Serial.print(i+1);
      Serial.print(": Drop ");
      Serial.print(valveTimes[i]);
      Serial.print("ms, wait ");
      Serial.print(waitTimes[i]);
      Serial.println("ms."); 
    }
  }
  //Serial.println("Flash 50ms.");
}

void startProcedure(){
  Serial.print("Starting procedure...");
  for (byte i=0; i<dropCount; i++){
    if(valveTimes[i]!=0){
      digitalWrite(VLV_PIN, HIGH);
      delay(valveTimes[i]);
      digitalWrite(VLV_PIN, LOW);
      delay(waitTimes[i]);  
    }
  }
  digitalWrite(FLASH_PIN, HIGH);
  delay(50);
  digitalWrite(FLASH_PIN, LOW);
  while (digitalRead(BTN_PIN) == LOW) {}
  Serial.println("Done");
}

void checkSerial() {
  static byte ndx = 0;
  char endMarker1 = '\n';
  char endMarker2 = ',';
  char rc;
    
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (rc != endMarker1 && rc != endMarker2) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    } else {
       receivedChars[ndx] = '\0'; // terminate the string
       ndx = 0;
       newData = true;
    }
  }
}


void parseNewData() {
  //Serial.println("Parsing input: ");
  newData = false;
  int pos = int(receivedChars[1])-48-1;
  switch (receivedChars[0]) {
  case 'd':
    if(pos < 0 || pos >= dropCount){
      Serial.print("Command out of range: "); Serial.println(receivedChars);
    } else {
      valveTimes[pos]= atoi(&receivedChars[3]);
      Serial.println("New Values:");
      printProcedure();
    }
    break;
  case 'w':
    if(pos < 0 || pos >= dropCount){
      Serial.print("Command out of range: "); Serial.println(receivedChars);
    } else {
      waitTimes[pos]= atoi(&receivedChars[3]);
      Serial.println("New Values:");
      printProcedure();
    }
    break;
  case 'p':
    printProcedure();
    break;
  case 'g':
    startProcedure();
    break;
  case 'o':
    Serial.print("Opening valve...");
    digitalWrite(VLV_PIN, HIGH);
    delay(atoi(&receivedChars[2]));
    digitalWrite(VLV_PIN, LOW);
    Serial.println("Done");
    break;
  default:
    Serial.print("Unknown command: "); Serial.println(receivedChars); break;
  }
}
