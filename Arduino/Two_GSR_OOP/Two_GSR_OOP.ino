#include <ArduinoBLE.h>

// we have buffer of size 4: 2 for GSR, and another 2 because each GSR will have its own button value
#define NUMBER_OF_DATA 4
#define UUID_service "ac825cd3-3274-42b7-8c31-8171eac33b9a"
#define UUID_characteristic "77bc0536-aac5-11ec-b909-0242ac120002"
#define LENGTH_OF_BUFFER 9
#define MEDIAN_POS (LENGTH_OF_BUFFER - 1)/2
#define GSR1 A0
#define GSR2 A7

class Data {
    short values[NUMBER_OF_DATA];
    uint8_t bytes[ NUMBER_OF_DATA * sizeof( short ) ];
}

class Buffer {
    short values[LENGTH_OF_BUFFER];
public:
    Buffer() {
        // filling Data initially with 0
        for ( int i = 0; i < NUMBER_OF_DATA; i++ ) {
            values[i] = 0;
        }
    }
    void quicksort(){
        short i, j, pivot, temp;
        short first = 0;
        short last = LENGTH_OF_BUFFER - 1;
    
        if(first < last){
            pivot = first;
            i = first;
            j = last;
            while(i < j){
                while(values[i] <= values[pivot] && (i < last)) {
                    i++;
                }
                while(values[j] > values[pivot]) {
                    j--;
                } 
                if(i < j){
                    temp = values[i];
                    values[i] = values[j];
                    values[j] = temp;
                }
            }
            temp = values[pivot];
            values[pivot] = values[j];
            values[j] = temp;
            quicksort(values, first, j-1);
            quicksort(values, j+1, last);
        }
    }

    void add(short newVal) {
        for (int i = 0, j = 1; i < LENGTH_OF_BUFFER - 1; i++, j++) {
            values[i] = values[j];
        }
        values[LENGTH_OF_BUFFER - 1] = newVal;
    }

    Data operator=(Data values_toBe_copied){
        for (int i = 0; i < LENGTH_OF_BUFFER; i++) {
            values[i] = values_toBe_copied[i];
        }
    }

    void print_buffer() {
        Serial.print("[");
        for (int i = 0; i < LENGTH_OF_BUFFER - 1; i++) {
            Serial.print(values[i]);
            Serial.print(",");
        }
        Serial.print(values[LENGTH_OF_BUFFER - 1]);
        Serial.println("]");
        delay(2000);
    }

    short median() {
        Buffer temp;
        temp = this;
        temp.quicksort();
        return values[MEDIAN_POS];
    }
}

Buffer GSR_buffer1;
Buffer GSR_buffer2;


BLEService gsrService(UUID_service);
BLECharacteristic gsrChar(UUID_characteristic, BLERead | BLENotify | BLEWrite, sizeof Data.bytes);


void setup() {

  
    Serial.begin(9600);
    while (!Serial);

    pinMode(LED_BUILTIN, OUTPUT);


    if (!BLE.begin()) {
        Serial.println("starting BLE failed!");
        while (1);
    }

    // filling buffer first
    for (int i = 0; i < LENGTH_OF_BUFFER; i++) {
        short gsrVal1 = analogRead(GSR1);
        short gsrVal2 = analogRead(GSR2);
        GSR_buffer1.add(gsrVal1);
        GSR_buffer2.add(gsrVal2);
        delay(100);
    }
 
  
    BLE.setLocalName("GsrMonitor");
    BLE.setAdvertisedService(gsrService);
    gsrService.addCharacteristic(gsrChar);
    BLE.addService(gsrService);
    BLE.advertise();
    Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
    BLEDevice central = BLE.central();
  
    if (central) {
        Serial.print("Connected to central: ");
        Serial.println(central.address());
        // turn on the LED to indicate the connection:
        digitalWrite(LED_BUILTIN, HIGH);
  
        while (central.connected()) {
            short sensorValue1 = 0;
            short sensorValue2 = 0;

            sensorValue1 = analogRead(GSR1);
            sensorValue2 = analogRead(GSR2);

            // initial value of the button
            Data.values[2] = 0;
            Data.values[3] = 0;

            GSR_buffer1.add(sensorValue1);
            GSR_buffer2.add(sensorValue2);
            
            Data.values[0] = GSR_buffer1.median();
            Data.values[1] = GSR_buffer2.median();
        
            if (gsrChar.written()) {
                if (gsrChar.value()) {
                    Data.values[2] = Data.values[0];
                    Data.values[3] = Data.values[1];
                }
            }

            Serial.print("GSR value: No.1: ");
            Serial.print(Data.values[0]);

            Serial.print(" | No.2: ");
            Serial.print(Data.values[1]);

            Serial.print(" | button 1: ");
            Serial.print(Data.values[2]);

            Serial.print(" | button 2: ");
            Serial.println(Data.values[3]);

            gsrChar.writeValue(Data.values, sizeof Data.bytes );

            
            
            delay(100);
        }
    
        // when the central disconnects, turn off the LED:
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print("Disconnected from central: ");
        Serial.println(central.address());
    }
}
