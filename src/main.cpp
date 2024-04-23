#include "libKickSensor.h"
#ifdef ARDUINO_NANO_ESP32
	#include <vector>
	#include <string>
#else
	#include <ArduinoSTL.h>
#endif

extern "C" {
int _write(int fd, char *ptr, int len) {
  (void) fd;
  return Serial.write(ptr, len);
}
}
////////////DEBUG////////////
// #include "avr8-stub.h"
////////////DEBUG////////////

//AccelSensor sensor();

//Logger* logger = GlobalLogger->getLogger();
AccelSensor *sensor;
time_t* currTime;
std::vector<int32_t> *lastData;

Communicator* comms_manager;

const int BAUDRATE = 9600;

void setup()
{
    comms_manager = new Communicator(WIFI_SSID, "", 80);
    sensor = new AccelSensor(0x68, 8);
    // ////////////DEBUG////////////
    // // debug_init();
    // ////////////DEBUG////////////
    Serial.begin(BAUDRATE);
    sensor->powerUpSensor(0x6B, 0);
    while(!sensor->isReady())
    {  
        Serial.println("Waiting for sensor...");
    }
    Serial.println("Sensor active and communication port open!");
    Serial.printf("Running LOOP on core %d\n", xPortGetCoreID());
}

void loop()
{
    // sensor->continuousReading(5000, 1);
    // lastData = sensor->getLastData();
    // Serial.print("Last Data ");
    // for(size_t i=0; lastData->size(); i++)
    // {
    //     Serial.printf("%.3f ", lastData->at(i));
    // }
    // Serial.printf("\n");
    // // delay(2000);
    // Serial.println("Here comes!");
    // //sensor.continuousReading(5000, 10);

}