# Sense-Think-Act
This project implements a comprehensive Sense-Think-Act (STA) architecture on Arduino, designed as a security monitoring system that demonstrates advanced interrupt handling, state machine design, and modular programming practices.  It is organised into multiple files for better maintainability and code organisation.

## System Architecture
### Sense-Think-Act Pattern
- SENSE: Multiple digital sensors monitored via Pin Change Interrupts (PCI)
- THINK: State machine processing with coordinated logic and system flags
- ACT: Coordinated output control for LEDs, buzzer, and status indicators

### Key Features
- Event-driven sensing using Pin Change Interrupts for immediate response
- Time-based processing using Timer1 interrupts for periodic tasks
- State machine coordination with proper separation of concerns
- Interrupt-safe programming with minimal ISR processing
- Comprehensive logging for system traceability

### Components
- Arduino Uno
- 2 x Digital sensors (PIR detector for motion, flex sensor for door or window)
- 2 x LEDs (status and alarm indicators)
- 1 x Piezo speaker
- 1x Analog temperature sensor (TMP36)
- Resistors and breadboard for connections

#### Pin Configuration
- Motion Sensor  → Pin D8  (PCINT0)
- Door Sensor    → Pin D9  (PCINT1)
- Status LED     → Pin D13 (Built-in LED)
- Alarm LED      → Pin D7
- Buzzer         → Pin D6
- Temperature    → Pin A0  (Analog)

## Software Architecture
### Interrupt System
Pin Change Interrupt (PCINT0_vect)
- Monitors pins D8-D13 group
- Specifically configured for D8 (pir) and D9 (flex)
- Sets flags for main loop processing
- Includes basic debouncing logic


Timer1 Interrupt (TIMER1_COMPA_vect)
- 1-second periodic interrupt
- Handles status LED blinking
- Triggers temperature readings
- Provides system heartbeat

Interrupt Safety Measures
- All ISRs use minimal processing
- Volatile variables for interrupt-shared data
- No delay() functions in interrupt contexts
- Flag-based communication between ISRs and main loop

### State Machine
The system operates through four states:
- IDLE: System disarmed, minimal monitoring
- MONITORING: Armed and watching for intrusions
- ALERT: Single sensor triggered, brief warning state
- ALARM: Multiple sensors or sustained alert condition

### Modular Function Design
- Initialisation: systemInit(), setupPinChangeInterrupts(), setupTimerInterrupt()
- Sensing: processPCIEvents(), readTemperatureSensor(), processSerialCommands()
- Thinking: processStateMachine(), processTimerEvents(), executeStateActions()
- Acting: updateSystemOutputs()
- Monitoring: printSystemStatus(), periodicStatusUpdate()

## File Descriptions
### Core Files
main.cpp
- Main Arduino sketch with setup() and loop() functions
- Includes all other modules
- Implements the main Sense-Think-Act control loop

system_config.h/cpp
- Global constants and pin definitions
- Data structures (SystemState, SensorStates, SystemFlags)
- Global variable declarations and definitions
- System initialisation function

### Functional Modules
interrupts.h/cpp
- Pin Change Interrupt (PCI) setup and handling
- Timer interrupt configuration
- Interrupt Service Routines (ISRs)
- Interrupt event processing functions

sensors.h/cpp
- Temperature sensor reading
- Serial command processing
- Input validation and processing

state_machine.h/cpp
- Main state machine logic
- State transition handling
- State entry/exit actions
- System coordination logic

actuators.h/cpp
- Output control functions
- LED and buzzer management
- Output state coordination

utilities.h/cpp
- Helper functions
- Status reporting
- String conversion utilities
- System monitoring functions

## Setup Instructions
1. Hardware Assembly
Motion Sensor (PIR):
- VCC → 5V
- GND → GND
- OUT → Pin D8

Door Sensor (Tilt Switch):
- OUT → Pin D9
- VCC → 5V 
- GND → GND

Status LED:
- Uses built-in LED on Pin D13

Alarm LED:
- Anode → Pin D7
- Cathode → GND (through 220Ω resistor)

Piezo Speaker:
- Positive → Pin D6
- Negative → GND

Temperature Sensor (TMP36):
- VCC → 5V
- GND → GND
- VOUT → Pin A0

## Operation Guide
Serial Commands
- ARM: Activate security monitoring
- DISARM: Deactivate system and clear alarms
- STATUS: Display comprehensive system status

System Behavior
- Startup: System initialixes in IDLE state
- Arming: Send ARM command to enter MONITORING state
- Detection: Sensor triggers cause state transitions
- Alerting: Visual and audio feedback based on threat level
- Disarming: DISARM command returns to safe state

Status Indicators
- Status LED: Blinks every second when system is active
- Alarm LED: Steady on during alarm, flashing during alert
- Buzzer: Sounds during full alarm condition
- Serial Output: Continuous logging of all system changes