#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define LEDR 46
#define LEDG 45
#define LEDB 0

#ifdef LED_BUILTIN
#undef LED_BUILTIN
#define LED_BUILTIN 48
#endif

#define MSG_DELIMITER ';'
#define PARAM_DELIMITER ':'

#endif  //_CONSTANTS_H_