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

template <typename T>
class GyroSensor : public SensorManager<T>
{
public:
	GyroSensor(int address, int bufferSize);
	int readSensor(std::vector<T> *returnValue);
	std::vector<T>* getLastData();
	String formatDataStream(const std::vector<T> *dataStream);

private:
	std::vector<T>* mLastDataStream;
	String mLastDataStreamString;
	int mSensorBufferSize;
	std::vector<T>* mSensorBuffer;
	bool mReady = false;
};

#endif