/*
 * Utilities header declares utility and helper functions
 */

 #ifndef UTILITIES_H
 #define UTILITIES_H
 
 #include "system_config.h"
 
 String stateToString(SystemState state);
 void printSystemStatus();
 void periodicStatusUpdate();
 
 #endif // UTILITIES_H