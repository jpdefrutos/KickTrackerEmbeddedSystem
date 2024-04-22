#include <Arduino.h>
#include "libKickSensor.h"
#ifdef ARDUINO_NANO_ESP32
	#include <vector>
	#include <string>
	#include <iostream>
#else
	#include <ArduinoSTL.h>
#endif

////////////DEBUG////////////
// #include "avr8-stub.h"
////////////DEBUG////////////

//AccelSensor sensor();

//Logger* logger = GlobalLogger->getLogger();
AccelSensor sensor(0x68, 8);
time_t* currTime;
std::vector<int32_t> *lastData;

Communicator comms_manager(WIFI_SSID, "", 80);

const int BAUDRATE = 9600;

void setup()
{
    ////////////DEBUG////////////
    // debug_init();
    ////////////DEBUG////////////
    Serial.begin(BAUDRATE);
    sensor.powerUpSensor(0x6B, 0);
    while(!sensor.isReady() and !Serial)
    {  
    }
    std::cout << "Sensor active and communication port open!" << std::endl;
    std::cout << "Running SETUP on core " << xPortGetCoreID() << std::endl;
}

void loop()
{
    std::cout << "Running LOOP on core " << xPortGetCoreID() << std::endl;
    sensor.continuousReading(5000, 1);
    lastData = sensor.getLastData();
    std::cout << "Last Data ";
    for(size_t i=0; lastData->size(); i++)
    {
        std::cout << lastData->at(i) << " ";
    }
    std::cout << std::endl;
    // delay(2000);
    std::cout << "Here comes!" << std::endl;
    //sensor.continuousReading(5000, 10);

}