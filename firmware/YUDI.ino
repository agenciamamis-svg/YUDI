#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Pantalla LCD y DFPlayer
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial dfSerial(10, 11); // TX, RX
DFRobotDFPlayerMini myDFPlayer;

// Carita parpadeando (código base)
byte eyeOpen[8] = {
  B00000,
  B01110,
  B01110,
  B01110,
  B01110,
  B00000,
  B00000,
  B00000
};

byte eyeClosed[8] = {
  B00000,
  B00000,
  B00000,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000
};

byte mouthHappyLeft[8] = {
  B00000,
  B00000,
  B11111,
  B10000,
  B01000,
  B00111,
  B00000,
  B00000
};

byte mouthHappyRight[8] = {
  B00000,
  B00000,
  B11111,
  B00001,
  B00010,
  B11100,
  B00000,
  B00000
};

void drawFace(bool eyesOpen) {
  lcd.setCursor(5, 0);
  lcd.write(eyesOpen ? byte(0) : byte(1));
  lcd.setCursor(10, 0);
  lcd.write(eyesOpen ? byte(0) : byte(1));

  lcd.setCursor(7, 1);
  lcd.write(byte(2));
  lcd.setCursor(8, 1);
  lcd.write(byte(3));
}

void drawEyes(bool open) {
  lcd.setCursor(5, 0); lcd.write(open ? byte(0) : byte(1));
  lcd.setCursor(10, 0); lcd.write(open ? byte(0) : byte(1));
}

// Pines de botones
const uint8_t PIN_MOLESTO = 2; // Enojado
const uint8_t PIN_FELIZ = 3;
const uint8_t PIN_TRISTE = 4;
const uint8_t PIN_HOLA = 5;
const uint8_t PIN_GRACIAS = 6;
const uint8_t PIN_PORFAVOR = 7;
const uint8_t PIN_DENADA = 8;
const uint8_t PIN_HASTALUEGO = 9;
const uint8_t PIN_SI = A0;
const uint8_t PIN_NO = A1;
const uint8_t PIN_MODO_EMOCIONES = A2;
const uint8_t PIN_MODO_CORTESIA = A3;

// Temporizadores
const unsigned long TIMEOUT_15S = 15000UL;

// Control de texto para evitar parpadeos
String currentLine1 = "";
String currentLine2 = "";

void printLCD(const String &l1, const String &l2) {
  if (l1 != currentLine1) {
    lcd.setCursor(0, 0);
    String text = l1;
    if (text.length() < 16) text += String(' ', 16 - text.length());
    lcd.print(text.substring(0, 16));
    currentLine1 = l1;
  }
  if (l2 != currentLine2) {
    lcd.setCursor(0, 1);
    String text = l2;
    if (text.length() < 16) text += String(' ', 16 - text.length());
    lcd.print(text.substring(0, 16));
    currentLine2 = l2;
  }
}

// Estado general
enum MainState {
  START_TEAYUDA_ANIM,
  START_TEAYUDA_HOLD,
  START_FACE_BLINK,
  START_PRESENTATION,
  START_PLAYTIME,
  MODE_PROMPT,
  MODE_WAIT,
  MODE_TO_CORTESIA,
  MODE_TO_EMOCIONES,
  CORTESIA_ASK,
  CORTESIA_WAIT,
  CORTESIA_SUCCESS,
  CORTESIA_ERROR,
  CORTESIA_REPEAT,
  CORTESIA_CLOSE1,
  CORTESIA_CLOSE2,
  CORTESIA_CLOSE3,
  CORTESIA_CLOSE4,
  CORTESIA_REPLAY_WAIT,
  EMO_Q1_PROMPT,
  EMO_Q1_WAIT,
  EMO_Q1_FEEDBACK,
  EMO_ASK,
  EMO_WAIT,
  EMO_SUCCESS,
  EMO_CLOSE1,
  EMO_CLOSE2,
  EMO_CLOSE3,
  EMO_CLOSE4,
  EMO_REPLAY_WAIT,
  INTERRUPT_CONFIRM,
  INTERRUPT_WAIT,
  INTERRUPT_RESUME,
  GENERAL_CLOSING,
  GENERAL_WAIT_ANY,
  GENERAL_RESTART_PROMPT,
  GENERAL_RESTART_WAIT
};

MainState state = START_TEAYUDA_ANIM;
MainState returnState = MODE_PROMPT;
unsigned long stateStart = 0;
unsigned long blinkTimer = 0;
bool eyesAreOpen = true;
bool blinkingEnabled = false;
uint8_t cortesiaIndex = 0;
uint8_t emocionesIndex = 0;
bool emoFromAngry = false;
bool emoBreathDone = false;
bool emoReplayPrompted = false;
bool emoAngerMessageShown = false;

struct CortesiaQuestion {
  const char *l1;
  const char *l2;
  uint8_t pin;
  const char *success1;
  const char *success2;
  const char *repeatWord;
};

CortesiaQuestion cortesia[5] = {
  {"Un amigo", "te dice hola", PIN_HOLA, "Gran trabajo!", "", "Hola"},
  {"Te regalan", "un dulce", PIN_GRACIAS, "Bravo!", "", "Gracias"},
  {"Alguien te", "dice gracias", PIN_DENADA, "Muy bien!", "", "De nada"},
  {"Al pedir algo.", "Se dice:", PIN_PORFAVOR, "Super bien!", "", "Por favor"},
  {"El adiós mágico", "es:", PIN_HASTALUEGO, "Super bien!", "", "Hasta luego"}
};

struct EmocionQuestion {
  const char *l1;
  const char *l2;
  uint8_t pin;
  const char *success1;
  const char *success2;
};

EmocionQuestion emociones[4] = {
  {"Un amigo", "esta llorando", PIN_TRISTE, "Muy bien!", "Lo reconociste"},
  {"Si estas feliz", "puedes decir...", PIN_FELIZ, "Genial!", "Sonríe y comparte"},
  {"Si estas triste", "puedes decir…", PIN_TRISTE, "Bien hecho,", "eres valiente!"},
  {"Si estas enojado", "puedes decir...", PIN_MOLESTO, "Muy bien!", ""}
};

// Lectura de botones (asumiendo INPUT_PULLUP)
bool buttonPressed(uint8_t pin) {
  return digitalRead(pin) == LOW;
}

void resetBlink(bool enable) {
  blinkingEnabled = enable;
  blinkTimer = millis();
  eyesAreOpen = true;
  if (enable) {
    drawFace(true);
  }
}

void updateBlink() {
  if (!blinkingEnabled) return;
  unsigned long now = millis();
  if (now - blinkTimer > 1200) {
    eyesAreOpen = !eyesAreOpen;
    drawEyes(eyesAreOpen);
    blinkTimer = now;
  }
}

void playAudio(uint16_t track) {
  myDFPlayer.play(track);
}

void setState(MainState newState) {
  state = newState;
  stateStart = millis();
}

void setup() {
  pinMode(PIN_MOLESTO, INPUT_PULLUP);
  pinMode(PIN_FELIZ, INPUT_PULLUP);
  pinMode(PIN_TRISTE, INPUT_PULLUP);
  pinMode(PIN_HOLA, INPUT_PULLUP);
  pinMode(PIN_GRACIAS, INPUT_PULLUP);
  pinMode(PIN_PORFAVOR, INPUT_PULLUP);
  pinMode(PIN_DENADA, INPUT_PULLUP);
  pinMode(PIN_HASTALUEGO, INPUT_PULLUP);
  pinMode(PIN_SI, INPUT_PULLUP);
  pinMode(PIN_NO, INPUT_PULLUP);
  pinMode(PIN_MODO_EMOCIONES, INPUT_PULLUP);
  pinMode(PIN_MODO_CORTESIA, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, eyeOpen);
  lcd.createChar(1, eyeClosed);
  lcd.createChar(2, mouthHappyLeft);
  lcd.createChar(3, mouthHappyRight);
  lcd.clear();
  currentLine1 = "";
  currentLine2 = "";

  dfSerial.begin(9600);
  if (myDFPlayer.begin(dfSerial)) {
    myDFPlayer.volume(10); // Volumen establecido en 10
  }

  setState(START_TEAYUDA_ANIM);
}

void loop() {
  updateBlink();

  switch (state) {
    case START_TEAYUDA_ANIM:
      printLCD("TEAYUDA", "");
      resetBlink(true);
      if (millis() - stateStart > 4000) {
        setState(START_TEAYUDA_HOLD);
      }
      break;

    case START_TEAYUDA_HOLD:
      printLCD("TEAYUDA", "");
      if (millis() - stateStart > 3000) {
        setState(START_FACE_BLINK);
        resetBlink(true);
      }
      break;

    case START_FACE_BLINK:
      printLCD("", "");
      drawFace(true);
      resetBlink(true);
      if (millis() - stateStart > 4000) {
        resetBlink(false);
        setState(START_PRESENTATION);
      }
      break;

    case START_PRESENTATION:
      printLCD("Soy Yudi,", "la nube");
      playAudio(1);
      if (millis() - stateStart > 3000) {
        setState(START_PLAYTIME);
      }
      break;

    case START_PLAYTIME:
      printLCD("ES HORA DE", "JUGAR!");
      playAudio(3);
      if (millis() - stateStart > 3000) {
        setState(MODE_PROMPT);
      }
      break;

    case MODE_PROMPT:
      printLCD("Elige un modo", "");
      playAudio(47);
      resetBlink(true);
      setState(MODE_WAIT);
      break;

    case MODE_WAIT:
      if (buttonPressed(PIN_MODO_CORTESIA)) {
        resetBlink(false);
        cortesiaIndex = 0;
        setState(MODE_TO_CORTESIA);
      } else if (buttonPressed(PIN_MODO_EMOCIONES)) {
        resetBlink(false);
        emocionesIndex = 0;
        setState(MODE_TO_EMOCIONES);
      } else if (millis() - stateStart > TIMEOUT_15S) {
        resetBlink(false);
        setState(GENERAL_CLOSING);
      }
      break;

    case MODE_TO_CORTESIA:
      printLCD("Modo Cortesia", "");
      playAudio(2);
      if (millis() - stateStart > 2000) {
        setState(CORTESIA_ASK);
      }
      break;

    case MODE_TO_EMOCIONES:
      printLCD("Modo Emociones", "");
      playAudio(27);
      if (millis() - stateStart > 2000) {
        setState(EMO_Q1_PROMPT);
      }
      break;

    case CORTESIA_ASK: {
      if (cortesiaIndex >= 5) {
        setState(CORTESIA_CLOSE1);
        break;
      }
      printLCD(cortesia[cortesiaIndex].l1, cortesia[cortesiaIndex].l2);
      if (cortesiaIndex == 0) {
        playAudio(5);
        playAudio(6);
      } else if (cortesiaIndex == 1) {
        playAudio(12);
        playAudio(13);
      } else if (cortesiaIndex == 2) {
        playAudio(18);
        playAudio(13);
      } else if (cortesiaIndex == 3) {
        playAudio(21);
      } else if (cortesiaIndex == 4) {
        playAudio(48);
      }
      setState(CORTESIA_WAIT);
      break;
    }

    case CORTESIA_WAIT: {
      if (buttonPressed(PIN_NO)) {
        returnState = CORTESIA_WAIT;
        setState(INTERRUPT_CONFIRM);
        break;
      }
      if (buttonPressed(cortesia[cortesiaIndex].pin)) {
        const char *s1 = cortesia[cortesiaIndex].success1;
        const char *s2 = cortesia[cortesiaIndex].success2;
        printLCD(s1, s2);
        if (cortesiaIndex == 0) playAudio(8);
        else if (cortesiaIndex == 1) playAudio(15);
        else if (cortesiaIndex == 2) playAudio(20);
        else playAudio(20);
        setState(CORTESIA_SUCCESS);
      } else if (millis() - stateStart > TIMEOUT_15S) {
        printLCD("Probemos", "de nuevo");
        playAudio(57);
        playAudio(10);
        setState(CORTESIA_ERROR);
      }
      break;
    }

    case CORTESIA_SUCCESS:
      if (millis() - stateStart > 2000) {
        cortesiaIndex++;
        setState(CORTESIA_ASK);
      }
      break;

    case CORTESIA_ERROR:
      if (millis() - stateStart > 2000) {
        printLCD("Repite conmigo:", cortesia[cortesiaIndex].repeatWord);
        playAudio(11);
        if (cortesiaIndex == 0) playAudio(7);
        else if (cortesiaIndex == 1) playAudio(14);
        else if (cortesiaIndex == 2) playAudio(19);
        else if (cortesiaIndex == 3) playAudio(22);
        else if (cortesiaIndex == 4) playAudio(49);
        setState(CORTESIA_REPEAT);
      }
      break;

    case CORTESIA_REPEAT:
      if (millis() - stateStart > 3000) {
        setState(CORTESIA_ASK);
      }
      break;

    case CORTESIA_CLOSE1:
      printLCD("Gracias por", "jugar conmigo");
      playAudio(23);
      setState(CORTESIA_CLOSE2);
      break;

    case CORTESIA_CLOSE2:
      if (millis() - stateStart > 2000) {
        printLCD("Eres genial!", "");
        setState(CORTESIA_CLOSE3);
      }
      break;

    case CORTESIA_CLOSE3:
      if (millis() - stateStart > 2000) {
        printLCD("Modo Cortesia", "Terminado!");
        playAudio(50);
        setState(CORTESIA_CLOSE4);
      }
      break;

    case CORTESIA_CLOSE4:
      if (millis() - stateStart > 2000) {
        printLCD("Quieres jugar", "de nuevo?");
        playAudio(24);
        setState(CORTESIA_REPLAY_WAIT);
      }
      break;

    case CORTESIA_REPLAY_WAIT:
      if (buttonPressed(PIN_SI)) {
        printLCD("Perfecto,", "vamos!");
        playAudio(25);
        setState(MODE_PROMPT);
      } else if (buttonPressed(PIN_NO) || millis() - stateStart > TIMEOUT_15S) {
        printLCD("Esta bien,", "jugamos luego");
        playAudio(26);
        setState(GENERAL_CLOSING);
      }
      break;

    case EMO_Q1_PROMPT:
      printLCD("Como te", "sientes ahora?");
      playAudio(28);
      setState(EMO_Q1_WAIT);
      break;

    case EMO_Q1_WAIT:
      if (buttonPressed(PIN_NO)) {
        returnState = EMO_Q1_WAIT;
        setState(INTERRUPT_CONFIRM);
        break;
      }
      if (buttonPressed(PIN_FELIZ)) {
        printLCD("Que alegria!", "");
        playAudio(30);
        emoFromAngry = false;
        emoBreathDone = false;
        setState(EMO_Q1_FEEDBACK);
      } else if (buttonPressed(PIN_TRISTE)) {
        printLCD("Todo mejorara,", "Estoy contigo");
        playAudio(32);
        emoFromAngry = false;
        emoBreathDone = false;
        setState(EMO_Q1_FEEDBACK);
      } else if (buttonPressed(PIN_MOLESTO)) {
        printLCD("Oh, respira", "conmigo");
        playAudio(34);
        emoFromAngry = true;
        emoBreathDone = false;
        setState(EMO_Q1_FEEDBACK);
      } else if (millis() - stateStart > TIMEOUT_15S) {
        printLCD("Probemos", "de nuevo");
        playAudio(57);
        emoFromAngry = false;
        emoBreathDone = false;
        setState(EMO_Q1_FEEDBACK);
      }
      break;

    case EMO_Q1_FEEDBACK:
      if (emoFromAngry && !emoBreathDone && millis() - stateStart > 2000) {
        printLCD("Inhala", "Exhala");
        playAudio(53);
        playAudio(54);
        emoBreathDone = true;
        stateStart = millis();
      } else if ((!emoFromAngry && millis() - stateStart > 2000) || (emoBreathDone && millis() - stateStart > 3000)) {
        printLCD("Un amigo", "esta llorando");
        playAudio(35);
        playAudio(36);
        emocionesIndex = 0;
        emoBreathDone = false;
        setState(EMO_ASK);
      }
      break;

    case EMO_ASK: {
      if (emocionesIndex >= 4) {
        setState(EMO_CLOSE1);
        break;
      }
      printLCD(emociones[emocionesIndex].l1, emociones[emocionesIndex].l2);
      emoAngerMessageShown = false;
      setState(EMO_WAIT);
      break;
    }

    case EMO_WAIT:
      if (buttonPressed(PIN_NO)) {
        returnState = EMO_WAIT;
        setState(INTERRUPT_CONFIRM);
        break;
      }
      if (buttonPressed(emociones[emocionesIndex].pin)) {
        printLCD(emociones[emocionesIndex].success1, emociones[emocionesIndex].success2);
        if (emocionesIndex == 0) playAudio(37);
        else if (emocionesIndex == 1) playAudio(40);
        else if (emocionesIndex == 2) playAudio(42);
        else {
          playAudio(64);
          playAudio(44);
        }
        setState(EMO_SUCCESS);
      } else if (millis() - stateStart > TIMEOUT_15S) {
        printLCD("Probemos", "de nuevo");
        playAudio(57);
        setState(EMO_SUCCESS);
      }
      break;

    case EMO_SUCCESS:
      if (emocionesIndex == 3 && !emoAngerMessageShown && millis() - stateStart > 2000) {
        printLCD("El enojo se va", "poquito a poquito");
        emoAngerMessageShown = true;
        stateStart = millis();
      } else if (millis() - stateStart > 2000 && (emocionesIndex != 3 || emoAngerMessageShown)) {
        emocionesIndex++;
        setState(EMO_ASK);
      }
      break;

    case EMO_CLOSE1:
      printLCD("Lo lograste!", "Eres valiente");
      playAudio(45);
      setState(EMO_CLOSE2);
      break;

    case EMO_CLOSE2:
      if (millis() - stateStart > 2000) {
        printLCD("Recuerda, todas", "las emociones");
        playAudio(46);
        setState(EMO_CLOSE3);
      }
      break;

    case EMO_CLOSE3:
      if (millis() - stateStart > 2000) {
        printLCD("son importantes", "");
        setState(EMO_CLOSE4);
      }
      break;

    case EMO_CLOSE4:
      if (millis() - stateStart > 2000) {
        printLCD("Modo Emociones", "Terminado!");
        playAudio(51);
        emoReplayPrompted = false;
        setState(EMO_REPLAY_WAIT);
      }
      break;

    case EMO_REPLAY_WAIT:
      if (!emoReplayPrompted) {
        printLCD("Quieres jugar", "de nuevo?");
        playAudio(24);
        emoReplayPrompted = true;
        stateStart = millis();
      }
      if (buttonPressed(PIN_SI)) {
        printLCD("Perfecto,", "vamos!");
        playAudio(25);
        emoReplayPrompted = false;
        setState(MODE_PROMPT);
      } else if (buttonPressed(PIN_NO) || millis() - stateStart > TIMEOUT_15S) {
        printLCD("Esta bien,", "jugamos luego");
        playAudio(26);
        emoReplayPrompted = false;
        setState(GENERAL_CLOSING);
      }
      break;

    case INTERRUPT_CONFIRM:
      printLCD("Quieres terminar", "el juego?");
      playAudio(61);
      setState(INTERRUPT_WAIT);
      break;

    case INTERRUPT_WAIT:
      if (buttonPressed(PIN_SI)) {
        printLCD("Esta bien,", "te espero luego");
        playAudio(62);
        setState(GENERAL_CLOSING);
      } else if (buttonPressed(PIN_NO)) {
        printLCD("Que bien!", "Sigamos jugando!");
        playAudio(63);
        setState(INTERRUPT_RESUME);
      } else if (millis() - stateStart > TIMEOUT_15S) {
        setState(returnState);
      }
      break;

    case INTERRUPT_RESUME:
      if (millis() - stateStart > 2000) {
        setState(returnState);
      }
      break;

    case GENERAL_CLOSING:
      printLCD("Estoy aqui", "cuando quieras!!");
      playAudio(65);
      setState(GENERAL_WAIT_ANY);
      break;

    case GENERAL_WAIT_ANY:
      if (buttonPressed(PIN_SI) || buttonPressed(PIN_NO) || buttonPressed(PIN_MODO_CORTESIA) || buttonPressed(PIN_MODO_EMOCIONES) ||
          buttonPressed(PIN_HOLA) || buttonPressed(PIN_GRACIAS) || buttonPressed(PIN_TRISTE) || buttonPressed(PIN_FELIZ) || buttonPressed(PIN_MOLESTO) ||
          buttonPressed(PIN_PORFAVOR) || buttonPressed(PIN_DENADA) || buttonPressed(PIN_HASTALUEGO)) {
        printLCD("Quieres jugar", "de nuevo?");
        playAudio(24);
        setState(GENERAL_RESTART_WAIT);
      }
      break;

    case GENERAL_RESTART_WAIT:
      if (buttonPressed(PIN_SI)) {
        printLCD("Perfecto,", "vamos!");
        playAudio(25);
        setState(MODE_PROMPT);
      } else if (buttonPressed(PIN_NO) || millis() - stateStart > TIMEOUT_15S) {
        printLCD("Esta bien,", "jugamos luego");
        playAudio(26);
        if (millis() - stateStart > 2000) {
          setState(GENERAL_CLOSING);
        }
      }
      break;
  }
}
