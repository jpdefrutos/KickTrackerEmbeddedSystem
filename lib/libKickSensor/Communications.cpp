#include "Communications.h"


template<typename T>
Communicator<T>::Communicator(String ssid, String pwd, IPAddress serverIP, int serverPort, QueueHandle_t* dataQueue, QueueHandle_t* tasksQueue)
{
    if(*dataQueue != NULL)
    {
        mDataQueue = dataQueue;
    }
    
    if(*tasksQueue != NULL)
    {
        mTasksQueue = tasksQueue;
    }

    IPAddress subnetIP(192, 168, 1, 1);
    IPAddress gatewayIP(255, 255, 0, 0);
    digitalWrite(LED_BUILTIN, LOW);
    // ssid, psw, ip, port should be saved in a config file and read upon power up
    Serial.printf("Setting up network: %s", ssid);
    WiFi.config(serverIP, gatewayIP, subnetIP);
    setupWiFiNetwork(ssid, pwd);
    setupCommsSocket();
    setupBroadcastSocket();
    // int retries = 3;
    // do
    // {
    //     std::cout << ".";
    //     mStatus = WiFi.begin(ssid.c_str(), psw.c_str());
    //     delay(5000);
    //     retries--;/* code */
    // } while (mStatus != WL_CONNECTED && retries);
    
    // if(mStatus != WL_CONNECTED)
    // {
    //     std::cout << "Unable to connect to " << ssid << std::endl;
    //     digitalWrite(LED_BUILTIN, HIGH);
    //     digitalWrite(LEDR, HIGH);
    //     digitalWrite(LEDG, LOW);
    //     digitalWrite(LEDB, LOW);
    // }
    // else
    // {
    //     setupCommsSocket(ip, port);
    //     digitalWrite(LED_BUILTIN, HIGH);
    //     digitalWrite(LEDR, LOW);
    //     digitalWrite(LEDG, HIGH);
    //     digitalWrite(LEDB, LOW);
    // }

}

template<typename T>
void Communicator<T>::setupWiFiNetwork(String ssid, String pwd)
{
    if(WiFi.softAP(ssid))
    {
         // Add WIFI_PSW if needed
        Serial.printf("Created network %s\n", ssid);
        mWiFiNetworkOpen = true; 
        mBoardIP = WiFi.softAPIP();
        mBroadcastIP = IPAddress(255, 255, 255, 255);
        mServerIP = IPAddress(mBoardIP);
        mServerIP[3] = 10;
        Serial.printf("Board IP: %s\tBroadcast IP: %s\n", mBoardIP.toString(), mBroadcastIP.toString());
        Serial.printf("Signal strength: %d dB\n", WiFi.RSSI());
    }
    else
    {
        Serial.println("Cannot setup WiFi");
        mWiFiNetworkOpen = false; 
    }
}

template<typename T>
void Communicator<T>::setupBroadcastSocket()
{
    if(!mBroadcastSocketOpen)
    {
        mBroadcastServer = new WiFiUDP();
        
        if(mBroadcastServer->begin(mBroadcastPort))
        {
            Serial.println("UDP Broadcast socket ready");
            mBroadcastSocketOpen = true;
        }
        else
        {
            mBroadcastSocketOpen = false;
        }
    }
}

template<typename T>
bool Communicator<T>::broadcastMessage()
{
    int numAttempts = 10;
    bool success = false;
    static char rcvBuffer[1024] = "";
    Task parsedMsg;
    if(mBroadcastSocketOpen)
    {
        do
        {
            sprintf(rcvBuffer, "0;2;%s:%d;", mServerIP.toString().c_str(), mServerPort);
            mBroadcastServer->beginPacket(mBroadcastIP, mBroadcastPort);
            mBroadcastServer->printf(rcvBuffer);
            mBroadcastServer->endPacket(); 

            if(mBroadcastServer->parsePacket())
            {
                int len = mBroadcastServer->read(rcvBuffer, 1024);
                if (len)
                {
                    rcvBuffer[len] = 0;
                }
                Serial.printf("Received message from %s: %s\n", mBroadcastServer->remoteIP().toString().c_str(), rcvBuffer);
                parsedMsg = parseMessage(rcvBuffer);
                mClientSocketIP.fromString(parsedMsg.strParams[0]);
                mClientCommsSocketPort = parsedMsg.strParams[1].toInt();
                Serial.printf("Received client data receiver socket info: %s @ %s\n", parsedMsg.strParams[1].c_str(), mClientSocketIP.toString().c_str());
                success = true;
                numAttempts = 0;
            }
            else
            {
                numAttempts--;
                delay(5000);
            }
        } while (numAttempts || !success);
        mBroadcastServer->stop();
    }
    return success;
}

template<typename T>
void Communicator<T>::setupCommsSocket()
{
    if(!mWiFiNetworkOpen)
    {
        Serial.println("Network is not ready. Did you run setupWiFiNetwork()?");
        mCommsSocketOpen = false;
    }
    else
    {
        mServer = new WiFiUDP();
        mServer->begin(mServerPort);
        if(mServer)
        {
            Serial.print("Server ready. Listening at IP: ");
            Serial.print(mServerIP);
            Serial.printf(" and port: %d\n", mServerPort);
            mCommsSocketOpen = true;
        }
        else
        {
            Serial.println("Failed to open server...");
            mCommsSocketOpen = false;
        }
    }
}

template<typename T>
void Communicator<T>::handleCommunication()
{
    WiFiClient client;
    Task receivedMessage;
    String parsedData;
    static char messageBuffer[1024];

    Task rcvTask;
    std::vector<T> rcvDataSample;

    int objectInQueue = 0;
    int packetSize = 0;
    int readBytes = 0;
    Serial.println("Running server");
    while(mCommsSocketOpen)
    {
        packetSize = mServer->parsePacket();
        if(packetSize)
        {
            readBytes = mServer->read(messageBuffer, 1024);
            if (readBytes)
            {
                messageBuffer[readBytes] = 0;
            }
            receivedMessage = parseMessage(messageBuffer);
            Serial.printf("Received %s\n", receivedMessage.raw);
            sendACK();
            if (receivedMessage.taskID != ALIVE)
            {
                if(uxQueueSpacesAvailable(*mTasksQueue) > 0)
                {
                    int ret = xQueueSend(*mTasksQueue, (void*) &receivedMessage, 0);
                    if(ret == errQUEUE_FULL)
                    {
                        Serial.println("mTasksQueue is full!");
                    }
                }
            }


        }

        objectInQueue = xQueueReceive(*mTasksQueue, &rcvTask, 0);
        if(objectInQueue == pdPASS)
        {
            Serial.printf("New task unqueued: Task ID: %d\n", rcvTask.taskID);
            switch(rcvTask.taskID)
            {
                case PREPARE_ACQUISITION:
                    Serial.println("PREPARE_ACQUISITION task received");
                    mAcquisitioTimeout = rcvTask.strParams[0].toInt();
                    xQueueReset(*mDataQueue);
                    break;

                case START_ACQUISITION:
                    Serial.println("START_ACQUISITION task received");
                    esp_event_post(COMMUNICATOR_EVENTS, COMMUNICATOR_EVENT_START_ACQUISITION, (void *) &mAcquisitioTimeout, sizeof(mAcquisitioTimeout), 0);
                    break;

                case STOP_ACQUISITION:
                    Serial.println("STOP_ACQUISITION task received");
                    esp_event_post(COMMUNICATOR_EVENTS, COMMUNICATOR_EVENT_STOP_ACQUISITION, NULL, sizeof(NULL), 0);
                    break;

                case POWER_OFF:
                    Serial.println("POWER_OFF task received");
                    break;
                
                case DATA_SAMPLE:
                    Serial.println("DATA_SAMPLE received. Forwarding...");
                    formatTaskMessage(rcvTask);
                    mBroadcastServer->beginPacket(mClientSocketIP, mClientDataSocketPort);
                    mBroadcastServer->printf(rcvTask.raw.c_str());
                    mBroadcastServer->endPacket(); 
            }
        }
        objectInQueue = xQueueReceive(*mDataQueue, &rcvDataSample, 0);
        if(objectInQueue == pdPASS)
        {
            Task dataSampleTask = formatSensorDataTask(rcvDataSample);
            if(uxQueueSpacesAvailable(*mTasksQueue) > 0)
            {
                int ret = xQueueSend(*mTasksQueue, (void*) &dataSampleTask, 0);
                if(ret == errQUEUE_FULL)
                {
                    Serial.println("mTasksQueue is full!");
                }
            }
        }
    }
}

template<typename T>
void Communicator<T>::setDataLoop()
{

}

template<typename T>
void Communicator<T>::sendACK()
{
    static char message[1024];
    sprintf(message, "%d;0;;", ALIVE);
    sendMessage(message);
}

template<typename T>
Task Communicator<T>::parseMessage(String rcvMsg)
{       
    Task parsedMsg;
    parsedMsg.raw = rcvMsg;


    size_t pos = 0;
    size_t tokenPos = 0;
    String token;
    String param;
    while ((pos = rcvMsg.indexOf(MSG_DELIMITER)) != -1) {
        token = rcvMsg.substring(0, pos);

        switch (tokenPos)
        {
        case 0:
            parsedMsg.taskID = static_cast<TASK_ID>(token.toInt());
            break;
        
        case 1:
            parsedMsg.numParams = token.toInt();
            break;
        
        case 2:
            parseParametersString(token, parsedMsg);
            break;
        default:
            Serial.printf("Unexpected message length: %d\n", token);
        }
        tokenPos += 1;
        rcvMsg.remove(0, pos + 1);
    }

    return parsedMsg;
}

template<typename T>
void Communicator<T>::parseParametersString(String rawString, Task &taskInformation)
{
    size_t pos = 0;
    String token;
    size_t i=0;
    for(i=0; i < taskInformation.numParams; i++)
    {
        if((pos = rawString.indexOf(PARAM_DELIMITER)) != -1)
        {
            taskInformation.strParams[i] = rawString.substring(0, pos);
            rawString.remove(0, pos + 1);
        }
        else
        {
            taskInformation.strParams[i] = rawString;
        }
    }
}

template<typename T>
const char* Communicator<T>::formatTaskMessage(Task &task)
{
    char buffer[1024];
    if(task.raw.length())
    {
        // The Task message has already been formated by someone else
        *buffer = *task.raw.c_str();
    }
    else{
        sprintf(buffer, "%d;%d;", task.taskID, task.numParams);
        if(task.numParams)
        {
            sprintf(buffer, "%s%s", buffer, task.strParams[0].c_str());
            for (size_t i=1; i < task.numParams; i++)
            {
                sprintf(buffer, "%s:%s", buffer, task.strParams[i].c_str());
            }
        }
        sprintf(buffer, "%s;\0", buffer);
        task.raw = String(buffer);
    }
    return buffer;
}

template<typename T>
Task Communicator<T>::formatSensorDataTask(std::vector<T> &dataVector)
{
    Task dataSampleTask;
    dataSampleTask.taskID = DATA_SAMPLE;
    dataSampleTask.numParams = dataVector.size();
    for(size_t i=0; i < dataSampleTask.numParams; i++)
    {
        dataSampleTask.strParams[i] = String(dataVector[i]);
    }

    return dataSampleTask;
}

template<typename T>
int Communicator<T>::sendMessage(const char* message)
{
    return mServer->printf(message);
};

template<typename T>
int Communicator<T>::sendMessage(String message)
{
    return sendMessage(message.c_str());
}

template<typename T>
int Communicator<T>::sendDataOnQueue(String dataLocation, int numberSamples)
{
    for(size_t i=0; i < numberSamples; i++)
    {

    }
}

template<typename T>
int Communicator<T>::getWiFiStatus()
{
    mStatus = WiFi.status();
    return mStatus;
}

template<typename T>
void Communicator<T>::stopServer()
{
    mCommsSocketOpen = false;
    Serial.println("Stopping server");
}

template<typename T>
void Communicator<T>::updataDataCollectionFlag(bool newStatus)
{

}

template<typename T>
WiFiClient Communicator<T>::available()
{
    return mServer->available();
}

template<typename T>
IPAddress Communicator<T>::getBoardIP()
{
    IPAddress retValue;
    if(mWiFiNetworkOpen)
    {
        retValue = WiFi.softAPIP();
    }
    return retValue;
}

template<typename T>
IPAddress Communicator<T>::getServerIP()
{
    IPAddress retValue;
    if(mCommsSocketOpen)
    {
        retValue = mServerIP;
    }
    return retValue;
}

template<typename T>
int32_t Communicator<T>::getServerPort()
{
    int32_t retValue;
    if(mCommsSocketOpen)
    {
        retValue = mServerPort;
    }
    return retValue;
}

template<typename T>
IPAddress Communicator<T>::getBroadcastIP()
{
    IPAddress retValue;
    if(mWiFiNetworkOpen)
    {
        retValue = mBroadcastIP;
    }
    return retValue;

}

template<typename T>
int32_t Communicator<T>::getBroadcastPort()
{
    int32_t retValue;
    if(mCommsSocketOpen)
    {
        retValue = mBroadcastPort;
    }
    return retValue;
}

template<typename T>
bool Communicator<T>::isServerRunning()
{
    return mCommsSocketOpen;
}

template<typename T>
bool Communicator<T>::isNetworkRunning()
{
    return mWiFiNetworkOpen;
}

template<typename T>
IPAddress Communicator<T>::getRemoteIP()
{
    return mBroadcastServer->remoteIP();
}

template<typename T>
int Communicator<T>::getRemotePort()
{
    return mBroadcastServer->remotePort();
}

template<typename T>
QueueHandle_t* Communicator<T>::getReferenceToDataQueue()
{
    return mDataQueue;
}

template<typename T>
QueueHandle_t* Communicator<T>::getReferenceToTaskQueue()
{
    return mTasksQueue;
}

template class Communicator<float>;
template class Communicator<int32_t>;