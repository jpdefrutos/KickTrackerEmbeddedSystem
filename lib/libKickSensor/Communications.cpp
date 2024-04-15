#include "Communications.h"


Communicator::Communicator(String ssid, String psw, String ip, int port)
{
    // ssid, psw, ip, port should be saved in a config file and read upon power up
    std::cout << "Connecting to " << ssid ;
    int retries = 3;
    do
    {
        std::cout << ".";
        mStatus = WiFi.begin(ssid.c_str(), psw.c_str());
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


void Communicator::setQueue(std::queue<String>* queue)
{
    mQueue = queue;
}


void Communicator::openServer(String ip, int port)
{
    mServer.begin();
    WiFi.config(ip);
    mServer = WiFiServer(port);

    std::cout << "Connected to " << WiFi.SSID() << std::endl;
    std::cout << "\tIP: " << WiFi.localIP() << "\tPort: " << port << std::endl;
    std::cout << "\tSignal strength: " << WiFi.RSSI() << " dB" << std::endl;
}

void Communicator::handleServer()
{
    mRunServer = true;
    WiFiClient client;
    String receivedMessage;
    String parsedData;
    
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

void Communicator::sendDataLoop()
{
    mCollectingDataFlag = true;
    while(!mQueue.empty() && !mCollectingDataFlag)
    {
        parsedData = parseData(mQueue->pop());
        sendMessage(parseData);
    }
}

String Communicator::readMessage(WiFiClient client)
{
    return client.read();
};

String Communicator::parseData(std::vector<int32_t> &dataVector)
{

}

int Communicator::sendMessage(String message)
{
    return mServer.write(message.c_str());
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