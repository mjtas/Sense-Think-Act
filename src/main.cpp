/*
 main.cpp includes all other components and implements the main setup() and loop() functions
*/

#include <Arduino.h>
#include "system_config.h"
#include "interrupts.h"
#include "sensors.h"
#include "state_machine.h"
#include "actuators.h"
#include "utilities.h"
 
void setup() {
  systemInit();
}
 
void loop() {
  // SENSE: Process all inputs
  processPCIEvents();          // Event-based sensor processing
  processSerialCommands();     // User commands
   
  // THINK: Process state machine and coordination
  processStateMachine();       // Main state logic
  processTimerEvents();        // Time-based processing
   
  // ACT: Update all outputs
  updateSystemOutputs();       // Actuator control
   
  // MONITOR: Provide system feedback
  periodicStatusUpdate();      // Serial monitoring
   
  // Small delay to prevent overwhelming the system
   millis();
}