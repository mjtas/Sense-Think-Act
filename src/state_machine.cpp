/*
 * State Machine implementation contains all state machine logic and coordination functions
 */

 #include "state_machine.h"

 /*
  * Main state machine processor
  * Coordinates system responses based on sensor inputs and current state
  */
 void processStateMachine() {
   SystemState nextState = currentState;
   
   switch (currentState) {
     case IDLE:
       // System disarmed - only monitor for commands
       if (systemFlags.armed) {
         nextState = MONITORING;
       }
       break;
       
     case MONITORING:
       // System armed - monitor for intrusions
       if (!systemFlags.armed) {
         nextState = IDLE;
       }
       else if (sensors.pir) {
         nextState = ALERT;
         Serial.println("STATE: Intrusion detected - Alert mode");
       }
       else if (!sensors.gasSafe) {
        nextState = ALERT;
        Serial.println("STATE: Dangerous gas levels - Alert mode");
       }
       break;
       
     case ALERT:
       // Brief alert state before full alarm
       if (!systemFlags.armed) {
         nextState = IDLE;
       }
       else if (sensors.pir && !sensors.gasSafe) {
         nextState = ALARM;
         systemFlags.alarmStartTime = millis();
         systemFlags.alarmActive = true;
         Serial.println("STATE: Multiple sensors triggered - Alarm mode");
       }
       else if (!sensors.pir && sensors.gasSafe) {
         nextState = MONITORING;
         Serial.println("STATE: Sensors clear - Returning to monitoring");
       }
       break;
       
     case ALARM:
       // Full alarm state
       if (!systemFlags.armed) {
         nextState = IDLE;
         systemFlags.alarmActive = false;
       }
       else if (millis() - systemFlags.alarmStartTime > ALARM_TIMEOUT) {
         nextState = MONITORING;
         systemFlags.alarmActive = false;
         Serial.println("STATE: Alarm timeout - Returning to monitoring");
       }
       break;
   }
   
   // State transition logging
   if (nextState != currentState) {
     Serial.print("STATE: Transition from ");
     Serial.print(stateToString(currentState));
     Serial.print(" to ");
     Serial.println(stateToString(nextState));
     
     previousState = currentState;
     currentState = nextState;
     
     // Execute state entry actions
     executeStateActions();
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
       // Flash alarm LED
       digitalWrite(ALARM_LED_PIN, HIGH);
       millis();
       digitalWrite(ALARM_LED_PIN, LOW);
       break;
       
     case ALARM:
       digitalWrite(ALARM_LED_PIN, HIGH);
       digitalWrite(BUZZER_PIN, HIGH);
       break;
   }
 }