/*
 * system_config.h contains all constants, pin definitions, and global data structures
 */

 #ifndef SYSTEM_CONFIG_H
 #define SYSTEM_CONFIG_H

 #include <Arduino.h>
 #include <avr/interrupt.h>
 
 // Input pins
 extern const int PIR_SENSOR_PIN;
 extern const int GAS_D_PIN;
 extern const int TEMP_SENSOR_PIN;
 extern const int GAS_A_PIN;
 
 // Output pins
 extern const int STATUS_LED_PIN;
 extern const int ALARM_LED_PIN;
 extern const int BUZZER_PIN;
 
 // Timing constants 
 extern const unsigned long DEBOUNCE_DELAY;
 extern const unsigned long ALARM_TIMEOUT;
 extern const unsigned long TEMP_READ_INTERVAL;
 extern const unsigned long SERIAL_UPDATE_INTERVAL;
 
 // System state enumeration
 enum SystemState {
   IDLE,
   MONITORING,
   ALERT,
   ALARM
 };
 
 // Sensor state structure
 struct SensorStates {
   bool pir;
   bool gasSafe;
   bool pirPrevious;
   bool gasPrevious;
   unsigned long pirLastChange;
   unsigned long gasLastChange;
   int temperature;
   int gasReading;
   unsigned long tempLastRead;
 };
 
 // System flags structure
 struct SystemFlags {
   bool armed;
   bool alarmActive;
   bool statusLedState;
   unsigned long alarmStartTime;
   unsigned long lastStatusUpdate;
 };

 // Interrupt flags (volatile)
 extern volatile bool pirChange;
 extern volatile bool gasChange;
 extern volatile bool timerTick;
 extern volatile bool pciTriggered;
 
 // State variables
 extern SystemState currentState;
 extern SystemState previousState;
 
 // System data structures
 extern SensorStates sensors;
 extern SystemFlags systemFlags;
 
 // Timing variables
 extern unsigned long lastSerialUpdate;
  
 // Initialisation
 void systemInit();
 
 // Utility functions
 String stateToString(SystemState state);
 void printSystemStatus();
 void periodicStatusUpdate();
 
 #endif // SYSTEM_CONFIG_H