/*

  Samco - Arduino Powered IR Light Gun

  Go to https://github.com/samuelballantyne for code & instructions.

  created June 2019
  by Sam Ballantyne

  This sample code is part of the public domain.

*/

int positionX[4];               // RAW Sensor Values
int positionY[4];

int oneY = 0;                   // Re-mapped so left sensor is always read first
int oneX = 0;
int twoY = 0;
int twoX = 0;

int finalX = 0;                 // Values after tilt correction
int finalY = 0;

int xLeft = 0;                  // Stored calibration points
int yTop = 0;
int xRight = 0;
int yBottom = 0;

int MoveXAxis = 0;              // Unconstrained mouse postion
int MoveYAxis = 0;               

int conMoveXAxis = 0;           // Constrained mouse postion
int conMoveYAxis = 0;           

enum state {
  Paused,
  TopLeft,
  BottomRight,
  Action
};

// Set intial state
int currentState = Paused;

// number of buttons
const int buttons = 11;

// button names
enum button {
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

int res_x = 1920;               // Put your screen resolution width here
int res_y = 1080;               // Put your screen resolution height here


void setup() {
  Serial.begin(9600);                     // For debugging (make sure your serial monitor has the same baud rate)

  for (int i=0;i<buttons;i++) {
    pinMode(pin[i], INPUT_PULLUP);         // Set pin modes
    lastButtonState[i] = 0;                // Initialized last button state
  }
  
  myDFRobotIRPosition.begin();            // Start IR Camera
  
  AbsMouse.init(res_x, res_y);
  AbsMouse.move((res_x / 2), (res_y / 2));          // Set mouse position to centre of the screen
  
  delay(500);
}

void loop() {
  readButtonStates();
  getPosition();

  switch (currentState) {
    case Paused:
      skip();
      mouseCount();
      mouseButtons();
      break;
    case TopLeft:
      AbsMouse.move(300, 200);
      mouseCount();
      reset();
      xLeft = finalX;
      yTop = finalY;
      break;
    case BottomRight:
      AbsMouse.move((res_x - 300), (res_y - 200));
      mouseCount();
      reset();
      xRight = finalX;
      yBottom = finalY;
      break;
    case Action:
      AbsMouse.move(conMoveXAxis, conMoveYAxis);
      mouseButtons();
      MoveXAxis = map (finalX, xLeft, xRight, 300, (res_x - 300));
      MoveYAxis = map (finalY, yTop, yBottom, 200, (res_y - 200));
      conMoveXAxis = constrain (MoveXAxis, 0, res_x);
      conMoveYAxis = constrain (MoveYAxis, 0, res_y);
      reset();
      break;
    default:
      currentState = Paused;
      break;
  }
  
  saveButtonStates();
  PrintResults();
}


/*        -----------------------------------------------        */
/* --------------------------- METHODS ------------------------- */
/*        -----------------------------------------------        */

void getPosition() {    // Get tilt adjusted position from IR postioning camera
  myDFRobotIRPosition.requestPosition();
  if (myDFRobotIRPosition.available()) {
    for (int i = 0; i < 4; i++) {
      positionX[i] = myDFRobotIRPosition.readX(i);
      positionY[i] = myDFRobotIRPosition.readY(i);
    }
    if (positionX[0] > positionX[1]) {
      oneY = positionY[0];
      oneX = positionX[0];
      twoY = positionY[1];
      twoX = positionX[1];
    } else if (positionX[0] < positionX[1]) {
      oneY = positionY[1];
      oneX = positionX[1];
      twoY = positionY[0];
      twoX = positionX[0];
    } else {
      oneY = 1023;
      oneX = 0;
      twoY = 1023;
      twoX = 0;
    }
    finalX = 512 + cos(atan2(twoY - oneY, twoX - oneX) * -1) * (((oneX - twoX) / 2 + twoX) - 512) - sin(atan2(twoY - oneY, twoX - oneX) * -1) * (((oneY - twoY) / 2 + twoY) - 384);
    finalY = 384 + sin(atan2(twoY - oneY, twoX - oneX) * -1) * (((oneX - twoX) / 2 + twoX) - 512) + cos(atan2(twoY - oneY, twoX - oneX) * -1) * (((oneY - twoY) / 2 + twoY) - 384);
  } else {
    Serial.println("Device not available!");
  }
}

void readButtonStates() {
  for (int i=0;i<buttons;i++) {
    buttonState[i] = digitalRead(pin[i]);
  }
}

void saveButtonStates() {
  for (int i=0;i<buttons;i++) {
    lastButtonState[i] = buttonState[i];
  }
}

void mouseButtons() {    // Setup Left, Right & Middle Mouse buttons
  if (buttonState[Trigger] != lastButtonState[Trigger]) {
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
}

void mouseCount() {    // Set count down on trigger
  if (buttonState[Trigger] != lastButtonState[Trigger]) {
    if (buttonState[Trigger] == LOW) {
      currentState++;
      delay(50);
    }
    delay(50);
  }
}

void skip() {    // Unpause button
  if (buttonState[Reload] != lastButtonState[Reload]) {
    if (buttonState[Reload] == LOW) {
      currentState = Action;
      delay(50);
    }
    delay(50);
  }
}

void reset() {    // Pause/Re-calibrate button
  if (buttonState[Reload] != lastButtonState[Reload]) {
    if (buttonState[Reload] == LOW) {
      currentState = Paused;
      delay(50);
    }
    delay(50);
  }
}

void PrintResults() {    // Print results for debugging
  Serial.print("RAW: ");
  Serial.print(finalX);
  Serial.print(", ");
  Serial.print(finalY);
  Serial.print("     State: ");
  Serial.print(currentState);
  Serial.print("     Calibration: ");
  Serial.print(xLeft);
  Serial.print(", ");
  Serial.print(yTop);
  Serial.print(", ");
  Serial.print(xRight);
  Serial.print(", ");
  Serial.print(yBottom);
  Serial.print("     Position: ");
  Serial.print(conMoveXAxis);
  Serial.print(", ");
  Serial.println(conMoveYAxis);
}
