/*
 * State Machine implementation contains all state machine logic and coordination functions
 */

 #include "state_machine.h"
 #include "system_config.h"

 /*
  * Main state machine processor
  * Coordinates system responses based on sensor inputs and current state
  */
 void processStateMachine() {
  SystemState desiredState = currentState;
  unsigned long currentTime = millis();
   
   switch (currentState) {
     case IDLE:
       // System disarmed - only monitor for commands
       if (systemFlags.armed) {
         desiredState = MONITORING;
       }
       break;
       
     case MONITORING:
       // System armed - monitor for interrupts
       if (!systemFlags.armed) {
        desiredState = IDLE;
      }
      else if (sensors.pir || 
               (sensors.gasReading > GAS_WARNING && sensors.gasSafe) ||
               sensors.temperature > TEMP_HIGH_WARNING ||
               sensors.temperature < TEMP_LOW_WARNING) {
        desiredState = ALERT;
      }
      break;
       
     case ALERT:
       // Brief alert state before full alarm
       if (!systemFlags.armed) {
        desiredState = IDLE;
      }
      else if (shouldEscalateToAlarm()) {
        desiredState = ALARM;
      }
      else if (alertConditionsCleared()) {
        desiredState = MONITORING;
      }
      // Stay in ALERT if conditions persist but haven't escalated
      break;
       
     case ALARM:
       // Full alarm state
       if (!systemFlags.armed) {
        desiredState = IDLE;
        systemFlags.alarmActive = false;
      }
      else if (currentTime - systemFlags.alarmStartTime > ALARM_TIMEOUT) {
        desiredState = MONITORING;
        systemFlags.alarmActive = false;
        LOG_NORMAL("STATE: Alarm timeout - Returning to monitoring");
      }
      break;
   }
   
   // Handle state debouncing
  if (desiredState != currentState) {
    if (pendingState != desiredState) {
      // New state change request
      pendingState = desiredState;
      stateChangeTime = currentTime;
      LOG_VERBOSE("STATE: Change requested to " + stateToString(desiredState) + " - debouncing...");
    }
    else if (currentTime - stateChangeTime >= getStateDebounceTime(desiredState)) {
      // State has been stable long enough, commit the change
      executeStateTransition(desiredState);
      pendingState = currentState; // Reset pending state
    }
  }
  else {
    // Current conditions match current state, reset pending
    pendingState = currentState;
  }
}

/*
 * Get debounce time for different state transitions
 */
unsigned long getStateDebounceTime(SystemState targetState) {
  switch (targetState) {
    case ALARM:
      return ALERT_TO_ALARM_DELAY; // Longer delay before full alarm
    case IDLE:
      return 500; // Quick disarm
    case MONITORING:
      return 1000; // Medium delay for monitoring
    case ALERT:
      return STATE_DEBOUNCE_DELAY; // Standard debounce
    default:
      return STATE_DEBOUNCE_DELAY;
  }
}

/*
 * Check if conditions warrant escalation to full alarm
 */
bool shouldEscalateToAlarm() {
  // Multiple sensor triggers or critical conditions
  bool multipleSensors = false;
  bool criticalCondition = false;
  
  int activeSensors = 0;
  if (sensors.pir) activeSensors++;
  if (sensors.gasReading > GAS_WARNING) activeSensors++;
  if (sensors.temperature > TEMP_HIGH_WARNING || sensors.temperature < TEMP_LOW_WARNING) activeSensors++;
  
  multipleSensors = (activeSensors >= 2);
  criticalCondition = (sensors.pir && !sensors.gasSafe); // Motion + gas danger
  
  return multipleSensors || criticalCondition;
}

/*
 * Check if alert conditions have been cleared
 */
bool alertConditionsCleared() {
  return !sensors.pir && 
         sensors.gasSafe && 
         sensors.gasReading <= GAS_WARNING &&
         sensors.temperature <= TEMP_HIGH_WARNING &&
         sensors.temperature >= TEMP_LOW_WARNING;
}

/*
 * Execute state transition with logging and actions
 */
void executeStateTransition(SystemState newState) {
  if (newState == currentState) return;
  
  // Log state transition (level depends on importance)
  if (newState == ALARM || currentState == ALARM) {
    LOG_MINIMAL("STATE: " + stateToString(currentState) + " -> " + stateToString(newState));
  } else {
    LOG_NORMAL("STATE: " + stateToString(currentState) + " -> " + stateToString(newState));
  }
  
  // Log specific trigger conditions for alerts/alarms
  if (newState == ALERT || newState == ALARM) {
    logTriggerConditions();
  }
  
  previousState = currentState;
  currentState = newState;
  systemFlags.lastStateChange = millis();
  
  // Execute state entry actions
  executeStateActions();
}

/*
 * Log what conditions triggered the state change
 */
void logTriggerConditions() {
  if (!systemFlags.verboseLogging && systemFlags.logLevel < 1) return;
  
  String triggers = "TRIGGERS: ";
  bool hasAny = false;
  
  if (sensors.pir) {
    triggers += "Motion ";
    hasAny = true;
  }
  if (!sensors.gasSafe) {
    triggers += "GasDanger ";
    hasAny = true;
  }
  if (sensors.gasReading > GAS_WARNING) {
    triggers += "GasHigh ";
    hasAny = true;
  }
  if (sensors.temperature > TEMP_HIGH_WARNING) {
    triggers += "TempHigh ";
    hasAny = true;
  }
  if (sensors.temperature < TEMP_LOW_WARNING) {
    triggers += "TempLow ";
    hasAny = true;
  }
  
  if (hasAny) {
    LOG_NORMAL(triggers);
  }
}

/*
 * Execute actions when entering a new state
 */
void executeStateActions() {
  switch (currentState) {
    case IDLE:
      digitalWrite(ALARM_LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      break;
      
    case MONITORING:
      digitalWrite(ALARM_LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      break;
      
    case ALERT:
      // Flash handled in timer interrupt
      break;
      
    case ALARM:
      digitalWrite(ALARM_LED_PIN, HIGH);
      systemFlags.alarmStartTime = millis();
      systemFlags.alarmActive = true;
      break;
  }
}