#include <Servo.h>
#include <CD74HC4067.h>

const int initialPositionVal = 0;
Servo Servohori;                  // servo for horizontal movement (BOTTOM SERVO)
int servoh = initialPositionVal;  // initial horizontal position
int servohLimitHigh = 175;        // maximum horizontal angle
int servohLimitLow = 5;           // minimum horizontal angle

Servo Servoverti;                 // servo for vertical movement
int servov = initialPositionVal;  // initial vertical position
int servovLimitHigh = 175;        // maximum vertical angle
int servovLimitLow = 5;           // minimum vertical angle

int tol = 30;        //Tolerance
int SunsetVal = 50;  // the value to go back to zero
// Pins for the CD74HC4067 multiplexer control
const int MUX_S0 = 3;
const int MUX_S1 = 4;
const int MUX_S2 = 5;
const int MUX_S3 = 6;
const int MUX_SIG = A0;  // Signal pin connected to the multiplexer

CD74HC4067 mux(MUX_S0, MUX_S1, MUX_S2, MUX_S3);

void setup() {
  Serial.begin(9600);                    // initialize serial communication
  Servohori.attach(9);                   // attach horizontal servo to pin 9
  Servohori.write(initialPositionVal);   // set initial horizontal position
  Servoverti.attach(10);                 // attach vertical servo to pin 10
  Servoverti.write(initialPositionVal);  // set initial vertical position
  delay(500);

  pinMode(MUX_S0, OUTPUT);  // set multiplexer control pins as output
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);
}

void loop() {
  servoh = Servohori.read();
  servov = Servoverti.read();
  Serial.print("SH=");
  Serial.println(servoh);
  Serial.print("SV=");
  Serial.println(servov);
  // Arrays to store sensor readings
  int sensorValues[12];
  int sensorSum[4] = { 0 };  // Initialize sums for each quadrant

  // Read sensor values from multiplexer channels
  for (int i = 0; i < 12; i++) {
    // Set the address of the multiplexer channel
    // digitalWrite(MUX_S0, bitRead(i, 0));
    // digitalWrite(MUX_S1, bitRead(i, 1));
    // digitalWrite(MUX_S2, bitRead(i, 2));
    // digitalWrite(MUX_S3, bitRead(i, 3));
    // Read the analog value from the selected channel
    mux.channel(i);
    int value = analogRead(MUX_SIG);
    Serial.print("Chanal ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(value);
    // Store the sensor reading
    sensorValues[i] = value;

    // Accumulate sensor values for each quadrant
    if (i < 3) {
      sensorSum[0] += value;  // Lef
    } else if (i < 6) {
      sensorSum[1] += value;  // Right
    } else if (i < 9) {
      sensorSum[2] += value;  //  Top
    } else {
      sensorSum[3] += value;  // Bottom
    }
  }
  Serial.print("Sum");
  Serial.println(sensorSum[0]);
  Serial.println(sensorSum[1]);
  Serial.println(sensorSum[2]);
  Serial.println(sensorSum[3]);
  // Calculate average value for each quadrant
  int avrH1 = sensorSum[0] / 3.0;  // Left
  int avrH2 = sensorSum[1] / 3.0;  // Right
  int avrV1 = sensorSum[2] / 3.0;  // Top
  int avrV2 = sensorSum[3] / 3.0;  // Bottom

  if (avrH1 - avrH2 > tol) {
    servoh++;
    if (servoh > servohLimitHigh)
      servoh = servohLimitHigh;
    Servoverti.write(servoh);
  } else if (avrH2 - avrH1 > tol) {
    servoh--;
    if (servoh < servohLimitLow)
      servoh = servohLimitLow;
    Servoverti.write(servoh);
  }

  delay(10);

  if (avrV1 - avrV2 > tol) {
    servov++;
    if (servov > servovLimitHigh)
      servov = servovLimitHigh;
    Servoverti.write(servov);
  } else if (avrV2 - avrV1 > tol) {
    servov--;
    if (servov < servovLimitLow)
      servov = servovLimitLow;
    Servoverti.write(servov);
  }

  delay(10);

  if (avrH1 < SunsetVal && avrH2 < SunsetVal && avrV1 < SunsetVal && avrV2 < SunsetVal) {  //Back to initial position
    servoh = initialPositionVal;
    servov = initialPositionVal;
    Servoverti.write(servov);
    Servohori.write(servoh);
  }

  // Print sensor values and servo positions

  Serial.println("Servo Positions->");
  Serial.print("Horizontal: ");
  Serial.println(servoh);
  Serial.print("Vertical: ");
  Serial.println(servov);

  // Your servo control logic here...

  delay(1000);
}