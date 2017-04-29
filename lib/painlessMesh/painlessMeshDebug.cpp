//
//  painlessMeshDebug.cpp
//
//
//  Created by Bill Gray on 8/18/16.
//
//

#include <Arduino.h>
#include <stdarg.h>

#include "painlessMesh.h"

uint16_t types = 0;

void painlessMesh::initDebugPort(uint32_t speed) {
  DEBUG_PORT.begin(speed);
}

void painlessMesh::setDebugMsgTypes(uint16_t newTypes) {
    // set the different kinds of debug messages you want to generate.
    types = newTypes;
    DEBUG_PORT.printf("\nsetDebugTypes 0x%x\n", types);
}

// To assign a debug message to several type use | (bitwise or) operator
// Example: debugMsg( GENERAL | CONNECTION , "Debug message");
void painlessMesh::debugMsg(debugType type, const char* format ...) {

    if (type & types) {  //Print only the message types set for output
        char str[200];

        va_list args;
        va_start(args, format);

        vsnprintf(str, sizeof(str), format, args);

        if (types && MSG_TYPES)
            DEBUG_PORT.printf("0x%x\t", type, types);

        DEBUG_PORT.print(str);

        va_end(args);
    }
}
