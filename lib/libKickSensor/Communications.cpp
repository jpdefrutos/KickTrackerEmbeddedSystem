#include "Communications.h"


Communicator::Communicator(String ssid, String pwd, int port)
{
    digitalWrite(LED_BUILTIN, LOW);
    // ssid, psw, ip, port should be saved in a config file and read upon power up
    std::cout << "Setting up network" << ssid ;
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
    std::cout << "Created network " << ssid << std::endl;
    std::cout << "IP: " << WiFi.softAPIP() << std::endl;
    std::cout << "Signal strength: " << WiFi.RSSI() << " dB" << std::endl;
    mNetwork = true;
}

void Communicator::openServer(int port)
{
    if(!mNetwork)
    {
        std::cout << "Network is not ready. Did you run setupWiFiNetwork()?" << std::endl;
    }
    else
    {
        mServer = new WiFiServer(port);
        mServer->begin();

        std::cout << "Server ready. Listening at port: " << port << std::endl;
    }
}

void Communicator::handleServer()
{
    mRunServer = true;
    WiFiClient client;
    Task receivedMessage;
    String parsedData;
    
    std::cout << "Running server" << std::endl;
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
                    std::cout << "Received " << receivedMessage.raw << std::endl;
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
                        std::cout << "START_ACQUISITION task received" << std::endl;
                        break;

                    case STOP_ACQUISITION:
                        std::cout << "STOP_ACQUISITION task received" << std::endl;
                        break;

                    case TIMED_ACQUISITION:
                        std::cout << "TIMED_ACQUISITION task received" << std::endl;
                        break;

                    case GET_CONFIGURATION:
                        std::cout << "GET_CONFIGURATION task received" << std::endl;
                        break;

                    case UPDATE_CONFIGURATION:
                        std::cout << "UPDATE_CONFIGURATION task received" << std::endl;
                        break;

                    case POWER_OFF:
                        std::cout << "POWER_OFF task received" << std::endl;
                        break;

                    case ALIVE:
                        std::cout << "ALIVE task received" << std::endl;
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
            std::cout << "Unexpected message length: " << token << std::endl;
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
    std::cout << "Stopping server" << std::endl;
}

void Communicator::updataDataCollectionFlag(bool newStatus)
{
    mCollectingDataFlag = newStatus;
}

void Communicator::parseMessage(String message)
{

}