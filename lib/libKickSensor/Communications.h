#ifdef ARDUINO_NANO_ESP32
	#include <vector>
	#include <string>
	#include <iostream>
    #include <queue>
#else
	#include <ArduinoSTL.h>
#endif
#include <WiFi.h>
#include <SPI.h>

typedef struct Task
{
    char taskID;
    int* numParams;
    String* strParams;
};


class Communicator
{
public:
    Communicator(String ssid, String psw, String ip, int port);
    void openServer(String ip, int port);
    String readMessage(WiFiClient client);
    int sendMessage(String message);
    int getStatus();
    String parseData(std::vector<int32_t> &dataVector);
    void setQueue(std::queue<String>* queue);
    void handleServer();
    void setDataLoop();
    void sendDataQueue(String dataLocation, int numberSamples);
    void stopServer();
    void updateDataCollectionFlag(bool newStatus);
    void parseMessage(String message);

private:
    const int mBufferSize = 32;
    const int mMaxRetry = 100;

    std::queue<String>* mQueue;
    std::queue<int> mTasksQueue;
    int mStatus = WL_IDLE_STATUS;
    bool mCollectingDataFlag = false;
    bool mRunServer = false;
    WiFiServer mServer(0);
};