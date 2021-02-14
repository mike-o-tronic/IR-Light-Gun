/*
    This is an example Arduino code for DFRobot Positioning Infrared Camera ( SKU: SEN0158 ).
    Basically, it's a camera that can "see" infrared lights and ascertain the coordinates of those lights, just like what's inside a Wiimote.
    It can track up to four infrared lights at a time.
    You can find more info on this sensor at https://www.dfrobot.com/index.php?route=product/product&product_id=1088

    WHAT THIS SKETCH DOES:
    Outputs x and y coordinate values for four points to Serial out.
    When a point is not detected ( blocked, out of range, etc ), the sensor outputs 1023 for both x and y values.

    HOOKUP:
            Arduino     Positioning IR Camera
    ===============     =====================
                 5V --> Red wire
                GND --> Black wire
    SDA (A4 on UNO) --> Yellow wire
    SCL (A5 on UNO) --> Green wire

    MODIFICATION HISTORY:
    // modified for use with SAMCO light guns by Sam Ballantyne Nov 2019.
    // modified for http://DFRobot.com by kurakura, Jan 2015.
    // modified for http://DFRobot.com by Lumi, Jan. 2014
    // modified output for Wii-BlobTrack program by RobotFreak http://www.letsmakerobots.com/user/1433
    // Wii Remote IR sensor  test sample code  by kako http://www.kako.com
*/

#include <Wire.h>
#include <DFRobotIRPosition.h>

DFRobotIRPosition myDFRobotIRPosition;   // declare a IRCam object

int positionX[4];               // RAW Sensor Values
int positionY[4];

int oneY = 0;                   // Re-mapped so left sensor is always read first
int oneX = 0;
int twoY = 0;
int twoX = 0;

// original ribbon direction (pin 1 at top)
int _trigger = 13;               // Label Pin to buttons
int _up = 10;
int _down = 12;
int _left = 11;
int _right = 9;
int _APin = A1;
int _BPin = A0;
int _startPin = A2;
int _select = A3;
int _reload = 7;
int _pedal = 5;

/*
// reversed ribbon direction (pin 1 at bottom)
int _trigger = 7;                // Label Pin to buttons
int _up = 11;
int _down = 9;
int _left = 10;
int _right = 12;
int _APin = A1;
int _BPin = A0;
int _startPin = A2;
int _select = A3;
int _reload = 13;
int _pedal = 5;
*/

int triggerButton = 0;
int upButton = 0;
int downButton = 0;
int leftButton = 0;
int rightButton = 0;
int aButton = 0;
int bButton = 0;
int startButton = 0;
int selectButton = 0;
int reloadButton = 0;

int buttonState1 = 0;            // Set Button states
int lastButtonState1 = 0;
int buttonState2 = 0;
int lastButtonState2 = 0;
int buttonState3 = 0;
int lastButtonState3 = 0;
int buttonState4 = 0;
int lastButtonState4 = 0;
int buttonState5 = 0;
int lastButtonState5 = 0;
int buttonState6 = 0;
int lastButtonState6 = 0;
int buttonState7 = 0;
int lastButtonState7 = 0;
int buttonState8 = 0;
int lastButtonState8 = 0;
int buttonState9 = 0;
int lastButtonState9 = 0;
int buttonState10 = 0;
int lastButtonState10 = 0;


void setup() {
  digitalWrite (5, HIGH);              // This pin is used to power the IR Camera when using Samco PCB
  delay(500);
  myDFRobotIRPosition.begin();         // initialize the object in setup()
  Serial.begin(9600);

  pinMode(_trigger, INPUT_PULLUP);      // Set pin modes
  pinMode(_up, INPUT_PULLUP);
  pinMode(_down, INPUT_PULLUP);
  pinMode(_left, INPUT_PULLUP);
  pinMode(_right, INPUT_PULLUP);       // Set pin modes
  pinMode(_APin, INPUT_PULLUP);
  pinMode(_BPin, INPUT_PULLUP);
  pinMode(_startPin, INPUT_PULLUP);
  pinMode(_select, INPUT_PULLUP);
  pinMode(_reload, INPUT_PULLUP);
}

void loop() {
  getPosition();
  readButtonStates();
  mouseButtons();
  saveButtonStates();
  PrintResults();
}


void readButtonStates() {
  buttonState1 = digitalRead(_trigger);
  buttonState2 = digitalRead(_up);
  buttonState3 = digitalRead(_down);
  buttonState4 = digitalRead(_left);
  buttonState5 = digitalRead(_right);
  buttonState6 = digitalRead(_APin);
  buttonState7 = digitalRead(_BPin);
  buttonState8 = digitalRead(_startPin);
  buttonState9 = digitalRead(_select);
  buttonState10 = digitalRead(_reload);
}

void saveButtonStates() {
  lastButtonState1 = buttonState1;
  lastButtonState2 = buttonState2;
  lastButtonState3 = buttonState3;
  lastButtonState4 = buttonState4;
  lastButtonState5 = buttonState5;
  lastButtonState6 = buttonState6;
  lastButtonState7 = buttonState7;
  lastButtonState8 = buttonState8;
  lastButtonState9 = buttonState9;
  lastButtonState10 = buttonState10;
}

void mouseButtons() {    // Setup Cali, Left, Right & Middle Mouse buttons
  if (buttonState1 != lastButtonState1) {
    if (buttonState1 == LOW) {
      triggerButton = 0;
    } else {
      triggerButton = 255;
    }
    delay(10);
  }
  if (buttonState2 != lastButtonState2) {
    if (buttonState2 == LOW) {
      upButton = 0;
    } else {
      upButton = 255;
    }
    delay(10);
  }
  if (buttonState3 != lastButtonState3) {
    if (buttonState3 == LOW) {
      downButton = 0;
    } else {
      downButton = 255;
    }
    delay(10);
  }
  if (buttonState4 != lastButtonState4) {
    if (buttonState4 == LOW) {
      leftButton = 0;
    } else {
      leftButton = 255;
    }
    delay(10);
  }
  if (buttonState5 != lastButtonState5) {
    if (buttonState5 == LOW) {
      rightButton = 0;
    } else {
      rightButton = 255;
    }
    delay(10);
  }
  if (buttonState6 != lastButtonState6) {
    if (buttonState6 == LOW) {
      aButton = 0;
    } else {
      aButton = 255;
    }
    delay(10);
  }
  if (buttonState7 != lastButtonState7) {
    if (buttonState7 == LOW) {
      bButton = 0;
    } else {
      bButton = 255;
    }
    delay(10);
  }
  if (buttonState8 != lastButtonState8) {
    if (buttonState8 == LOW) {
      startButton = 0;
    } else {
      startButton = 255;
    }
    delay(10);
  }
  if (buttonState9 != lastButtonState9) {
    if (buttonState9 == LOW) {
      selectButton = 0;
    } else {
      selectButton = 255;
    }
    delay(10);
  }
  if (buttonState10 != lastButtonState10) {
    if (buttonState10 == LOW) {
      reloadButton = 0;
    } else {
      reloadButton = 255;
    }
    delay(10);
  }
}

void PrintResults() {    // Print results for debugging
  Serial.print( oneX );
  Serial.print( "," );
  Serial.print( oneY );
  Serial.print( "," );
  Serial.print( twoX );
  Serial.print( "," );
  Serial.print( twoY );
  Serial.print( "," );
  Serial.print( positionX[2] );
  Serial.print( "," );
  Serial.print( positionY[2] );
  Serial.print( "," );
  Serial.print( positionX[3] );
  Serial.print( "," );
  Serial.print( positionY[3] );
  Serial.print( "," );
  Serial.print( triggerButton );
  Serial.print( "," );
  Serial.print( upButton );
  Serial.print( "," );
  Serial.print( downButton );
  Serial.print( "," );
  Serial.print( leftButton );
  Serial.print( "," );
  Serial.print( rightButton );
  Serial.print( "," );
  Serial.print( aButton );
  Serial.print( "," );
  Serial.print( bButton );
  Serial.print( "," );
  Serial.print( startButton );
  Serial.print( "," );
  Serial.print( selectButton );
  Serial.print( "," );
  Serial.print( reloadButton );
  Serial.println();
  delay( 20 );
}

void getPosition() {    // Get tilt adjusted position from IR postioning camera
  myDFRobotIRPosition.requestPosition();
  if (myDFRobotIRPosition.available()) {
    for (int i = 0; i < 4; i++) {
      positionX[i] = myDFRobotIRPosition.readX(i);
      positionY[i] = map (myDFRobotIRPosition.readY(i), 0, 768, 768, 0 );
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
  }
}
