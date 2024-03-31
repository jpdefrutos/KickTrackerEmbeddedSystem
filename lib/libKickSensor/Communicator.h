#include <ArduinoSTL.h>
#include <WiFi.h>



class Communicator
{
public:
    Communicator(string ssid, string psw);
    void openServer(string ip, int port);
    int readMessage();
    int sendMessage();

private:
    const int mBufferSize = 32;
    const int mMaxRetry = 100;
};