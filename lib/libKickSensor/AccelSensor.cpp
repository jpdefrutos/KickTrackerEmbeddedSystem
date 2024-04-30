#include "AccelSensor.h"
#include <time.h>
#include <Wire.h>

template <typename T>
AccelSensor<T>::AccelSensor(int address, int bufferSize) : SensorManager<T>(address, bufferSize)
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
    std::vector<T> _buffer(mSensorBufferSize, 0);
    mSensorBuffer = &_buffer;
    mReady = true;
};

template <typename T>
int AccelSensor<T>::readSensor(std::vector<T> *returnValue)
{
    int retValue = 0;
    if (mReady)
    {
        returnValue->clear();
        mSensor->read();
        mSensor->getEvent(&mLastEvent);
        returnValue->push_back(millis());
        returnValue->push_back(mLastEvent.acceleration.x);
        returnValue->push_back(mLastEvent.acceleration.y);
        returnValue->push_back(mLastEvent.acceleration.z);
        
        mLastDataStream = returnValue;
                
        // returnValue = mSensorBuffer;
        retValue = returnValue->size();
    }

    return mSensorBufferSize;
};

template <typename T>
String AccelSensor<T>::formatDataStream(const std::vector<T>* dataStream)
{
    char* stringData;
    if(dataStream->size() > 0)
    {
        Serial.println("B");
        sprintf(stringData, "%f:%f:%f:%f", dataStream->at(0),  dataStream->at(1),  dataStream->at(2), dataStream->at(3));
        Serial.printf("Formatter data %s\n", stringData);
    }
    else
    {
        Serial.println("[ERR] Stream is empty");
    }
    
    Serial.println("C");
    return String(stringData);
}

template <typename T>
std::vector<T>* AccelSensor<T>::getLastData()
{
    return mLastDataStream;
}

template class AccelSensor<float>;