/*
 * Utilities implementation contains all utility and helper functions
 */

 #include "utilities.h"

 /*
  * Convert state enum to string for logging
  */
 String stateToString(SystemState state) {
   switch (state) {
     case IDLE: return "IDLE";
     case MONITORING: return "MONITORING";
     case ALERT: return "ALERT";
     case ALARM: return "ALARM";
     default: return "UNKNOWN";
   }
 }
 
 /*
  * Print comprehensive system status
  */
 void printSystemStatus() {
   Serial.println("\n=== SYSTEM STATUS ===");
   Serial.print("State: "); Serial.println(stateToString(currentState));
   Serial.print("Armed: "); Serial.println(systemFlags.armed ? "YES" : "NO");
   Serial.print("Alarm Active: "); Serial.println(systemFlags.alarmActive ? "YES" : "NO");
   Serial.print("Motion Sensor: "); Serial.println(sensors.pir ? "ACTIVE" : "INACTIVE");
   Serial.print("Gas Alert: "); Serial.println(sensors.gasSafe ? "SAFE" : "DANGER");
   Serial.print("Temperature: "); Serial.print(sensors.temperature); Serial.println("°C");
   Serial.print("Gas: "); Serial.println(sensors.gasReading);
   Serial.println("Uptime: "); Serial.print(millis() / 1000); Serial.println(" seconds");
   Serial.println("====================\n");
 }
 
 /*
  * Provide periodic status updates
  */
 void periodicStatusUpdate() {
   unsigned long currentTime = millis();
   
   if (currentTime - lastSerialUpdate >= SERIAL_UPDATE_INTERVAL) {
     if (currentState != IDLE) {
       Serial.print("STATUS: ");
       Serial.print(stateToString(currentState));
       Serial.print(" | Motion: ");
       Serial.print(sensors.pir ? "1" : "0");
       Serial.print(" | Gas Alert: ");
       Serial.print(sensors.gasSafe ? "0" : "1");
       Serial.print(" | Temp: ");
       Serial.print(sensors.temperature);
       Serial.print("°C | Gas: ");
       Serial.println(sensors.gasReading);
     }
     lastSerialUpdate = currentTime;
   }
 }