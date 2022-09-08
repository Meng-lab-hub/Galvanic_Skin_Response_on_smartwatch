#include <ArduinoBLE.h>
BLEService GSRService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEUnsignedShortCharacteristic GSRValue("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

void setup() {
Serial.begin(9600);
while (!Serial);

pinMode(LED_BUILTIN, OUTPUT);
if (!BLE.begin()) 
{
Serial.println("starting BLE failed!");
while (1);
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
      Serial.print("GSR value: ");
      Serial.println(GSR);
      GSRValue.writeValue(GSR);
      delay(200);
  }
}
digitalWrite(LED_BUILTIN, LOW);
Serial.print("Disconnected from central: ");
Serial.println(central.address());
}
