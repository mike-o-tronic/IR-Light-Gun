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

enum coordinate {
  X,
  Y
};

int rawPosition[4][2];                   // Four raw positions from sensor

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

  for (int i=0; i<buttons; i++) {
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
  delay(50);
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

void PrintResults() {    // Print results for debugging
  for (int i=0; i<4; i++) {
    Serial.print( rawPosition[i][X] );
    Serial.print( "," );
    Serial.print( rawPosition[i][Y] );
    Serial.print( "," );
  }
  for (int i=0; i<buttons; i++) {
    Serial.print( buttonState[i] );
    Serial.print( "," );
  }
  Serial.println();
}

void getPosition() {    // Get tilt adjusted position from IR postioning camera
  myDFRobotIRPosition.requestPosition();
  if (myDFRobotIRPosition.available()) {
    for (int i=0; i<4; i++) {
      rawPosition[i][X] = myDFRobotIRPosition.readX(i);
      rawPosition[i][Y] = 768 - myDFRobotIRPosition.readY(i);
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
  }
}
