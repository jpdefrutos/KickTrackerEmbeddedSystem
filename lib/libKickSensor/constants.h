#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define LEDR 46
#define LEDG 45
#define LEDB 0

#ifdef LED_BUILTIN
    #undef LED_BUILTIN
#endif
#define LED_BUILTIN 48

#define MSG_DELIMITER ';'
#define PARAM_DELIMITER ':'

#define WIFI_SSID "KickSensorWiFi"
#define WIFI_PSW "KickSensorLibrary"

#endif  //_CONSTANTS_H_