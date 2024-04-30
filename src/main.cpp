#include "libKickSensor.h"
#ifdef ARDUINO_NANO_ESP32
	#include <vector>
	#include <string>
#else
	#include <ArduinoSTL.h>
#endif

extern "C" {
int _write(int fd, char *ptr, int len) {
  (void) fd;
  return Serial.write(ptr, len);
}
}


typedef enum{
    START,
    SENDING_DATA,
} STATES_VALUE;

int STATE = START;

///////// SENSOR //////////
AccelSensor<float> *sensor;
time_t* currTime;
int sensorBufferSize = 8;
std::vector<float> dataSample(sensorBufferSize);
///////// SENSOR //////////

///////// COMMUNICATION//////////
const int BAUDRATE = 115200;

int socketPort = 3500;
IPAddress socketIP;
IPAddress broadcastIP;

int clientPort;
IPAddress clientIP;

WiFiUDP *socket;
Communicator<float>* comms_manager;

bool socketIsOpen = false;
bool dataSocketOpen = false;
///////// COMMUNICATION//////////
void setup()
{
    Serial.begin(BAUDRATE);
    WiFi.softAP(WIFI_SSID);
    
    // sensor = new Adafruit_MMA8451();
    sensor = new AccelSensor<float>(0x68, sensorBufferSize);
    sensor->powerUpSensor(0x6B, 0);

    while(!sensor->isReady())
    {  
        Serial.println("Waiting for sensor...");
    }

    socketIP = WiFi.softAPIP();
    broadcastIP = IPAddress(255, 255, 255, 255);

    Serial.println("Sensor active and communication port open!");
    Serial.printf("Running LOOP on core %d\n", xPortGetCoreID());

    STATE = START;
}

void loop()
{
    switch (STATE)
    {
    case START:
        socket = new WiFiUDP();
        
        if(socket->begin(socketPort))
        {
            Serial.println("UDP Broadcast socket ready");
            socketIsOpen = true;

            int numAttempts = 10;
            bool success = false;
            static char msgBuffer[1024] = "";
            Task parsedMsg;
            do
            {
                sprintf(msgBuffer, "0;2;%s:%d;", WiFi.softAPIP().toString().c_str(), socketPort);
                socket->beginPacket(broadcastIP, socketPort);
                socket->printf(msgBuffer);
                socket->endPacket(); 

                if(socket->parsePacket())
                {
                    int len = socket->read(msgBuffer, 1024);
                    if (len)
                    {
                        msgBuffer[len] = 0;
                    }
                    Serial.printf("Received message from %s: %s\n", socket->remoteIP().toString().c_str(), msgBuffer);
                    parsedMsg = comms_manager->parseMessage(msgBuffer);
                    clientIP.fromString(parsedMsg.strParams[0]);
                    clientPort = parsedMsg.strParams[1].toInt();
                    Serial.printf("Received client data receiver socket info: %s @ %s\n", parsedMsg.strParams[1].c_str(), clientIP.toString().c_str());
                    success = true;
                    numAttempts = 0;
                }
                else
                {
                    numAttempts--;
                    delay(5000);
                }
            } while (numAttempts || !success);

            if(success)
            {
                STATE = SENDING_DATA;
            }
        }
        break;
    
    case SENDING_DATA:
        if (!socketIsOpen)
        {
            Serial.println("A");
            socket->stop();
            socket = new WiFiUDP();
            socketIsOpen = socket->begin(socketPort);
        }
        
        Serial.println("Sending data!");
        sensor->readSensor(&dataSample);
        
        Serial.printf("Sensor read [%f, %f, %f] at %f\n",  dataSample.at(1),  dataSample.at(2),  dataSample.at(3), dataSample.at(0));
        
        Task dataTask = comms_manager->formatSensorDataTask(dataSample);
        comms_manager->formatTaskMessage(dataTask);

        socket->beginPacket(clientIP, clientPort);
        socket->printf(dataTask.raw.c_str());
        socket->endPacket(); 
        Serial.printf("Sent: {%s} to %d@%s\n", dataTask.raw.c_str(), clientPort, clientIP.toString().c_str());

        break;

    }
    delay(100);
}