#include <ArduinoSTL.h>
#include <WiFi.h>

typedef struct Task
{
    char taskID;
    int* numParams;
    string* strParams;
};


class Communications
{
public:
    Communications(string ssid, string psw);
    void openServer(string ip, int port);
    string readMessage(WiFiClient client);
    int sendMessage();
    int getStatus();
    string parseData(std::vector<int32_t> &dataVector);


private:
    const int mBufferSize = 32;
    const int mMaxRetry = 100;

    queue *mQueue;
    queue mTasksQueue;
    int mStatus = WL_IDLE_STATUS;
    bool mCollectingDataFlag = false;
    bool mRunServer = false;
    WiFiServer mServer(80);
    queue
};