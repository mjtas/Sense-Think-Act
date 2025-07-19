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
  Serial.print("State: "); Serial.print(stateToString(currentState));
  
  // Show pending state if different
  if (pendingState != currentState) {
    Serial.print(" (Pending: "); Serial.print(stateToString(pendingState)); Serial.print(")");
  }
  Serial.println();
  
  Serial.print("Armed: "); Serial.println(systemFlags.armed ? "YES" : "NO");
  Serial.print("Alarm Active: "); Serial.println(systemFlags.alarmActive ? "YES" : "NO");
  Serial.print("Log Level: ");
  switch (systemFlags.logLevel) {
    case 0: Serial.println("QUIET (0)"); break;
    case 1: Serial.println("NORMAL (1)"); break;
    case 2: Serial.println("VERBOSE (2)"); break;
    default: Serial.println("UNKNOWN"); break;
  }
  
  Serial.println("--- Sensors ---");
  Serial.print("Motion: "); Serial.print(sensors.pir ? "ACTIVE" : "INACTIVE");
  Serial.print(" (Last change: "); Serial.print((millis() - sensors.pirLastChange) / 1000); Serial.println("s ago)");
  
  Serial.print("Gas Alert: "); Serial.print(sensors.gasSafe ? "SAFE" : "DANGER");
  Serial.print(" (Last change: "); Serial.print((millis() - sensors.gasLastChange) / 1000); Serial.println("s ago)");
  
  Serial.print("Temperature: "); Serial.print(sensors.temperature, 1); Serial.print("°C");
  if (sensors.temperature > TEMP_HIGH_WARNING) {
    Serial.print(" [HIGH WARNING]");
  } else if (sensors.temperature < TEMP_LOW_WARNING) {
    Serial.print(" [LOW WARNING]");
  }
  Serial.println();
  
  Serial.print("Gas Level: "); Serial.print(sensors.gasReading);
  if (sensors.gasReading > GAS_WARNING) {
    Serial.print(" [WARNING]");
  }
  Serial.println();
  
  Serial.println("--- System Info ---");
  Serial.print("Uptime: "); 
  unsigned long uptime = millis() / 1000;
  Serial.print(uptime / 3600); Serial.print("h ");
  Serial.print((uptime % 3600) / 60); Serial.print("m ");
  Serial.print(uptime % 60); Serial.println("s");
  
  Serial.print("Time in State: "); 
  unsigned long stateTime = (millis() - systemFlags.lastStateChange) / 1000;
  Serial.print(stateTime); Serial.println("s");
  
  if (systemFlags.alarmActive) {
    unsigned long alarmTime = (millis() - systemFlags.alarmStartTime) / 1000;
    Serial.print("Alarm Duration: "); Serial.print(alarmTime); Serial.println("s");
  }
  
  Serial.println("====================\n");
}
 
 /*
  * Provide periodic status updates
  */
 void periodicStatusUpdate() {
   unsigned long currentTime = millis();
   
   if (currentTime - lastSerialUpdate >= SERIAL_UPDATE_INTERVAL) {
    bool shouldUpdate = false;
    
    if (systemFlags.verboseLogging) {
      shouldUpdate = true; // Always update in verbose mode
    } else if (currentState == ALARM) {
      shouldUpdate = true; // Always update during alarms
    } else if (currentState == ALERT) {
      shouldUpdate = true; // Update during alerts
    } else if (currentState == MONITORING && systemFlags.logLevel >= 1) {
      // Occasional updates while monitoring
      static int updateCounter = 0;
      updateCounter++;
      if (updateCounter >= 6) { // Every 30 seconds (6 * 5 second intervals)
        shouldUpdate = true;
        updateCounter = 0;
      }
    }
    
    if (shouldUpdate) {
       Serial.print("STATUS: ");
       Serial.print(stateToString(currentState));
       Serial.print(" | Motion: ");
       Serial.print(sensors.pir ? "1" : "0");
       Serial.print(" | Gas Danger: ");
       Serial.print(sensors.gasSafe ? "0" : "1");
       Serial.print(" | Temp: ");
       Serial.print(sensors.temperature);
       Serial.print("°C | Gas Reading: ");
       Serial.println(sensors.gasReading);
     }
     lastSerialUpdate = currentTime;
   }
 }