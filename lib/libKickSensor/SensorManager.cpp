#include "SensorManager.h"
#include <Arduino.h>
#include <TimeLib.h>
#include "tools.h"


SensorManager::SensorManager(int address, int bufferSize)
{
	setAddress(address);
	mBufferSize = bufferSize;
	std::vector<int32_t> _buffer(mBufferSize, 0);
	mBuffer = &_buffer;
};

void SensorManager::powerUpSensor(const int regAddress, const int value)
{
	Wire.begin();
	Wire.beginTransmission(mSensorAddress);
	Wire.write(regAddress);
	Wire.write(value);
	Wire.endTransmission(true);
	mReady = true;
};

int SensorManager::readSensor(std::vector<int32_t> *returnValue)
{
	//GlobalLogger->logEvent(ERROR, CLASSNAME, "readSensor", "readSensor is a virtual method");
	Serial.println("[ERR] readSensor is a virtual method");
	return -1;
};

void SensorManager::continuousReading(int durationMs, float freqHz)
{
	time_t startTime, lastIterationTime;

	// std::vector<int32_t> storageArray[mBufferSize];
	// memset(storageArray, 0, mBufferSize);
	mBuffer->clear();
	
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
			readSize = readSensor(mBuffer);
			Serial.printf("Received size: %d\n", readSize);
			for(size_t i=0; i < readSize; i++)
			{
				Serial.printf("[%d]\t%.3f\n", i, mBuffer->at(i));
			};
			lastIterationTime = millis();
		};
	};
	Serial.println("... Done!");
};

void SensorManager::setAddress(int address)
{
	mSensorAddress = address;
};

int SensorManager::getAddress(int &address)
{
	if (address != NULL)
		address = mSensorAddress;
	return mSensorAddress;
};

bool SensorManager::isReady()
{
	return mReady;
};

int SensorManager::getBufferSize()
{
	return mBufferSize;
};

void SensorManager::setBufferSize(int bufferSize)
{
	mBufferSize = bufferSize;
}