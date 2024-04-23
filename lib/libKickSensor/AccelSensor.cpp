#include "AccelSensor.h"
#include <time.h>
#include <Wire.h>


AccelSensor::AccelSensor(int address, int bufferSize) : SensorManager(address, bufferSize)
{
    mSensor = new Adafruit_MMA8451();
    while(!mSensor->begin())
    {
        Serial.println("Initializing accelerometer...");
    }
    mSensor->setRange(MMA8451_RANGE_8_G);
    mSensor->setDataRate(MMA8451_DATARATE_800_HZ);
    mma8451_range_t range = mSensor->getRange();
    Serial.printf("[DEB] Sensor active (%d G)\n", range);
    
    mSensorBufferSize = bufferSize;
    std::vector<int32_t> _buffer(mSensorBufferSize, 0);
    mSensorBuffer = &_buffer;
    mReady = true;
};

int AccelSensor::readSensor(std::vector<int32_t> *returnValue)
{
    if (mReady)
    {
        returnValue->clear();
        mSensor->read();
        mSensor->getEvent(mLastEvent);

        returnValue->push_back(millis());
        returnValue->push_back(mLastEvent->acceleration.x);
        returnValue->push_back(mLastEvent->acceleration.y);
        returnValue->push_back(mLastEvent->acceleration.z);
    };

    mLastDataStream = returnValue;
    mLastDataStreamString = formatDataStream(returnValue);
    // returnValue = mSensorBuffer;
    return mSensorBufferSize;
};

String AccelSensor::formatDataStream(const std::vector<int32_t>* dataStream)
{
    String stringData = "";
    if(dataStream->size() > 0)
    {
        for(size_t i=0; i < dataStream->size(); i++)
        {
            stringData += stringData + dataStream->at(i) + ";";
        } 
        Serial.printf("Converted stream: %s\n", stringData);
    }
    else
    {
        Serial.println("[ERR] Stream is empty");
    }
    return stringData;
}

std::vector<int32_t>* AccelSensor::getLastData()
{
    return mLastDataStream;
}