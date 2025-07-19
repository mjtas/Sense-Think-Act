/*
 * Contains all interrupt-related functions and ISRs
 */

 #include "interrupts.h"
 #include "state_machine.h"

 /*
  * Pin Change Interrupt Service Routine
  * Handles PCI for pins D8-D13 (PCINT0-PCINT5)
  * Sets flags for main loop processing
  */
 ISR(PCINT0_vect) {
   // Set flag for main loop to handle
   pciTriggered = true;
   
   // Read current pin states (quick operation)
   bool currentPIR = digitalRead(PIR_SENSOR_PIN);
   bool currentGas = digitalRead(GAS_D_PIN);
   
   // Set specific flags if state changed
   if (currentPIR != sensors.pir) {
     pirChange = true;
   }
   if (currentGas != sensors.gasSafe) {
     gasChange = true;
   }
 }
 
 /*
  * Timer1 Compare Match Interrupt Service Routine
  * Executes every 1 second for periodic tasks
  */
 ISR(TIMER1_COMPA_vect) {
   timerTick = true;
   systemFlags.statusLedState = !systemFlags.statusLedState;
 }
 
 /*
  * Configure Pin Change Interrupts
  * Sets up PCI for pins D8 and D9 (PCINT0 and PCINT1)
  */
 void setupPinChangeInterrupts() {
   // Enable Pin Change Interrupt for PCINT0-7 (pins D8-D13)
   PCICR |= (1 << PCIE0);
   
   // Enable specific pins: PCINT0 (D8) and PCINT1 (D9)
   PCMSK0 |= (1 << PCINT0) | (1 << PCINT1);
   
   LOG_NORMAL("PCI configured for pins D8 (PCINT0) and D9 (PCINT1)");
 }
 
 /*
  * Configure Timer1 for 1-second interrupts
  * Uses CTC mode with prescaler for accurate timing
  */
 void setupTimerInterrupt() {
   // Disable interrupts during setup
   cli();
   
   // Clear Timer1 registers
   TCCR1A = 0;
   TCCR1B = 0;
   TCNT1 = 0;
   
   // Set compare match value for 1 second interval
   // 16MHz / 1024 prescaler = 15625 Hz
   // For 1 second: 15625 - 1 = 15624
   OCR1A = 15624;
   
   // Configure Timer1 for CTC mode
   TCCR1B |= (1 << WGM12);
   
   // Set prescaler to 1024
   TCCR1B |= (1 << CS12) | (1 << CS10);
   
   // Enable Timer1 compare match interrupt
   TIMSK1 |= (1 << OCIE1A);
   
   // Re-enable interrupts
   sei();
   
   LOG_NORMAL("Timer1 configured for 1-second intervals");
 }
 
 /*
  * Process Pin Change Interrupt events
  * Handles debouncing and state updates
  */
 void processPCIEvents() {
   if (!pciTriggered) return;
   
   unsigned long currentTime = millis();
   bool stateChanged = false;
   
   // Handle motion sensor change
   if (pirChange) {
     if (currentTime - sensors.pirLastChange > DEBOUNCE_DELAY) {
       sensors.pirPrevious = sensors.pir;
       sensors.pir = digitalRead(PIR_SENSOR_PIN);
       sensors.pirLastChange = currentTime;
       stateChanged = true;
       
      // Only log significant changes or in verbose mode
      if (systemFlags.verboseLogging || (sensors.pir && systemFlags.armed)) {
        LOG_NORMAL("SENSOR: PIR detector = " + String(sensors.pir ? "ACTIVE" : "INACTIVE"));
      }
      
      // Always log motion detection when armed
      if (sensors.pir && systemFlags.armed && !systemFlags.verboseLogging) {
        LOG_MINIMAL("ALERT: Motion detected");
      }
    }
    pirChange = false;
  }
   
   // Handle gas change
   if (gasChange) {
     if (currentTime - sensors.gasLastChange > DEBOUNCE_DELAY) {
       sensors.gasPrevious = sensors.gasSafe;
       sensors.gasSafe = digitalRead(GAS_D_PIN);
       sensors.gasLastChange = currentTime;
       stateChanged = true;
       
       // Always log gas safety changes
      LOG_MINIMAL("SENSOR: Gas sensor = " + String(sensors.gasSafe ? "SAFE" : "DANGER"));
    }
    gasChange = false;
  }
   
   pciTriggered = false;
   
   // Trigger state machine update if sensors changed
   if (stateChanged) {
    processStateMachine();
  }
 }
 
 /*
  * Process timer-based events
  * Handles periodic tasks triggered by Timer1 interrupt
  */
 void processTimerEvents() {
   if (!timerTick) return;
   
   // Update status LED
   digitalWrite(STATUS_LED_PIN, systemFlags.statusLedState);
   
   // Read analog sensors
   readAnalogSensors();
   
  static int timerCounter = 0;
  timerCounter++;
  
  // Log every 30 seconds in monitoring mode (every 10 seconds if verbose)
  bool shouldLog = false;
  if (systemFlags.verboseLogging && (timerCounter % 10 == 0)) {
    shouldLog = true;
  } else if (currentState == MONITORING && (timerCounter % 30 == 0)) {
    shouldLog = true;
  }
  
  if (shouldLog) {
    LOG_VERBOSE("TIMER: Periodic check - System operational");
  }
   
   timerTick = false;
 }