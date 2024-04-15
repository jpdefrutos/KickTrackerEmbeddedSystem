#include <Arduino.h>
#ifdef ARDUINO_NANO_ESP32
	#include <vector>
	#include <string>
#else
	#include <ArduinoSTL.h>
#endif
#ifndef _TOOLS_
#define _TOOLS_    

char* convert_int16_to_str(int16_t i);
// template<typename T>
// void printVector(const std::vector<T> &inputVector);
#endif