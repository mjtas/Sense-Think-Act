/*
 * System Configuration Implementation
 * Defines all constants and global variables
 */

 #include "system_config.h"
 #include "interrupts.h"

 // Pin definitions

 const int PIR_SENSOR_PIN = 8;       // PCINT0
 const int GAS_D_PIN = 9;          // PCINT1
 const int STATUS_LED_PIN = 13;      // Built-in LED
 const int ALARM_LED_PIN = 7;        // External alarm LED
 const int BUZZER_PIN = 6;           // Buzzer for audio alerts
 const int TEMP_SENSOR_PIN = A0;     // Analog temperature sensor
 const int GAS_A_PIN = A1;         // Analog gas output
 
 // Timing constants
 const unsigned long DEBOUNCE_DELAY = 50;     // ms
 const unsigned long STATE_DEBOUNCE_DELAY = 2000; // ms
 const unsigned long ALARM_TIMEOUT = 10000;   // ms
 const unsigned long TEMP_READ_INTERVAL = 2000; // ms
 const unsigned long SERIAL_UPDATE_INTERVAL = 5000; // ms
 const unsigned long ALERT_TO_ALARM_DELAY = 3000; // ms
 
 // Threshold constants
 const long GAS_WARNING = 500; // ppm
 const long TEMP_LOW_WARNING = 15; // degrees celsius
 const long TEMP_HIGH_WARNING = 30; // degrees celsius

 // Interrupt flags (volatile)
 volatile bool pirChange = false;
 volatile bool gasChange = false;
 volatile bool timerTick = false;
 volatile bool pciTriggered = false;
 
 // State variables
 SystemState currentState = IDLE;
 SystemState previousState = IDLE;
 SystemState pendingState = IDLE; // for state debouncing
 unsigned long stateChangeTime = 0; // track when state change was initiated

 
 // System data structures
 SensorStates sensors = {false, true, false, false, 0, 0, 0, 0, 0};
 SystemFlags systemFlags = {false, false, false, 0, 0, false, 1};
 
 // Timing variables
 unsigned long lastSerialUpdate = 0;
 
 // System initialisation
 void systemInit() {
   Serial.begin(115200);
   Serial.println("=== Home Monitoring System Initialising ===");
   
   // Configure pins
   pinMode(PIR_SENSOR_PIN, INPUT_PULLUP);
   pinMode(GAS_D_PIN, INPUT_PULLUP);
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
   sensors.gasSafe = digitalRead(GAS_D_PIN);
   sensors.pirPrevious = sensors.pir;
   sensors.gasPrevious = sensors.gasSafe;
   
   // Set initial state
   currentState = IDLE;
   systemFlags.armed = false;
   systemFlags.verboseLogging = true;
   systemFlags.logLevel = 1;
   
   Serial.println("System initialised successfully");
   Serial.println("Commands: ARM, DISARM, STATUS, VERBOSE, QUIET, DEBUG");
   Serial.println("===========================================");
 }