/*  The Faces Game
 *  Made by Toporivschi Ion, a student for I.T.T. E. Barsanti.
 */
#include <EEPROM.h>
#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//define the buttons
int b1; int b2; int b3;

//time vars that control difficulty/functionality of the game
int tempo; //to know for how long the user was pressing a button
int tempoMax; //corresponds to the maximum amount of time the user is given to press a button, after successfully scoring a point it's decreasing
int tempoMassimo; //used to punish the users who didn't press a button in the time given (triple value of tempoMax), so it's also decreasing with the rise of difficulty level

//game objective vars
int vite; //corresponds to the "lives" the user has at his disposal, the game starts with three
int score; //corresponds to the number of points the user has achieved during his playtime

//EEPROM vars
int scoreE; //0 | the address of the cell
int viteE;  //1 | the address of the cell

//vars that control buttons | all three below execute control functions
int buttonDaPremere; //is assigned a value after a "face" is generated on lcd screen
int ultimoButtonPremuto; //is assigned a value after pressing a button
bool premuto; //used to exit the cycle after pressing a button

//custom characters

//a heart in bytes
byte heart[] = {
  B00000,
  B00000,
  B11011,
  B10101,
  B10001,
  B01010,
  B00100,
  B00000
};
//a "happy face" in bytes
byte happyFace[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B10001,
  B11011,
  B01110,
  B00100
};
//an "upset face" in bytes
byte upsetFace[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B00000,
  B01110,
  B10001,
  B10001
};

//end custom characters

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.createChar(0, heart);
  lcd.createChar(1, happyFace);
  lcd.createChar(2, upsetFace);
  pinMode(10, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  scoreE = EEPROM.read(0); viteE = EEPROM.read(1);
  if (scoreE != 255 && viteE != 255) {
    score = scoreE; vite = viteE;
    tempoMax = 1000-(score*30);
  }
  else {
     vite = 3; score = 0; tempoMax = 1000;
  }
  tempo = 0; tempoMassimo = 3 * tempoMax; premuto = false;
  b1 = 10; b2 = 9; b3 = 8;
  randomSeed(analogRead(0));
}

void loop() {
  
  CaricaCuori(vite); //shows the number of lives
  CaricaScore(); //shows the score
  Azione(); //the main method
  lcd.clear();
  
  if (vite < 1) {
    GameEnded();
  }
}

//interface methods
void CaricaCuori(int numCuori) {
  int lastPlace = 15; //is needed to write byte characters from the last place in the line (which is 15 because the first is 0)
  while (numCuori != 0) {
    lcd.setCursor(lastPlace, 0);
    lcd.write(byte(0));
    numCuori--;
    lastPlace--;
  }
}
void CaricaScore() {
  /* developer's note: can add an if millis() < neccesary time to reach this method, 
   * to be sure it's not triggered during the start of the game, but if the user exits with smaller number of lives
   * while the score is still 0, it, ultimately, will be triggered || just cancell if score != 0
   UPDATE: not delevoped.
  */
  if(score != 0){ 
  EEPROM.write(0, score); 
  }
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.print(score);
}
//end interface methods

//the faces methods
void MostraUnaFaccia(int tipo, int posto) {
  if (posto == 10) {
    lcd.setCursor(3, 1);
  }
  else if (posto == 9) {
    lcd.setCursor(7, 1);
  }
  else if (posto == 8) {
    lcd.setCursor(11, 1);
  }
  lcd.write(byte(tipo));
  buttonDaPremere = posto;
}
void Azione() {

  int tipo = random(0, 3); //generate a random number between 0 and 2 that will correspond to a byte character created before 
  int posto = random(8, 11); //generate a random number that will correspond to a button

  MostraUnaFaccia(tipo, posto);

  while (!premuto) {
    if (tempoMassimo <= 0) { //if tempoMassimo reaches a zero, the cycle is interrupted
      break;
    }
    while (digitalRead(b1) == LOW) {
      if (tempoMassimo <= 0) { //if a button is pressed for too long the cycle will be broken
        break;
        ultimoButtonPremuto = 0;
      }
      tempo++;
      tempoMassimo--;
      delay(1);
      premuto = true;
      ultimoButtonPremuto = 10;
    }
    while (digitalRead(b2) == LOW) {
      if (tempoMassimo <= 0) {
        break;
        ultimoButtonPremuto = 0;
      }
      tempo++;
      tempoMassimo--;
      delay(1);
      premuto = true;
      ultimoButtonPremuto = 9;
    }
    while (digitalRead(b3) == LOW) {
      if (tempoMassimo <= 0) {
        break;
        ultimoButtonPremuto = 0;
      }
      tempo++;
      tempoMassimo--;
      delay(1);
      premuto = true;
      ultimoButtonPremuto = 8;
    }
    tempoMassimo--; //it's constantly decreasing, until it reaches a zero (see why above)
    delay(1);
  }

  //final control

  if (tempo <= tempoMax) {
    if (tipo == 0) { //if the "face" is a heart
      if (ultimoButtonPremuto == buttonDaPremere) { //if the correct button was pressed the user gains a life and a point
        score++;
        vite++;
        EEPROM.write(1, vite);
        tempoMax -= 30;
        if (vite >= 5) { //the maximum amount of lives is five
          vite = 5;
          EEPROM.write(1, vite);
        }
      }
      else {
        vite--;
        EEPROM.write(1, vite);
      }
    }
    else if (tipo == 1) { //if the "face" is a happy face
      if (ultimoButtonPremuto == buttonDaPremere) { //if the correct button was pressed the user gains a point
        score++;
        tempoMax -= 30;
      }
      else {
        vite--;
        EEPROM.write(1, vite);
      }
    }
    else if (tipo == 2) { //if the "face" is an upset face
      if (ultimoButtonPremuto == buttonDaPremere) { //if the correct button was pressed the user loses a life
        vite--;
        EEPROM.write(1, vite);
      }
    }
  }
  else {
    vite--;
    EEPROM.write(1, vite); //every change in number of lives is always written in eeprom memory
  }
  tempo = 0; ultimoButtonPremuto = -1; premuto = false; tempoMassimo = 3 * tempoMax;
}
//end the faces methods

//game ending methods
void GameEnded() {
  while (digitalRead(b3) == HIGH) {
    MostraMessaggioFinito();
  }
  lcd.clear();
  delay(1000);
  EEPROM.write(0, 255); EEPROM.write(1, 255);
  vite = 3; score = 0; tempoMax = 1000;
}
void MostraMessaggioFinito() { 
  //shows the amount of points reached by the end of the game and a message about "how to continue to play"
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over");
  lcd.setCursor(0, 1);
  lcd.print("Total score: ");
  lcd.print(score);
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press 3rd button");
  lcd.setCursor(0, 1);
  lcd.print("to continue");
  delay(1500);
}
//end game ending methods
