
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


template <typename T>
class SensorManager
{
public:
	SensorManager(int address, int bufferSize = 32);
	void powerUpSensor(const int regAddress, const int value);
	virtual int readSensor(std::vector<T> *readValue);
	void continuousReading(std::vector<T> *readValue, int durationMs, float freqHz);
	void setAddress(int address);
	int getAddress(int &address);
	bool isReady();
	int getBufferSize();
	void setBufferSize(int bufferSize);

protected:
	int mSensorAddress;
	bool mReady = false;
	unsigned int mBufferSize;
	std::vector<T>* mBuffer;

private:

};

#endif //_SENSORMANAGER_H_