#include "Logger.h"

static char* LOGBUFFER;

Logger::Logger()
{
    beginSerial(9600);
    LOGBUFFER = (char*)malloc(LOG_BUFFER_SIZE);
    memset(LOGBUFFER, '\0', LOG_BUFFER_SIZE);
};

Logger::~Logger()
{};

void Logger::beginSerial(int br)
{
    Serial.begin(br);
};

void Logger::logEvent(LOGSEV sev, const char* className, const char* methodName, const char* message)
{
    va_list strValues;

    sprintf(LOGBUFFER, "%u [%s] %s::%s\t%s", millis(), getSeverityString(sev), className, methodName, message);
    std::cout << millis() << "\t[" << getSeverityString(sev) << "]\t" << className << "::" << methodName << "\t" << message << std::endl; 
};

char* Logger::getSeverityString(LOGSEV sev)
{
    char* retVal = "INF";
    switch(sev)
    {
        case WARNING:
            retVal = "WAR";
            break;
        case ERROR:
            retVal = "ERR";
            break;
        case DEBUG:
            retVal = "DEB";
            break;
    };
    return retVal;
};

Logger* Logger::getLogger()
{
    return this;
};