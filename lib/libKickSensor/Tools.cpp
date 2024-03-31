#include <stdio.h>
#include "tools.h"

char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
    char tmp_str[7]; // temporary variable used in convert function

    sprintf(tmp_str, "%6d", i);
    return tmp_str;
};

// template<typename T>
// void printVector(const std::vector<T> &inputVector)
// {
//     typename std::vector<T>::iterator i = inputVector.begin();
//     for(i; i != inputVector.end(); i++)
//     {
//         std::cout << *i << ' ';
//     }
// }