#include "SensorManager.h"
#include <Arduino.h>
#include <TimeLib.h>
#include "tools.h"

template <typename T>
SensorManager<T>::SensorManager(int address, int bufferSize)
{
	setAddress(address);
	mBufferSize = bufferSize;
	std::vector<T> _buffer(mBufferSize, 0);
	mBuffer = &_buffer;
};

template <typename T>
void SensorManager<T>::powerUpSensor(const int regAddress, const int value)
{
	Wire.begin();
	Wire.beginTransmission(mSensorAddress);
	Wire.write(regAddress);
	Wire.write(value);
	Wire.endTransmission(true);
	mReady = true;
};

template <typename T>
int SensorManager<T>::readSensor(std::vector<T> *returnValue)
{
	//GlobalLogger->logEvent(ERROR, CLASSNAME, "readSensor", "readSensor is a virtual method");
	Serial.println("[ERR] readSensor is a virtual method");
	return -1;
};

template <typename T>
void SensorManager<T>::continuousReading(std::vector<T> *returnValue, int durationMs, float freqHz)
{
	time_t startTime, lastIterationTime;

	// std::vector<int32_t> storageArray[mBufferSize];
	// memset(storageArray, 0, mBufferSize);
	returnValue->clear();
	
	float waitBetweenLoops = 1000 / freqHz;
	int readSize;

	Serial.printf("Start collection... of %d ms every %d ms", durationMs, waitBetweenLoops);

	startTime = millis();
	lastIterationTime = millis();
	while (millis() - startTime < durationMs)
	{	
		if(millis() - lastIterationTime < waitBetweenLoops)
		{
			//Wait
		}
		else
		{
			Serial.println("Loop!!");
			readSize = readSensor(returnValue);
			Serial.printf("Received size: %d\n", readSize);
			for(size_t i=0; i < readSize; i++)
			{
				Serial.printf("[%d]\t%.3f\n", i, returnValue->at(i));
			};
			lastIterationTime = millis();
		};
	};
	Serial.println("... Done!");
};

template <typename T>
void SensorManager<T>::setAddress(int address)
{
	mSensorAddress = address;
};

template <typename T>
int SensorManager<T>::getAddress(int &address)
{
	if (address != NULL)
		address = mSensorAddress;
	return mSensorAddress;
};

template <typename T>
bool SensorManager<T>::isReady()
{
	return mReady;
};

template <typename T>
int SensorManager<T>::getBufferSize()
{
	return mBufferSize;
};

template <typename T>
void SensorManager<T>::setBufferSize(int bufferSize)
{
	mBufferSize = bufferSize;
}

template class SensorManager<float>;
template class SensorManager<int32_t>;