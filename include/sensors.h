/*
 * Sensors Header declares sensor reading and processing functions
 */

 #ifndef SENSORS_H
 #define SENSORS_H
 
 #include "system_config.h"

 void readAnalogSensors();
 void processSerialCommands();
 void printHelpCommands();
 void printDebugInfo();
 int getFreeRAM();

 
 #endif // SENSORS_H