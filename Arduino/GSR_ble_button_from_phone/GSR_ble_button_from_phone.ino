#include <ArduinoBLE.h>

#define NUMBER_OF_DATA 2
#define UUID_service "ac825cd3-3274-42b7-8c31-8171eac33b9a"
#define UUID_characteristic "77bc0536-aac5-11ec-b909-0242ac120002"
#define LENGTH_OF_BUFFER 9
#define MEDIAN_POS (LENGTH_OF_BUFFER - 1)/2

union data
{
    struct __attribute__( ( packed ) )
    {
      short values[NUMBER_OF_DATA];
    };
    uint8_t bytes[ NUMBER_OF_DATA * sizeof( short ) ];
};
union data Data;


short buffer_beforeMedian[LENGTH_OF_BUFFER];
short buffer_afterSort[LENGTH_OF_BUFFER];


BLEService gsrService(UUID_service);
BLECharacteristic gsrChar(UUID_characteristic, BLERead | BLENotify | BLEWrite, sizeof Data.bytes);


void setup() {

    short gsrVal = 0;

  
    Serial.begin(9600);
    while (!Serial);

    pinMode(LED_BUILTIN, OUTPUT);

    if (!BLE.begin()) {
        Serial.println("starting BLE failed!");
        while (1);
    }

    // filling Data initially with 0
    for ( int i = 0; i < NUMBER_OF_DATA; i++ ) {
        Data.values[i] = 0;
    }

    // filling buffer first
    for (int i = 0; i < LENGTH_OF_BUFFER; i++) {
        gsrVal = analogRead(A0);
        buffer_beforeMedian[i] = gsrVal;
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
            short sensorValue = 0;

            sensorValue = analogRead(A0);
        
            Data.values[1] = 0;

            shift_Buffer(buffer_beforeMedian, sensorValue);
            //Serial.print("Buffer: ");
            //print_buffer(buffer_beforeMedian);
            
            copy_Buffer(buffer_beforeMedian, buffer_afterSort);
            quicksort(buffer_afterSort, 0, LENGTH_OF_BUFFER - 1);
            //Serial.print("Sorted Buffer: ");
            //print_buffer(buffer_afterSort);
            
            Data.values[0] = buffer_afterSort[MEDIAN_POS];
            Serial.print("GSR is now: ");
            Serial.println(Data.values[0]);
        
            if (gsrChar.written()) {
                if (gsrChar.value()) {
                    Serial.println ("Button Clicked: ");
                    Data.values[1] = Data.values[0];
                }
            }
            gsrChar.writeValue(Data.values, sizeof Data.bytes );

            
            
            delay(100);
        }
    
        // when the central disconnects, turn off the LED:
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print("Disconnected from central: ");
        Serial.println(central.address());
    }
}
void quicksort(short* number,short first,short last){
    short i, j, pivot, temp;
    if(first < last){
        pivot = first;
        i = first;
        j = last;
        while(i < j){
            while(number[i] <= number[pivot] && (i < last)) {
                i++;
            }
            while(number[j] > number[pivot]) {
                j--;
            }
            if(i < j){
                temp = number[i];
                number[i] = number[j];
                number[j] = temp;
            }
        }
        temp = number[pivot];
        number[pivot] = number[j];
        number[j] = temp;
        quicksort(number, first, j-1);
        quicksort(number, j+1, last);
    }
}

void shift_Buffer(short* arr, short newVal) {
    for (int i = 0, j = 1; i < LENGTH_OF_BUFFER - 1; i++, j++) {
        arr[i] = arr[j];
    }
    arr[LENGTH_OF_BUFFER - 1] = newVal;
}

void copy_Buffer(short* arr1, short* arr2) {
    for (int i = 0; i < LENGTH_OF_BUFFER; i++) {
        arr2[i] = arr1[i];
    }
}

void print_buffer(short* buff) {
    Serial.print("[");
    for (int i = 0; i < LENGTH_OF_BUFFER - 1; i++) {
        Serial.print(buff[i]);
        Serial.print(",");
    }
    Serial.print(buff[LENGTH_OF_BUFFER - 1]);
    Serial.println("]");
    delay(2000);
}
