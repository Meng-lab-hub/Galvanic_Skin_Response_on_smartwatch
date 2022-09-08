/*
  Battery Monitor

  This example creates a BLE peripheral with the standard battery service and
  level characteristic. The A0 pin is used to calculate the battery level.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  You can use a generic BLE central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

#define NUMBER_OF_DATA 2
#define UUID_button "f311f30d-961c-4be6-afb6-8284794c9ce7"
union data
{
  struct __attribute__( ( packed ) )
  {
    short values[NUMBER_OF_DATA];
  };
  uint8_t bytes[ NUMBER_OF_DATA * sizeof( short ) ];
};

union data Data;

const int buttonPin = 4;
int buttonState;             // the current reading from the input pin
int lastButtonState = 0;   // the previous reading from the input pin
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 2000;    // the debounce time; increase if the output flickers

int oldgsrLevel = 0;  // last battery level reading from analog input
long previousMillis = 0;  // last time the battery level was checked, in ms

 // BLE gsr Service
BLEService gsrService("ac825cd3-3274-42b7-8c31-8171eac33b9a");

// BLE gsr Characteristic
BLECharacteristic gsrChar("77bc0536-aac5-11ec-b909-0242ac120002",  // standard 16-bit characteristic UUID
    BLERead | BLENotify, sizeof Data.bytes); // remote clients will be able to get notifications if this characteristic changes
//BLEUnsignedShortCharacteristic buttonCharacteristic("640144ee-b037-11ec-b909-0242ac120002", BLERead | BLENotify);
BLEShortCharacteristic button(UUID_button, BLEWrite);



void setup() {
  Serial.begin(9600);    // initialize serial communication
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected
  pinMode(buttonPin, INPUT); // use button pin as an input

  
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }
  for ( int i = 0; i < NUMBER_OF_DATA; i++ )
{
  Data.values[i] = 0;
}

  /* Set a local name for the BLE device
     This name will appear in advertising packets
     and can be used by remote devices to identify this BLE device
     The name can be changed but maybe be truncated based on space left in advertisement packet
  */
  BLE.setLocalName("GsrMonitor");
  BLE.setAdvertisedService(gsrService); // add the service UUID
  gsrService.addCharacteristic(gsrChar); // add the battery level characteristic
  gsrService.addCharacteristic(button);
 // gsrService.addCharacteristic(buttonCharacteristic);
  BLE.addService(gsrService); // Add the battery service
  //gsrChar.writeValue(oldgsrLevel); // set initial value for this characteristic
  //buttonCharacteristic.writeValue(0);
  /* Start advertising BLE.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // wait for a BLE central
  BLEDevice central = BLE.central();
  
  // if a central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);
    //button
  
    while (central.connected()) {

      // check the battery level every 200ms
      // while the central is connected:
      //long currentMillis = millis();
      // if 200ms have passed, check the battery level:
      //if (currentMillis - previousMillis >= 200) {
      //buttonCharacteristic.writeValue(buttonState);
      // previousMillis = currentMillis;
      int sensorValue = analogRead(A0);

      //int gsrLevel = map(sensorValue, 0, 1023, 0, 1023);

      if (sensorValue != oldgsrLevel) {      // if the battery level has changed
        Serial.print("GSR is now: "); // print it
        Serial.println(sensorValue);
        //gsrChar.writeValue(sensorValue);  // and update the battery level characteristic
        oldgsrLevel = sensorValue;           // save the level for next comparison
      }
      Data.values[1] = 0;
      Data.values[0] = sensorValue;
      //button
      // read the state of the switch into a local variable:
      int reading = button.value();

      

      if (reading != lastButtonState) {
        Data.values[1] = sensorValue;
        Serial.println("<----Button is clicked ");
      }

      lastButtonState = reading;
      
      gsrChar.writeValue(Data.values, sizeof Data.bytes );
      delay(50);

  /*
  // has the value changed since the last read
    if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      // only toggle the LED if the new button state is HIGH
      if (buttonState == LOW) {
        //buttonCharacteristic.writeValue(buttonState);
        lastDebounceTime = millis();
        Data.values[1] = sensorValue;
        Serial.println("<----Button is clicked "); // print it
      }
    }
    }
    */

    //gsrChar.writeValue(sensorValue&buttonState);
    //gsrChar.writeValue(buttonState);

    
    
      }
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
