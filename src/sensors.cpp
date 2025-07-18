/*
 * Sensors Implementation
 * Contains all sensor reading and input processing functions
 */

 #include "sensors.h"
 /*
  * Read analog temperature sensor
  * Converts ADC reading to approximate temperature
  */
 void readAnalogSensors() {
   unsigned long currentTime = millis();
   
   if (currentTime - sensors.tempLastRead >= TEMP_READ_INTERVAL) {
     int adcValue = analogRead(TEMP_SENSOR_PIN);
     // Convert to approximate temperature
     sensors.temperature = 1 / (log(1 / (1023. / adcValue - 1)) / 3950 + 1.0 / 298.15) - 273.15; // 3950 is BETA coefficient of thermistor
     sensors.gas = analogRead(GAS_A_PIN);
     sensors.tempLastRead = currentTime;

     Serial.print("SENSOR: Temperature = ");
     Serial.print(sensors.temperature);
     Serial.print("°C; Gas = ");
     Serial.println(sensors.gas);
   }
 }
 
 /*
  * Process serial commands
  * Handles system arming/disarming
  */
 void processSerialCommands() {
   if (Serial.available()) {
     String command = Serial.readString();
     command.trim();
     command.toUpperCase();
     
     if (command == "ARM") {
       systemFlags.armed = true;
       currentState = MONITORING;
       Serial.println("SYSTEM: Armed - Monitoring mode active");
     }
     else if (command == "DISARM") {
       systemFlags.armed = false;
       systemFlags.alarmActive = false;
       currentState = IDLE;
       digitalWrite(ALARM_LED_PIN, LOW);
       digitalWrite(BUZZER_PIN, LOW);
       Serial.println("SYSTEM: Disarmed - Idle mode");
     }
     else if (command == "STATUS") {
       printSystemStatus();
     }
     else {
       Serial.println("SYSTEM: Unknown command. Use ARM, DISARM, or STATUS");
     }
   }
 }