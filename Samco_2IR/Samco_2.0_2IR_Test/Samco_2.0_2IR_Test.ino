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


void setup() {
  Serial.begin(9600);

  for (int i=0;i<buttons;i++) {
    pinMode(pin[i], INPUT_PULLUP);         // Set pin modes
    lastButtonState[i] = 0;                // Initialized last button state
  }

  myDFRobotIRPosition.begin();         // initialize the object in setup()
}

void loop() {
  getPosition();
  readButtonStates();
  saveButtonStates();
  PrintResults();
  delay(10);
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
  Serial.print( buttonState[Trigger] );
  Serial.print( "," );
  Serial.print( buttonState[Up] );
  Serial.print( "," );
  Serial.print( buttonState[Down] );
  Serial.print( "," );
  Serial.print( buttonState[Left] );
  Serial.print( "," );
  Serial.print( buttonState[Right] );
  Serial.print( "," );
  Serial.print( buttonState[A] );
  Serial.print( "," );
  Serial.print( buttonState[B] );
  Serial.print( "," );
  Serial.print( buttonState[Start] );
  Serial.print( "," );
  Serial.print( buttonState[Select] );
  Serial.print( "," );
  Serial.print( buttonState[Reload] );
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
