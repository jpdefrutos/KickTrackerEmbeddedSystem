#include "libKickSensor.h"
#ifdef ARDUINO_NANO_ESP32
	#include <vector>
	#include <string>
#else
	#include <ArduinoSTL.h>
#endif

#define CORE_0 0
#define CORE_1 1

Communicator* comms_manager;

WiFiClient client;
WiFiUDP UDPSocket;

bool networkRunning = false;
bool socketRunning = false;

static char sendBuffer[1024] = "";
static char rcvBuffer[1024] = "";

IPAddress UDPIP;
IPAddress BroadcastIP;
IPAddress SensorBoardIP = IPAddress(192, 168, 4, 1);

int UDPPort = 3000;
int DestPort = 3500;

enum BOARD_STATE_VALUES {
    START,
    COMMS_OPEN,
    WAIT_FOR_START_ACQ,
    FORWARD_DATA
};

int BOARD_STATE = START;

AccelSensor *sensor;

ESP_EVENT_DEFINE_BASE(COMMUNICATOR_EVENTS);

static esp_event_handler_instance_t s_instance;
TaskHandle_t dataCollectionTaskHandle;

typedef struct{
    int timeout;
    QueueHandle_t dataQueue;
} THREAD_TASK_ARGS;

THREAD_TASK_ARGS taskArgs;

void ReadSensorTask(void* params)
{
    THREAD_TASK_ARGS args = *((THREAD_TASK_ARGS*) params);
    std::vector<int32_t> dataVector;
    if(args.timeout)
    {
        sensor->continuousReading(&dataVector, args.timeout, 100);
    }
    else{
        sensor->readSensor(&dataVector);
    }

    if(uxQueueSpacesAvailable(args.dataQueue) > 0)
    {
        int ret = xQueueSend(args.dataQueue, (void*) &dataVector, 0);
        if(ret == errQUEUE_FULL)
        {
            Serial.println("Data queue is full!");
        }
    }
}

void startDataCollectionTask(void* handler_args, esp_event_base_t base, int32_t id, void* eventData)
{
    BOARD_STATE = START_ACQUISITION;
    THREAD_TASK_ARGS args = *((THREAD_TASK_ARGS*) handler_args);
    args.timeout = *((int*) eventData);
    xTaskCreatePinnedToCore(
        ReadSensorTask
        , "Read sensor"
        , 2048
        , (void*) &args
        , 1
        , &dataCollectionTaskHandle
        , CORE_1
    );

}

void stopDataCollectionTask(void* handler_args, esp_event_base_t base, int32_t id, void* eventData)
{
    BOARD_STATE = COMMS_OPEN;
    vTaskDelete(dataCollectionTaskHandle);
    dataCollectionTaskHandle = NULL;
}


esp_event_loop_handle_t eventLoop;


std::vector<int32_t> dataSample;
Task taskSample;
QueueHandle_t DataQueue = xQueueCreate(1024, sizeof(dataSample));
QueueHandle_t TasksQueue = xQueueCreate(1024, sizeof(taskSample));

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting system");
    if(esp_event_loop_create_default() != ESP_OK)
    {
        while (1)
        {
            Serial.println("Failed to create event loop");
        } 
    }
    Serial.println("Event loop created");

    comms_manager = new Communicator(WIFI_SSID, "", SensorBoardIP, 3000, &DataQueue, &TasksQueue);
    Serial.println("Communicator created");
    // if (!WiFi.softAP(WIFI_SSID)) {
    //     log_e("Soft AP creation failed.");
    //     while(1);
    //     Serial.printf(".");
    // }
    // networkRunning = true;
    // do
	// {
	//     networkRunning = WiFi.softAP("TESTWIFI", "AAAAA");
	// 	delay(500);
	// 	Serial.printf(".");
	// } while (!networkRunning);// && noRetries < MAX_RETRY);
    	
    if(comms_manager->isNetworkRunning())
    {
        Serial.println("WiFi network stablished");

        // UDPIP = WiFi.softAPIP();
        // BroadcastIP = ~WiFi.subnetMask() | WiFi.gatewayIP();
        // server.begin();      
    }
    Serial.println("WiFi network open");

    taskArgs.dataQueue = DataQueue;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(COMMUNICATOR_EVENTS, COMMUNICATOR_EVENT_START_ACQUISITION, startDataCollectionTask, (void *) &taskArgs, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(COMMUNICATOR_EVENTS, COMMUNICATOR_EVENT_STOP_ACQUISITION, stopDataCollectionTask, NULL, NULL));
    Serial.println("Event handlers created");

}

void loop()
{
    // Serial.printf("Board information: %s", UDPIP.toString());
    // Serial.printf(" (%s)\n", BroadcastIP.toString());
    Serial.println("AA");
    if(comms_manager->isNetworkRunning())
    {
        switch (BOARD_STATE)
        {
        case START:
            if(!comms_manager->broadcastMessage())
            {
                Serial.println("Desktop app was not found. Retrying in 5 seconds");
                delay(5000);
            }
            else
            {
                Serial.println("Found the desktop app!");
                BOARD_STATE = COMMS_OPEN;
                comms_manager->handleCommunication();

                comms_manager->sendMessage("0;0;;");
                Serial.printf("Sent back %s\n", "0;0;;");
            }
            Serial.println("START -> COMMS ACTIVE");
            break;

        default:
            comms_manager->handleCommunication();
            break;
        }       

    }
    else
    {
        Serial.println("Network not available!");
        BOARD_STATE = START;
    }
}