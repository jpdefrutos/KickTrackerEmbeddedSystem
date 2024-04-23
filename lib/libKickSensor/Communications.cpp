#include "Communications.h"


Communicator::Communicator(String ssid, String pwd, int port)
{
    digitalWrite(LED_BUILTIN, LOW);
    // ssid, psw, ip, port should be saved in a config file and read upon power up
    Serial.printf("Setting up network: %s", ssid);
    setupWiFiNetwork(ssid, pwd);
    openServer(port);
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
    //     openServer(ip, port);
    //     digitalWrite(LED_BUILTIN, HIGH);
    //     digitalWrite(LEDR, LOW);
    //     digitalWrite(LEDG, HIGH);
    //     digitalWrite(LEDB, LOW);
    // }

}


void Communicator::setQueue(std::queue<std::vector<int32_t>>* queue)
{
    mQueue = queue;
}


void Communicator::setupWiFiNetwork(String ssid, String pwd)
{
    WiFi.softAP(ssid);  // Add WIFI_PSW if needed
    Serial.printf("Created network %s\n", ssid);
    Serial.printf("IP: %s\n", WiFi.softAPIP());
    Serial.printf("Signal strength: %d dB\n", WiFi.RSSI());
    mNetwork = true;
}

void Communicator::openServer(int port)
{
    if(!mNetwork)
    {
        Serial.println("Network is not ready. Did you run setupWiFiNetwork()?");
    }
    else
    {
        mServer = new WiFiServer(port);
        mServer->begin();

        Serial.printf("Server ready. Listening at port: %d\n", port);
    }
}

void Communicator::handleServer()
{
    mRunServer = true;
    WiFiClient client;
    Task receivedMessage;
    String parsedData;
    
    Serial.println("Running server");
    while(mRunServer)
    {
        client = mServer->available();

        if(client)
        {
            while(client.connected())
            {
                if(client.available())
                {
                    receivedMessage = readMessage(client);
                    Serial.printf("Received %s\n", receivedMessage.raw);
                    sendACK();
                    mTasksQueue.push(receivedMessage);
                }
            }
        }
        else
        {
            if(!mTasksQueue.empty())
            {
                Task newTask = mTasksQueue.front();
                mTasksQueue.pop();
                switch(newTask.taskID)
                {
                    case START_ACQUISITION:
                        Serial.println("START_ACQUISITION task received");
                        break;

                    case STOP_ACQUISITION:
                        Serial.println("STOP_ACQUISITION task received");
                        break;

                    case TIMED_ACQUISITION:
                        Serial.println("TIMED_ACQUISITION task received");
                        break;

                    case GET_CONFIGURATION:
                        Serial.println("GET_CONFIGURATION task received");
                        break;

                    case UPDATE_CONFIGURATION:
                        Serial.println("UPDATE_CONFIGURATION task received");
                        break;

                    case POWER_OFF:
                        Serial.println("POWER_OFF task received");
                        break;

                    case ALIVE:
                        Serial.println("ALIVE task received");
                        sendACK();
                        break;

                }
            }
        }

    }

}

void Communicator::setDataLoop()
{
    mCollectingDataFlag = true;
    String parsedData;
    while(!mQueue->empty() && !mCollectingDataFlag)
    {
        parsedData = formatSensorData(mQueue->front());
        mQueue->pop();
        sendMessage(parsedData);
    }
}

void Communicator::sendACK()
{
    sendMessage("OK");
}

Task Communicator::readMessage(WiFiClient client)
{
    String rcvMsg = client.readString();
    
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
};

void Communicator::parseParametersString(String rawString, Task &taskInformation)
{
    size_t pos = 0;
    String token;
    size_t i=0;
    for(i=0; i<taskInformation.numParams; i++)
    {
        if(pos = rawString.indexOf(PARAM_DELIMITER) != -1)
        {
            *taskInformation.strParams[i] = rawString.substring(0, pos);
            rawString.remove(0, pos + 1);
        }
    }
    *taskInformation.strParams[i+1] = rawString;
}

String Communicator::formatSensorData(std::vector<int32_t> &dataVector)
{
    String retVal;
    return retVal;
}

int Communicator::sendMessage(String message)
{
    return mServer->write(message.c_str());
};

int Communicator::sendDataOnQueue(String dataLocation, int numberSamples)
{
    for(size_t i=0; i < numberSamples; i++)
    {

    }
}

int Communicator::getStatus()
{
    mStatus = WiFi.status();
    return mStatus;
}

void Communicator::stopServer()
{
    mRunServer = false;
    Serial.println("Stopping server");
}

void Communicator::updataDataCollectionFlag(bool newStatus)
{
    mCollectingDataFlag = newStatus;
}

void Communicator::parseMessage(String message)
{

}