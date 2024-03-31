#include "SensorManager.h"
#include <Arduino.h>
#include <TimeLib.h>
#include "tools.h"


SensorManager::SensorManager(int address, int bufferSize = 32)
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
	std::cout << "[ERR] readSensor is a virtual method" << std::endl;
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

	std::cout << "Start collection... of " << durationMs << " ms every " << waitBetweenLoops << " ms." << std::endl;

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
			std::cout << "Loop!!" << std::endl;
			readSize = readSensor(mBuffer);
			std::cout << "Received size: " << readSize << std::endl;
			for(size_t i=0; i < readSize; i++)
			{
				std::cout << "["<< i << "]" << "\t" << mBuffer->at(i) << std::endl;
			};
			lastIterationTime = millis();
		};
	};
	std::cout << "... Done!" << std::endl;
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