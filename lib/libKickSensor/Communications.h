#ifdef ARDUINO_NANO_ESP32
	#include <vector>
	#include <string>
    #include <queue>
#else
	#include <ArduinoSTL.h>
#endif
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <SPI.h>
#include "constants.h"

ESP_EVENT_DECLARE_BASE(COMMUNICATOR_EVENTS);

typedef enum {
    COMMUNICATOR_EVENT_NEW_MESSAGE = 0,
    COMMUNICATOR_EVENT_START_ACQUISITION,
    COMMUNICATOR_EVENT_STOP_ACQUISITION
} communicator_event_id_t;

typedef void (*CommunicatorEventCallback)(communicator_event_id_t event);

typedef enum
{
    HAND_SHAKE = 0,
    PREPARE_ACQUISITION,
    START_ACQUISITION,
    STOP_ACQUISITION,
    DATA_SAMPLE,
    POWER_OFF,
    ALIVE,
} TASK_ID;


typedef struct
{
    TASK_ID taskID;
    int numParams;
    String strParams[5];
    String raw;
} Task;

template<typename T>
class Communicator
{
public:
    Communicator(String ssid, String pwd, IPAddress serverIP, int serverPort, QueueHandle_t* dataQueue, QueueHandle_t* tasksQueue);
    void setupWiFiNetwork(String ssid, String pwd);
    void setupCommsSocket();
    Task parseMessage(String rcvMsg);
    int sendMessage(const char* message);
    int sendMessage(String message);
    void sendACK();
    int getWiFiStatus();
    Task formatSensorDataTask(std::vector<T> &dataVector);
    const char* formatTaskMessage(Task &task);
    void handleCommunication();
    void setDataLoop();
    int sendDataOnQueue(String dataLocation, int numberSamples);
    void stopServer();
    void updateDataCollectionFlag(bool newStatus);
    void parseParametersString(String rawString, Task &taskInformation);
    void updataDataCollectionFlag(bool newStatus);

    WiFiClient available();
    IPAddress getServerIP();
    int32_t getServerPort();
    IPAddress getBoardIP();
    IPAddress getBroadcastIP();
    int32_t getBroadcastPort();

    bool isServerRunning();
    bool isNetworkRunning();

    void setupBroadcastSocket();
    bool broadcastMessage();
    IPAddress getRemoteIP();
    int getRemotePort();

    QueueHandle_t* getReferenceToDataQueue();
    QueueHandle_t* getReferenceToTaskQueue();

    communicator_event_id_t onEvent(CommunicatorEventCallback callback);

private:
    const int mBufferSize = 32;
    const int mMaxRetry = 100;

    int mBroadcastAttempts = 3;

    // std::queue<std::vector<int32_t>>* mDataQueue;
    QueueHandle_t* mDataQueue;
    // std::queue<Task> mTasksQueue;
    QueueHandle_t* mTasksQueue;

    int mStatus = WL_IDLE_STATUS;
    bool mBroadcastSocketOpen = false;
    bool mCommsSocketOpen = false;
    bool mWiFiNetworkOpen = false;

    WiFiUDP* mServer;  // Default port is 80
    WiFiUDP* mBroadcastServer;

    IPAddress mServerIP;    // Comms TCP socket
    IPAddress mBoardIP;     // Board assigned IP
    IPAddress mBroadcastIP; // Broadcast IP
    int32_t mServerPort = 3000;
    int32_t mBroadcastPort = 3500;

    IPAddress mClientSocketIP;  // IP of the socket where to broadcast the sensor
    int32_t mClientCommsSocketPort = 3000;     // Port of the socket where to bradcast the sensor
    int32_t mClientDataSocketPort = 3500;

    int mAcquisitioTimeout = 0;
};