#ifdef ARDUINO_NANO_ESP32
	#include <vector>
	#include <string>
	#include <iostream>
    #include <queue>
#else
	#include <ArduinoSTL.h>
#endif
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <SPI.h>
#include "constants.h"


#define WIFI_SSID "KickSensorWiFi"
#define WIFI_PSW "KickSensorLibrary"

enum TASK_ID
{
    START_ACQUISITION,
    STOP_ACQUISITION,
    TIMED_ACQUISITION,
    GET_CONFIGURATION,
    UPDATE_CONFIGURATION,
    POWER_OFF,
    ALIVE,
};


struct Task
{
    TASK_ID taskID;
    int numParams;
    String* strParams[5];
    String raw;
};

class Communicator
{
public:
    Communicator(String ssid, String pwd, int port);
    void setupWiFiNetwork(String ssid, String pwd);
    void openServer(int port);
    Task readMessage(WiFiClient client);
    int sendMessage(String message);
    void sendACK();
    int getStatus();
    String formatSensorData(std::vector<int32_t> &dataVector);
    void setQueue(std::queue<std::vector<int32_t>>* queue);
    void handleServer();
    void setDataLoop();
    int sendDataOnQueue(String dataLocation, int numberSamples);
    void stopServer();
    void updateDataCollectionFlag(bool newStatus);
    void parseMessage(String message);
    void parseParametersString(String rawString, Task &taskInformation);
    void updataDataCollectionFlag(bool newStatus);

private:
    const int mBufferSize = 32;
    const int mMaxRetry = 100;

    std::queue<std::vector<int32_t>>* mQueue;
    std::queue<Task> mTasksQueue;
    int mStatus = WL_IDLE_STATUS;
    bool mCollectingDataFlag = false;
    bool mRunServer = false;
    bool mNetwork = false;
    WiFiServer* mServer;  // Default port is 80
};