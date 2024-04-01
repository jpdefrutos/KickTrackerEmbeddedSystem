#include "Communications.h"


Communications::Communicator(string ssid, string psw, string ip, int port)
{
    // ssid, psw, ip, port should be saved in a config file and read upon power up
    std::cout << "Connecting to " << ssid ;
    int retries = 3
    do
    {
        std::cout << ".";
        mStatus = WiFi.begin(ssid, psw);
        delay(5000);
        retries--;/* code */
    } while (mStatus != WL_CONNECTED && retries);
    
    if(mStatus != WL_CONNECTED)
    {
        std::cout << "Unable to connect to " << ssid << std::endl;
    }
    else
    {
        openServer(ip, port);
    }

}


void setQueue(queue* queue)
{
    mQueue = queue;
}


void Communications::openServer(string ip, int port);
{
    mServer.begin();
    WiFi.config(ip);
    mServer = WiFiServer(port);

    std::cout << "Connected to " << WiFi.SSID() << std::endl;
    std::cout << "\tIP: " << WiFi.localIP() << "\tPort: " << port << std::endl;
    std::cout << "\tSignal strength: " << WiFi.RSSI() << " dB" << std::endl;
}

void Communications::handleServer()
{
    mRunServer = true;
    WiFiClient client;
    string receivedMessage;
    string parsedData;
    
    std::cout << "Running server" << std::endl;
    while(mRunServer)
    {
        client = mServer.available();

        if(client)
        {
            while(client.connected())
            {
                if(client.available())
                {
                    receivedMessage = readMessage(client);
                    std::cout << "Received " << receivedMessage << std::endl;

                    // DO SOMETHING WITH IT
                }
            }
        }
        else
        {
            if(!mTasksQueue.empty())
            {
                switch(mTasksQueue.pop())
                {
                    case 
                }
            }
        }

    }

}

void Communications::sendDataLoop()
{
    mCollectingDataFlag = true;
    while(!mQueue.empty() && !mCollectingDataFlag)
    {
        parsedData = parseData(mQueue->pop());
        sendMessage(parseData);
    }
}

string Communications::readMessage(WiFiClient client)
{
    return client.read();
};

string Communications::parseData(std::vector<int32_t> &dataVector)
{

}

int Communications::sendMessage(string message)
{
    return mServer.write(message.c_str());
};

int Communications::sendDataOnQueue(string dataLocation, int numberSamples)
{
    for(size_t i=0; i < numberSamples; i++)
    {

    }
}

int Communications::getStatus()
{
    mStatus = WiFi.status();
    return mStatus;
}

void Communications::stopServer()
{
    mRunServer = false;
    std::cout << "Stopping server" << std::endl;
}

void Communications::updataDataCollectionFlag(bool newStatus)
{
    mCollectingDataFlag = newStatus;
}

void Communications::parseMessage(string message)
{

}