/*
 * Interrupts Header includes declarations for interrupt setup and handling
 */

 #ifndef INTERRUPTS_H
 #define INTERRUPTS_H
 
 #include "system_config.h"
 #include "sensors.h"
 
 // Interrupt setup
 void setupPinChangeInterrupts();
 void setupTimerInterrupt();
 
 // Interrupt processing
 void processPCIEvents();
 void processTimerEvents();
 
 #endif // INTERRUPTS_H