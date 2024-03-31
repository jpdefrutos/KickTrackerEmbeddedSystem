#ifndef _ACCELSENSOR_H_
#define _ACCELSENSOR_H_

#include <ArduinoSTL.h>
#include <Streaming.h>
#include "SensorManager.h"

#define CLASSNAME "AccelSensor"

using namespace std;

class AccelSensor : public SensorManager
{
public:
	AccelSensor(int address, int bufferSize);
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