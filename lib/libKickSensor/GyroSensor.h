#ifndef _GYROSENSOR_H_
#define _GYROSENSOR_H_


#ifdef ARDUINO_NANO_ESP32
	#include <vector>
	#include <string>
#else
	#include <ArduinoSTL.h>
#endif
#include <Streaming.h>
#include "SensorManager.h"

#define CLASSNAME "AccelSensor"

using namespace std;

class GyroSensor : public SensorManager
{
public:
	GyroSensor(int address, int bufferSize);
	int readSensor(std::vector<int32_t> *returnValue);
	std::vector<int32_t>* getLastData();
	String formatDataStream(const std::vector<int32_t> *dataStream);

private:
	std::vector<int32_t>* mLastDataStream;
	String mLastDataStreamString;
	int mSensorBufferSize;
	std::vector<int32_t>* mSensorBuffer;
};

#endif