/*

  Samco - Arduino Powered IR Light Gun

  Go to https://github.com/samuelballantyne for code & instructions.

  created June 2019
  by Sam Ballantyne

  This sample code is part of the public domain.

*/

enum coordinate {
  X,
  Y
};

int rawPosition[4][2];                 // Four raw positions from sensor
int finalPosition[2] = {0, 0};         // Pointer position after tilt correction
int calibPosition[2][2] = {{128,672},  // Two calibration positions (top-left, bottom-right)
                           {896,96}};  // Default calibration at 1/8 of the sensor range

int MoveXAxis = 0;              // Unconstrained mouse postion
int MoveYAxis = 0;               

int conMoveXAxis = 0;           // Constrained mouse postion
int conMoveYAxis = 0;           

enum ratioName {
  r4by3,
  r16by9,
  r21by9,
};

enum ratioIdentifier {
  R,
  TL,
  BR
};

const int ratio[][3][2] = {{{4096,3072},{512,384},{3584,2688}},       // Resolution, Top-Left, Bottom-Right
                           {{4096,2304},{512,288},{3584,2016}},
                           {{5376,2304},{672,288},{4704,2016}}};

int screen = r16by9;

enum stateName {
  Paused,
  RatioSelection,
  TopLeft,
  BottomRight,
  Action
};

// Set intial state
int state = Paused;

enum modeName {
  AutoFire,
  SingleShot
};

int mode = SingleShot;

// number of buttons
const int buttons = 11;

enum buttonName {
  Trigger,
  Up,
  Down,
  Left,
  Right,
  A,
  B,
  Start,
  Select,
  Reload,
  Pedal
};

// original ribbon direction (pin 1 at top)
const int pin[] = {13, 10, 12, 11, 9, A1, A0, A2, A3, 7, 5};
// reversed ribbon direction (pin 1 at bottom)
// const int pin[] = {7, 11, 9, 10, 12, A1, A0, A2, A3, 13, 5};

// button states
int buttonState[buttons];
int lastButtonState[buttons];

#include <HID.h>                // Load libraries
#include <Wire.h>
#include <Keyboard.h>
#include <AbsMouse.h>
#include <DFRobotIRPosition.h>

DFRobotIRPosition myDFRobotIRPosition;

void setup() {
  Serial.begin(9600);                     // For debugging (make sure your serial monitor has the same baud rate)

  for (int i=0; i<buttons; i++) {
    pinMode(pin[i], INPUT_PULLUP);         // Set pin modes
    lastButtonState[i] = 0;                // Initialized last button state
  }
  
  myDFRobotIRPosition.begin();            // Start IR Camera
  
  AbsMouse.init(ratio[screen][R][X], ratio[screen][R][Y]);
  AbsMouse.move((ratio[screen][R][X] / 2), (ratio[screen][R][Y] / 2));          // Set mouse position to centre of the screen
  
  delay(500);
}

void loop() {
  readButtonStates();
  getPosition();

  switch (state) {
    case Paused:
      pauseButtons();
      break;
    case RatioSelection:
      selectionButtons();
      demoRatio();
      break;
    case TopLeft:
      AbsMouse.move(ratio[screen][TL][X], ratio[screen][TL][Y]);
      calibButtons();
      calibPosition[0][X] = finalPosition[X];
      calibPosition[0][Y] = finalPosition[Y];
      break;
    case BottomRight:
      AbsMouse.move(ratio[screen][BR][X], ratio[screen][BR][Y]);
      calibButtons();
      calibPosition[1][X] = finalPosition[X];
      calibPosition[1][Y] = finalPosition[Y];
      break;
    case Action:
      MoveXAxis = map (finalPosition[X], calibPosition[0][X], calibPosition[1][X], ratio[screen][TL][X], ratio[screen][BR][X]);
      MoveYAxis = map (finalPosition[Y], calibPosition[0][Y], calibPosition[1][Y], ratio[screen][TL][Y], ratio[screen][BR][Y]);
      conMoveXAxis = constrain (MoveXAxis, 0, ratio[screen][R][X]);
      conMoveYAxis = constrain (MoveYAxis, 0, ratio[screen][R][Y]);
      AbsMouse.move(conMoveXAxis, conMoveYAxis);
      actionButtons();
      break;
    default:
      state = Paused;
      break;
  }
  
  saveButtonStates();
  //PrintResults();
}


/*        -----------------------------------------------        */
/* --------------------------- METHODS ------------------------- */
/*        -----------------------------------------------        */

void getPosition() {    // Get tilt adjusted position from IR postioning camera
  myDFRobotIRPosition.requestPosition();
  if (myDFRobotIRPosition.available()) {
    for (int i=0; i<4; i++) {
      rawPosition[i][X] = myDFRobotIRPosition.readX(i);
      rawPosition[i][Y] = myDFRobotIRPosition.readY(i);
    }
    if (rawPosition[0][X] > rawPosition[1][X]) {
      for (int i=0; i<2; i++) {
        int temp = rawPosition[0][i];
        rawPosition[0][i] = rawPosition[1][i];
        rawPosition[1][i] = temp;
      }
    } else if (rawPosition[0][X] == rawPosition[1][X]) {
      for (int i=0; i<2; i++) {
        rawPosition[i][X] = 1023;
        rawPosition[i][Y] = 0;
      }
    }
    finalPosition[X] = 512 + cos(atan2(rawPosition[1][Y] - rawPosition[0][Y], rawPosition[1][X] - rawPosition[0][X]) * -1) * (((rawPosition[0][X] - rawPosition[1][X]) / 2 + rawPosition[1][X]) - 512) - sin(atan2(rawPosition[1][Y] - rawPosition[0][Y], rawPosition[1][X] - rawPosition[0][X]) * -1) * (((rawPosition[0][Y] - rawPosition[1][Y]) / 2 + rawPosition[1][Y]) - 384);
    finalPosition[Y] = 384 + sin(atan2(rawPosition[1][Y] - rawPosition[0][Y], rawPosition[1][X] - rawPosition[0][X]) * -1) * (((rawPosition[0][X] - rawPosition[1][X]) / 2 + rawPosition[1][X]) - 512) + cos(atan2(rawPosition[1][Y] - rawPosition[0][Y], rawPosition[1][X] - rawPosition[0][X]) * -1) * (((rawPosition[0][Y] - rawPosition[1][Y]) / 2 + rawPosition[1][Y]) - 384);
  } else {
    Serial.println("Device not available!");
  }
}

void readButtonStates() {
  for (int i=0; i<buttons; i++) {
    buttonState[i] = digitalRead(pin[i]);
  }
}

void saveButtonStates() {
  for (int i=0; i<buttons; i++) {
    lastButtonState[i] = buttonState[i];
  }
}

void pauseButtons() {    // Button funtions in pause state
  if (buttonState[Reload] != lastButtonState[Reload]) {
    if (buttonState[Reload] == LOW) {
      state = Action;
    }
    delay(50);
  }
  if (buttonState[Trigger] != lastButtonState[Trigger]) {
    if (buttonState[Trigger] == LOW) {
      state = TopLeft;
    }
    delay(50);
  }
  if (buttonState[Up] != lastButtonState[Up]) {
    if (buttonState[Up] == LOW) {
      state = RatioSelection;
    }
    delay(50);
  }
  if (buttonState[Down] != lastButtonState[Down]) {
    if (buttonState[Down] == LOW) {
      state = RatioSelection;
    }
    delay(50);
  }
  if (buttonState[A] != lastButtonState[A]) {
    if (buttonState[A] == LOW) {
      mode = AutoFire;
    }
    delay(50);
  }
  if (buttonState[B] != lastButtonState[B]) {
    if (buttonState[B] == LOW) {
      mode = SingleShot;
    }
    delay(50);
  }
}

void selectionButtons() {    // Button funtions in pause state
  if (buttonState[Reload] != lastButtonState[Reload]) {
    if (buttonState[Reload] == LOW) {
      state = Action;
    }
    delay(50);
  }
  if (buttonState[Trigger] != lastButtonState[Trigger]) {
    if (buttonState[Trigger] == LOW) {
      state = TopLeft;
    }
    delay(50);
  }
  if (buttonState[Up] != lastButtonState[Up]) {
    if (buttonState[Up] == LOW) {
      screen = constrain(screen+1,0,r21by9);
      AbsMouse.init(ratio[screen][R][X], ratio[screen][R][Y]);
    }
    delay(50);
  }
  if (buttonState[Down] != lastButtonState[Down]) {
    if (buttonState[Down] == LOW) {
     screen = constrain(screen-1,0,r21by9);
     AbsMouse.init(ratio[screen][R][X], ratio[screen][R][Y]);
    }
    delay(50);
  }
}

void calibButtons() {    // Pause/Re-calibrate button
  if (buttonState[Reload] != lastButtonState[Reload]) {
    if (buttonState[Reload] == LOW) {
      state = Paused;
    }
    delay(50);
  }
  if (buttonState[Trigger] != lastButtonState[Trigger]) {
    if (buttonState[Trigger] == LOW) {
      state++;
    }
    delay(50);
  }
}

void actionButtons() {    // Setup Left, Right & Middle Mouse buttons
  if (mode == AutoFire && buttonState[Trigger] == LOW) {
    AbsMouse.press(MOUSE_LEFT);
    AbsMouse.release(MOUSE_LEFT);
    delay(10);
  } else if (buttonState[Trigger] != lastButtonState[Trigger]) {
    if (buttonState[Trigger] == LOW) {
        AbsMouse.press(MOUSE_LEFT);
    } else {
        AbsMouse.release(MOUSE_LEFT);
    }
    delay(10);
  }
  if (buttonState[Up] != lastButtonState[Up]) {
    if (buttonState[Up] == LOW) {
      Keyboard.press(KEY_UP_ARROW);
      delay(100);
      Keyboard.releaseAll();
    }
    delay(10);
  }
  if (buttonState[Down] != lastButtonState[Down]) {
    if (buttonState[Down] == LOW) {
      Keyboard.press(KEY_DOWN_ARROW);
      delay(100);
      Keyboard.releaseAll();
    }
    delay(10);
  }
  if (buttonState[Left] != lastButtonState[Left]) {
    if (buttonState[Left] == LOW) {
      Keyboard.press(KEY_LEFT_ARROW);
      delay(100);
      Keyboard.releaseAll();
    }
    delay(10);
  }
  if (buttonState[Right] != lastButtonState[Right]) {
    if (buttonState[Right] == LOW) {
      Keyboard.press(KEY_RIGHT_ARROW);
      delay(100);
      Keyboard.releaseAll();
    }
    delay(10);
  }
  if (buttonState[A] != lastButtonState[A]) {
    if (buttonState[A] == LOW) {
      AbsMouse.press(MOUSE_RIGHT);
    } else {
      AbsMouse.release(MOUSE_RIGHT);
    }
    delay(10);
  }
  if (buttonState[B] != lastButtonState[B]) {
    if (buttonState[B] == LOW) {
      AbsMouse.press(MOUSE_MIDDLE);
    } else {
      AbsMouse.release(MOUSE_MIDDLE);
    }
    delay(10);
  }
  if (buttonState[Start] != lastButtonState[Start]) {
    if (buttonState[Start] == LOW) {
      Keyboard.press(KEY_BACKSPACE);
      delay(100);
      Keyboard.releaseAll();
    }
    delay(10);
  }
  if (buttonState[Select] != lastButtonState[Select]) {
    if (buttonState[Select] == LOW) {
      Keyboard.press(KEY_RETURN);
      delay(100);
      Keyboard.releaseAll();
    }
    delay(10);
  }
  if (buttonState[Reload] != lastButtonState[Reload]) {
    if (buttonState[Reload] == LOW) {
      state = Paused;
    }
    delay(10);
  }
}

void demoRatio() {
  static bool toggle = true;
  static unsigned long tick = millis();

  if ((millis() - tick) > 500) {
    if (toggle) {
      AbsMouse.move(ratio[screen][TL][X], ratio[screen][TL][Y]);
    } else {
      AbsMouse.move(ratio[screen][BR][X], ratio[screen][BR][Y]);
    }
    toggle = !toggle;
    tick = millis();
    Serial.println(screen);
  }
}

void PrintResults() {    // Print results for debugging
  Serial.print("RAW: ");
  Serial.print(finalPosition[X]);
  Serial.print(", ");
  Serial.print(finalPosition[Y]);
  Serial.print("     State: ");
  Serial.print(state);
  Serial.print("     Calibration: ");
  Serial.print(calibPosition[0][X]);
  Serial.print(", ");
  Serial.print(calibPosition[0][Y]);
  Serial.print(", ");
  Serial.print(calibPosition[1][X]);
  Serial.print(", ");
  Serial.print(calibPosition[1][Y]);
  Serial.print("     Position: ");
  Serial.print(conMoveXAxis);
  Serial.print(", ");
  Serial.println(conMoveYAxis);
}
