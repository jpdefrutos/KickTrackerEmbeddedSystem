#include <stdlib.h>
#include <Arduino.h>
#include <time.h>
#ifdef ARDUINO_NANO_ESP32
	#include <vector>
	#include <string>
#else
	#include <ArduinoSTL.h>
#endif

#ifndef _LOGGER_H_
#define _LOGGER_H_

#define CLASSNAME "Logger"
#define LOG_BUFFER_SIZE 1024


enum LOGSEV
{
    INFORMATION,
    WARNING,
    ERROR,
    DEBUG,
};

struct eventLog {
    LOGSEV severity;
    int time;
    char* className;
    char* methodName;
    char* message;
};

class Logger
{
    public:
        Logger();
        ~Logger();

        void beginSerial(int br=9600);
        void logEvent(LOGSEV sev, const char* className, const char* methodName, const char* message);
        Logger* getLogger();

    private:
        char* getSeverityString(LOGSEV sev);

        char* mFilePath;

};

extern Logger* GlobalLogger;

#endif //_LOGGER_H_