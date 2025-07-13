# Sense-Think-Act
This project implements a comprehensive Sense-Think-Act (STA) architecture on Arduino, designed as a security monitoring system that demonstrates advanced interrupt handling, state machine design, and modular programming practices.  It is organised into multiple files for better maintainability and code organisation.

## System Architecture
### Sense-Think-Act Pattern
- SENSE: Multiple digital sensors monitored via Pin Change Interrupts (PCI)
- THINK: State machine processing with coordinated logic and system flags
- ACT: Coordinated output control for LEDs, buzzer, and status indicators

## Key Features
- Event-driven sensing using Pin Change Interrupts for immediate response
- Time-based processing using Timer1 interrupts for periodic tasks
- State machine coordination with proper separation of concerns
- Interrupt-safe programming with minimal ISR processing
- Comprehensive logging for system traceability

## Components
- Arduino Uno
- 2 x Digital sensors (PIR detector for motion, flex sensor for door or window)
- 2 x LEDs (status and alarm indicators)
- 1 x Piezo speaker
- 1x Analog temperature sensor (TMP36)
- Resistors and breadboard for connections

### Pin Configuration
- Motion Sensor  → Pin D8  (PCINT0)
- Door Sensor    → Pin D9  (PCINT1)
- Status LED     → Pin D13 (Built-in LED)
- Alarm LED      → Pin D7
- Buzzer         → Pin D6
- Temperature    → Pin A0  (Analog)

## File Descriptions
### Core Files
main.cpp
- Main Arduino sketch with setup() and loop() functions
- Includes all other modules
- Implements the main Sense-Think-Act control loop

system_config.h/cpp
-- Global constants and pin definitions
-- Data structures (SystemState, SensorStates, SystemFlags)
-- Global variable declarations and definitions
-- System initialisation function

### Functional Modules
interrupts.h/cpp
- Pin Change Interrupt (PCI) setup and handling
- Timer interrupt configuration
- Interrupt Service Routines (ISRs)
- Interrupt event processing functions