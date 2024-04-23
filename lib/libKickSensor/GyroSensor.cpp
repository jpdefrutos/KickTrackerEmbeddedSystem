#include "GyroSensor.h"
#include <time.h>
#include <Wire.h>


GyroSensor::GyroSensor(int address, int bufferSize) : SensorManager(address, bufferSize)
{
    mSensorBufferSize = bufferSize;
    std::vector<int32_t> _buffer(mSensorBufferSize, 0);
    mSensorBuffer = &_buffer;
};

int GyroSensor::readSensor(std::vector<int32_t> *returnValue)
{
    if (mReady)
    {
        returnValue->clear();
        Wire.beginTransmission(this->mSensorAddress);
        Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
        Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
        Wire.requestFrom(this->mSensorAddress, 7 * 2, true); // request a total of 7*2=14 registers
        
        returnValue->push_back(millis());
        int32_t buffer = 0;
        for (size_t i = 1; i < mSensorBufferSize; i++)
        {
            buffer = 0;
            // In order, reading registers:
            // 0     0x3B and 0x3C -> ACCEL_XOUT_H and ACCEL_XOUT_L
            // 1     0x3D and 0x3E -> ACCEL_YOUT_H and ACCEL_YOUT_L
            // 2     0x3F and 0x40 -> ACCEL_ZOUT_H and ACCEL_ZOUT_L
            // 3     0x41 and 0x42 -> TEMP_OUT_H and TEMP_OUT_L
            // 4     0x43 and 0x44 -> GYRO_XOUT_H and GYRO_XOUT_L
            // 5     0x45 and 0x46 -> GYRO_YOUT_H and GYRO_YOUT_L
            // 6     0x47 and 0x48 -> GYRO_ZOUT_H and GYRO_ZOUT_L
            buffer = Wire.read() << 8 | Wire.read();
            Serial.printf("ACC %d\n", buffer);
            if(i==3) // Temperature
            {
                returnValue->push_back(buffer / 340.00 + 36.53); //from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
            }
            else
            {
                returnValue->push_back(buffer);
            }

        };
    };

    mLastDataStream = returnValue;
    mLastDataStreamString = formatDataStream(returnValue);
    // returnValue = mSensorBuffer;
    return mSensorBufferSize;
};

String GyroSensor::formatDataStream(const std::vector<int32_t>* dataStream)
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
        Serial.printf("[ERR] Stream is empty");
    }
    return stringData;
}

std::vector<int32_t>* GyroSensor::getLastData()
{
    return mLastDataStream;
}