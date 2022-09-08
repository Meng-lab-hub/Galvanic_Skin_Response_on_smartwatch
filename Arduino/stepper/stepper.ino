#include <ArduinoBLE.h>

BLEService stepperService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, writable by central
BLEIntCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLEWrite);


const int ledPin = LED_BUILTIN; // pin to use for the LED
int delayStepper = 100; 
void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Stepper motor control");
  BLE.setAdvertisedService(stepperService);

  // add the characteristic to the service
  stepperService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(stepperService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(100);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();
  Serial.println("No central found");
  
  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:

      if (switchCharacteristic.written()) {
        delayStepper = switchCharacteristic.value();
        Serial.print("New value is: ");
        Serial.println(delayStepper, HEX);

      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
