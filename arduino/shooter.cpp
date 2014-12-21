#include <Arduino.h>
#include <DistanceSRF04.h>
#include <Servo.h>
#include <SPI.h>
#include <String.h>

// http://forum.pjrc.com/threads/26804-connecting-nRF8001-to-Teensy-3-1
#include "Adafruit_BLE_UART.h"

// Servos
#define YAW_SERVO   9
#define PITCH_SERVO 10
#define HOME_YAW    4
#define HOME_PITCH   90

// Outputs
#define LED     13
#define SHOOTER 14
#define BTN     15

// Ultrasonic
#define TRIGGER 17
#define ECHO    21

// BLE SPI
#define ADAFRUITBLE_REQ 4
#define ADAFRUITBLE_RDY 3
#define ADAFRUITBLE_RST 2

// Protocol
#define PROTOCOL_SET        's'
#define PROTOCOL_GET        'g'
#define PROTOCOL_FIRE       'f'
#define PROTOCOL_ULTRASONIC 'u'
#define PROTOCOL_PITCH      'v' // pitch is up and down
#define PROTOCOL_YAW        'h' // yaw is left and right
#define PROTOCOL_GO_HOME    'c' // c = center

// Math Stuff
#define DISTANCE_THRESHOLD  50
#define SHOOTER_DURATION    50
#define MAX_UART_BUFFER     20

Adafruit_BLE_UART uart = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);
DistanceSRF04 ultrasonic;
Servo yaw;
Servo pitch;

bool start;
int lastDistance;

// Function prototypes
void send_ultrasonic_data();
void shoot();
void servo_home();

// ----------------------------------------------------------------------------
// BLE Callbacks
// ----------------------------------------------------------------------------

void aciCallback(aci_evt_opcode_t event) {
  switch(event) {

    case ACI_EVT_DEVICE_STARTED:
      Serial.println(F("Advertising started"));
      break;

    case ACI_EVT_CONNECTED:
      Serial.println(F("Connected!"));
      // Send the servo home
      servo_home();
      break;

    case ACI_EVT_DISCONNECTED:
      Serial.println(F("Disconnected or advertising timed out"));
      break;

    default:
      break;
  }
}

// ----------------------------------------------------------------------------

void rxCallback(uint8_t *buffer, uint8_t len) {
  // String str = String(buffer);

  if (buffer[0] == PROTOCOL_GET) { // g = 'get'

    if (buffer[1] == PROTOCOL_PITCH) { // v = 'get vertical'
      // Unimplemented
    } else if (buffer[1] == PROTOCOL_YAW) { // h = 'get horizontal'
      // Unimplemented
    } else if (buffer[1] == PROTOCOL_ULTRASONIC) { // u = 'ultrasonic'
      send_ultrasonic_data();
    }

  } else if (buffer[0] == PROTOCOL_SET) { // s = 'set'
    float position;

    // Get position float value
    sscanf((char *)buffer, "%*2c%f", &position);


    if (buffer[1] == PROTOCOL_PITCH) { // v = 'set vertical'
      pitch.write(170.0 - (position * 170.0));
    } else if (buffer[1] == PROTOCOL_YAW) { // h = 'set horizontal'
      yaw.write(position * 90.0);
    } else if (buffer[1] == PROTOCOL_GO_HOME) {
      servo_home();
    }

  } else if (buffer[0] == PROTOCOL_FIRE) { // f = 'fire'
    shoot();
  }


  for(uint8_t i=0; i<len; i++)
   Serial.print((char)buffer[i]); 
}

// ----------------------------------------------------------------------------

void setup() {
  yaw.attach(YAW_SERVO);
  pitch.attach(PITCH_SERVO);
  
  // pinMode(LED, OUTPUT);
  pinMode(SHOOTER, OUTPUT);
  pinMode(BTN, INPUT);
  
  /* INIT */
  SPI.begin();
  
  // start = false;
  servo_home();
  digitalWrite(LED, HIGH);
  ultrasonic.begin(ECHO, TRIGGER);

  // Setup the BLE_UART
  uart.setRXcallback(rxCallback);
  uart.setACIcallback(aciCallback);
  uart.setDeviceName("shooter"); /* 7 characters max! */
  uart.begin();
  
 
  // For debugging 
  Serial.begin(9600);
}

// ----------------------------------------------------------------------------

void loop() {
  // Tell the BLE to poll
  uart.pollACI();
}

// ----------------------------------------------------------------------------
// Helper Functions
// ----------------------------------------------------------------------------

void send_ultrasonic_data() {
  int distance = ultrasonic.getDistanceCentimeter();

  // build command
  String str = "";
  str.append(PROTOCOL_GET);
  str.append(PROTOCOL_ULTRASONIC);

  str.append(distance);

  uint8_t sendBuffer[MAX_UART_BUFFER];
  str.getBytes(sendBuffer, MAX_UART_BUFFER);

  char sendBufferSize = min(MAX_UART_BUFFER, str.length());
  uart.write(sendBuffer, sendBufferSize);
}

// ----------------------------------------------------------------------------

void shoot() {
  digitalWrite(SHOOTER, HIGH);
  delay(SHOOTER_DURATION);
  digitalWrite(SHOOTER, LOW);
}

// ----------------------------------------------------------------------------

void servo_home() {
  yaw.write(HOME_YAW);
  pitch.write(HOME_PITCH);
}