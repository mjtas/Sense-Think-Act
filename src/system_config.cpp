/*
 * System Configuration Implementation
 * Defines all constants and global variables
 */

 #include "system_config.h"
 #include "interrupts.h"

 // Pin definitions

 const int PIR_SENSOR_PIN = 8;        // PCINT0
 const int TILT_SWTICH_PIN = 9;      // PCINT1
 const int STATUS_LED_PIN = 13;      // Built-in LED
 const int ALARM_LED_PIN = 7;        // External alarm LED
 const int BUZZER_PIN = 6;           // Buzzer for audio alerts
 const int TEMP_SENSOR_PIN = A0;     // Analog temperature sensor
 
 // Timing constants
 
 const unsigned long DEBOUNCE_DELAY = 50;     // ms
 const unsigned long ALARM_TIMEOUT = 10000;   // ms
 const unsigned long TEMP_READ_INTERVAL = 2000; // ms
 const unsigned long SERIAL_UPDATE_INTERVAL = 500; // ms
 
 // Interrupt flags (volatile)
 volatile bool pirDetected = false;
 volatile bool tiltDetected = false;
 volatile bool timerTick = false;
 volatile bool pciTriggered = false;
 
 // State variables
 SystemState currentState = IDLE;
 SystemState previousState = IDLE;
 
 // System data structures
 SensorStates sensors = {false, false, false, false, 0, 0, 0, 0};
 SystemFlags systemFlags = {false, false, false, 0, 0};
 
 // Timing variables
 unsigned long lastSerialUpdate = 0;
 
 // System initialisation
 void systemInit() {
   Serial.begin(115200);
   Serial.println("=== Sense-Think-Act System Initialising ===");
   
   // Configure pins
   pinMode(PIR_SENSOR_PIN, INPUT_PULLUP);
   pinMode(TILT_SWITCH_PIN, INPUT_PULLUP);
   pinMode(STATUS_LED_PIN, OUTPUT);
   pinMode(ALARM_LED_PIN, OUTPUT);
   pinMode(BUZZER_PIN, OUTPUT);
   
   // Initialise outputs
   digitalWrite(STATUS_LED_PIN, LOW);
   digitalWrite(ALARM_LED_PIN, LOW);
   digitalWrite(BUZZER_PIN, LOW);
   
   // Setup interrupts
   setupPinChangeInterrupts();
   setupTimerInterrupt();
   
   // Initialise sensor states
   sensors.pir = digitalRead(PIR_SENSOR_PIN);
   sensors.tilt = digitalRead(TILT_SWITCH_PIN);
   sensors.pirPrevious = sensors.pir;
   sensors.tiltPrevious = sensors.tilt;
   
   // Set initial state
   currentState = IDLE;
   systemFlags.armed = false;
   
   Serial.println("System initialised successfully");
   Serial.println("Send 'ARM' to arm the system, 'DISARM' to disarm");
   Serial.println("===========================================");
 }