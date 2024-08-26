/*
  LED Control example from Arduino

  To find original example for to: File -> Examples -> (Scroll down to examples from custom libraries) -> ArduinoBLE -> Central -> LEDControl
  Note: I deleted some intro notes that were not relavent to me.

  This example scans for Bluetooth® Low Energy peripherals until one with the advertised service
  "19b10000-e8f2-537e-4f6c-d104768a1214" UUID is found. Once discovered and connected,
  it will remotely control the Bluetooth® Low Energy peripheral's LED, when the button is pressed or released.


  You can use it with another board that is compatible with this library and the
  Peripherals -> LED example.

  Make sure you have ArduinoBLE library installed.
*/

#include <ArduinoBLE.h>


int count = 0; 


void setup() {
  Serial.begin(9600);
  while (!Serial);

  // configure the button pin as input
  pinMode(buttonPin, INPUT);

  // initialize the Bluetooth® Low Energy hardware
  BLE.begin();

  Serial.println("Bluetooth® Low Energy Central - LED control");

  // start scanning for peripherals
  BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();
  

  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

    if (peripheral.localName() != "LED") {
      return;
    }

    // stop scanning
    BLE.stopScan();

    controlLed(peripheral);

    // peripheral disconnected, start scanning again
    BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
  }
}



void controlLed(BLEDevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed! (please no dc)");
    //peripheral.disconnect(); //In the example code from Arduino this line is enabled. I disabled it for me because my bluetooth connection drops randomly regardless of code and this 'disconnect' line just made it worse for me.
    return;
  }

  // retrieve the LED characteristic
  BLECharacteristic ledCharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");

  if (!ledCharacteristic) {
    Serial.println("Peripheral does not have LED characteristic!");
    peripheral.disconnect();
    return;
  } else if (!ledCharacteristic.canWrite()) {
    Serial.println("Peripheral does not have a writable LED characteristic!");
    peripheral.disconnect();
    return;
  }

  while (peripheral.connected()) { //My biggest roadblock was the .writeValue funtion because it only takes 'byte' data and only when input a certain way. 
    //ledCharacteristic.writeValue((byte)0x01); //This is how the original code input 'byte' data. I tried tons of variations to get an updating value to work and the best method so far is 'byte x = ###' and then putting 'x' in the .writeValue function. See line 107 and 109 for reference.
    //byte x = 6; //Test line for sending a static number to .writeValue
    countfunc (); //This function needs to be in the 'while' loop. If placed elsewhere, the 'x' value does not get access to the updated 'count'. (At least not for me).
    byte x = count;
    ledCharacteristic.writeValue(x);
    
    Serial.print("Current count = "); //Test line for seeing 'count'
    Serial.println(count); //Test line for seeing 'count'
    //Serial.print("'byte x' = "); //Test line for seeing 'x'. But you only see a normal number rather than 0x0# like the original code used for .writeValue. I assume it is because 'byte' in this cases is just a data type. I read online somewhere that inputting '(byte)0x0#' into .writeValue is to FORCE Arduino into recognizing the input as byte data type. Either way, this serial print is here just for see what stuff looks like.
    //Serial.println(x);  //Test line for seeing 'x'

  }

  Serial.println("Peripheral disconnected for some reason"); //for some reason my bluetooth connection drops randomly. Fortunately this code is set up for the Arduino to keep trying to reconnect..
}

int countfunc() {
  count = count + 1;
  delay(1000);
  //Serial.println(count); //test line for making sure function is counting properly
  return count;
}
