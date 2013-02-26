/*
 * Author: GAMMa
 */
 
#include <sstream>
 
#ifndef _DEBUG_UTILITY_
#define _DEBUG_UTILITY_

#define LOG(message) {std::stringstream ss; message << ss; std::cout << "LOG: " << message.str():}

#endif