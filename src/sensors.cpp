/*
 * Sensors Implementation
 * Contains all sensor reading and input processing functions
 */

 #include "sensors.h"
 /*
  * Read analog temperature sensor with reduced logging noise
  */
 void readAnalogSensors() {
   unsigned long currentTime = millis();
   
   if (currentTime - sensors.tempLastRead >= TEMP_READ_INTERVAL) {
     int adcValue = analogRead(TEMP_SENSOR_PIN);
     float prevTemp = sensors.temperature;
     int prevGas = sensors.gasReading;
     
     // Convert to approximate temperature
     sensors.temperature = 1 / (log(1 / (1023. / adcValue - 1)) / 3950 + 1.0 / 298.15) - 273.15;
     sensors.gasReading = analogRead(GAS_A_PIN);
     sensors.tempLastRead = currentTime;
 
     // Only log if significant change or verbose mode
     bool significantTempChange = abs(sensors.temperature - prevTemp) > 1.0; // 1°C threshold
     bool significantGasChange = abs(sensors.gasReading - prevGas) > 50; // 50 unit threshold
     
     if (systemFlags.verboseLogging || significantTempChange || significantGasChange) {
       LOG_VERBOSE("SENSOR: Temperature = " + String(sensors.temperature, 1) + "°C; Gas = " + String(sensors.gasReading));
     }
     
     // Always log warnings regardless of log level
     if (sensors.temperature > TEMP_HIGH_WARNING || sensors.temperature < TEMP_LOW_WARNING) {
       LOG_MINIMAL("WARNING: Temperature " + String(sensors.temperature, 1) + "°C outside safe range");
     }
     if (sensors.gasReading > GAS_WARNING) {
       LOG_MINIMAL("WARNING: Gas level " + String(sensors.gasReading) + " above threshold");
     }
   }
 }
 
 /*
  * Enhanced serial command processing with input echoing and new commands
  */
 void processSerialCommands() {
   if (Serial.available()) {
     String command = Serial.readString();
     command.trim();
     
     // Echo user input (before converting to uppercase)
     Serial.print("CMD> ");
     Serial.println(command);
     
     command.toUpperCase();
     
     if (command == "ARM") {
       systemFlags.armed = true;
       currentState = MONITORING;
       pendingState = MONITORING; // Reset pending state
       LOG_MINIMAL("SYSTEM: Armed - Monitoring mode active");
     }
     else if (command == "DISARM") {
       systemFlags.armed = false;
       systemFlags.alarmActive = false;
       currentState = IDLE;
       pendingState = IDLE; // Reset pending state
       digitalWrite(ALARM_LED_PIN, LOW);
       digitalWrite(BUZZER_PIN, LOW);
       LOG_MINIMAL("SYSTEM: Disarmed - Idle mode");
     }
     else if (command == "STATUS") {
       printSystemStatus();
     }
     else if (command == "VERBOSE") {
       systemFlags.verboseLogging = true;
       systemFlags.logLevel = 2;
       Serial.println("SYSTEM: Verbose logging enabled");
     }
     else if (command == "QUIET") {
       systemFlags.verboseLogging = false;
       systemFlags.logLevel = 0;
       Serial.println("SYSTEM: Quiet mode enabled (minimal logging)");
     }
     else if (command == "NORMAL") {
       systemFlags.verboseLogging = false;
       systemFlags.logLevel = 1;
       Serial.println("SYSTEM: Normal logging enabled");
     }
     else if (command == "DEBUG") {
       printDebugInfo();
     }
     else if (command == "HELP") {
       printHelpCommands();
     }
     else {
       Serial.println("ERROR: Unknown command '" + command + "'. Type HELP for available commands.");
     }
   }
 }
 
 /*
  * Print available commands
  */
 void printHelpCommands() {
   Serial.println("\n=== AVAILABLE COMMANDS ===");
   Serial.println("ARM      - Arm the monitoring system");
   Serial.println("DISARM   - Disarm the system and stop alarms");
   Serial.println("STATUS   - Display current system status");
   Serial.println("VERBOSE  - Enable detailed logging");
   Serial.println("NORMAL   - Enable normal logging level");
   Serial.println("QUIET    - Enable minimal logging (warnings only)");
   Serial.println("DEBUG    - Show debug information");
   Serial.println("HELP     - Show this command list");
   Serial.println("==========================\n");
 }
 
 /*
  * Print debug information for troubleshooting
  */
 void printDebugInfo() {
   Serial.println("\n=== DEBUG INFORMATION ===");
   Serial.print("Current State: "); Serial.println(stateToString(currentState));
   Serial.print("Pending State: "); Serial.println(stateToString(pendingState));
   Serial.print("State Change Time: "); Serial.println(stateChangeTime);
   Serial.print("Current Time: "); Serial.println(millis());
   Serial.print("Time in Current State: "); Serial.println(millis() - systemFlags.lastStateChange);
   Serial.print("Log Level: "); Serial.println(systemFlags.logLevel);
   Serial.print("Verbose Logging: "); Serial.println(systemFlags.verboseLogging ? "ON" : "OFF");
   Serial.print("Free RAM: "); Serial.println(getFreeRAM());
   Serial.println("=========================\n");
 }
 
 /*
  * Get free RAM for debugging
  */
 int getFreeRAM() {
   extern int __heap_start, *__brkval;
   int v;
   return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
 }