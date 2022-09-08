#include <ArduinoBLE.h>
BLEService GSRService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEUnsignedShortCharacteristic GSRValue("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEUnsignedShortCharacteristic buttonStatus("b3468d88-b038-11ec-b909-0242ac120002", BLERead | BLENotify);

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

BLE.setLocalName("GSRSense");
BLE.setAdvertisedService(GSRService);
GSRService.addCharacteristic(GSRValue);
GSRService.addCharacteristic(buttonStatus);
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

      Serial.print("GSR value: ");
      Serial.println(GSR, HEX);
      GSRValue.writeValue(GSR);

      //if the button has been pressed, lets toggle the LED from "off to on" or "on to off"
      if ( (millis() - lastDebounceTime) > debounceDelay) {

        if (buttonState == LOW) {
          lastDebounceTime = millis(); //set the current time
          buttonStatus.writeValue(buttonState);
          Serial.println(buttonStatus);
    }
  }//close if(time buffer)
      
      delay(150);
  }
}
digitalWrite(LED_BUILTIN, LOW);
Serial.print("Disconnected from central: ");
Serial.println(central.address());
}
