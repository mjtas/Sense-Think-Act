/*
 * State Machine header declares state machine and logic processing functions
 */

 #ifndef STATE_MACHINE_H
 #define STATE_MACHINE_H
 
 #include "system_config.h"
 
 void processStateMachine();
 void executeStateActions();
 void executeStateTransition(SystemState state);
 void logTriggerConditions();
 bool shouldEscalateToAlarm();
 bool alertConditionsCleared();
 unsigned long getStateDebounceTime(SystemState state); 
 
 #endif // STATE_MACHINE_H