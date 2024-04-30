#ifndef _ACCELSENSOR_H_
#define _ACCELSENSOR_H_


#ifdef ARDUINO_NANO_ESP32
	#include <vector>
	#include <string>
#else
	#include <ArduinoSTL.h>
#endif
#include <Streaming.h>
#include "SensorManager.h"
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include "constants.h"

#define CLASSNAME "AccelSensor"

using namespace std;

template<typename T>
class AccelSensor : public SensorManager<T>
{
public:
	AccelSensor(int address, int bufferSize);
	int readSensor(std::vector<T> *returnValue);
	std::vector<T>* getLastData();
	String formatDataStream(const std::vector<T> *dataStream);

private:
	Adafruit_MMA8451* mSensor;
	sensors_event_t mLastEvent;
	std::vector<T>* mLastDataStream;
	String mLastDataStreamString;
	int mSensorBufferSize;
	std::vector<T>* mSensorBuffer;
	bool mReady = false;
};

#endif