#include <ArduinoBLE.h>

#define NUMBER_OF_DATA 2

union data
{
  struct __attribute__( ( packed ) )
  {
    short values[NUMBER_OF_DATA];
  };
  uint8_t bytes[ NUMBER_OF_DATA * sizeof( short ) ];
};

union data Data;


BLEService GSRService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic GSRValue("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, sizeof Data.bytes);


const int buttonPin = 4;
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 2000;    // the debounce time; increase if the output flickers
int buttonState = HIGH;

void setup() {
Serial.begin(9600);
while (!Serial);

pinMode(LED_BUILTIN, OUTPUT);
pinMode(buttonPin, INPUT);
  
if (!BLE.begin()) 
{
Serial.println("starting BLE failed!");
while (1);
}

for ( int i = 0; i < NUMBER_OF_DATA; i++ )
{
  Data.values[i] = 0;
}

BLE.setLocalName("GSRSense");
BLE.setAdvertisedService(GSRService);
GSRService.addCharacteristic(GSRValue);
BLE.addService(GSRService);

BLE.advertise();
Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() 
{
BLEDevice central = BLE.central();

if (central) 
{
  Serial.print("Connected to central: ");
  Serial.println(central.address());
  digitalWrite(LED_BUILTIN, HIGH);

  while (central.connected()) {

      int GSR = analogRead(A0);
      buttonState = digitalRead(buttonPin);
      Data.values[1] = 0;
      Serial.print("GSR value: ");
      Serial.println(GSR, HEX);
      Data.values[0] = GSR;

      //if the button has been pressed, lets toggle the LED from "off to on" or "on to off"
      if ( (millis() - lastDebounceTime) > debounceDelay) {

        if (buttonState == LOW) {
          lastDebounceTime = millis(); //set the current time
          Data.values[1] = GSR;
          Serial.println("button is clicked");
        }
      }
      GSRValue.writeValue(Data.values, sizeof Data.bytes );
      delay(150);
  }
}
digitalWrite(LED_BUILTIN, LOW);
Serial.print("Disconnected from central: ");
Serial.println(central.address());
}
