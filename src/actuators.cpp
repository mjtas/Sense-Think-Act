/*
 * Actuators implementation contains all output control and actuation functions
 */

 #include "actuators.h"

 /*
  * Update all system outputs based on current state
  * Separates actuation logic from state logic
  */
 void updateSystemOutputs() {
   // Status LED handled in timer interrupt
   
   // Alarm LED
   switch (currentState) {
     case IDLE:
     case MONITORING:
       digitalWrite(ALARM_LED_PIN, LOW);
       break;
     case ALERT:
       // Handled in timer for blinking
       break;
     case ALARM:
       digitalWrite(ALARM_LED_PIN, HIGH);
       break;
   }
   
   // Buzzer
   if (systemFlags.alarmActive && currentState == ALARM) {
     digitalWrite(BUZZER_PIN, HIGH);
   } else {
     digitalWrite(BUZZER_PIN, LOW);
   }
 }