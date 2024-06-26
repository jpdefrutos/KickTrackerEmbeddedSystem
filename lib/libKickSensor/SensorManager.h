
#ifndef _SENSORMANAGER_H_
#define _SENSORMANAGER_H_

#include <Wire.h>
#ifdef ARDUINO_NANO_ESP32
	#include <vector>
	#include <string>
#else
	#include <ArduinoSTL.h>
#endif
#include "Logger.h"

#define CLASSNAME "SensorManager"


class SensorManager
{
public:
	SensorManager(int address, int bufferSize = 32);
	void powerUpSensor(const int regAddress, const int value);
	virtual int readSensor(std::vector<int32_t> *readValue);
	void continuousReading(int durationMs, float freqHz);
	void setAddress(int address);
	int getAddress(int &address);
	bool isReady();
	int getBufferSize();
	void setBufferSize(int bufferSize);

protected:
	int mSensorAddress;
	bool mReady = false;
	unsigned int mBufferSize;
	std::vector<int32_t>* mBuffer;

private:

};

#endif //_SENSORMANAGER_H_