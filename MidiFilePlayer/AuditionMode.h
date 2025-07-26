//
// Created by Administrator on 25-4-1.
//

#ifndef AUDITIONMODE_H
#define AUDITIONMODE_H

#include "State.h"
#include "StateManager.h"
#include "Event.h"
#include "StateMachine.h"
#include "SAM2695Synth.h"

// The maximum recording interval is set to 2s
#define MaxTimeLimit 2000 


#ifdef __AVR__
	#include <SoftwareSerial.h>
	extern SAM2695Synth<SoftwareSerial> synth;
#endif

#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) ||  defined(ARDUINO_XIAO_RA4M1) 
	#include <SoftwareSerial.h>
	extern SAM2695Synth<SoftwareSerial> synth;
#endif

#if  defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32S3)
	extern SAM2695Synth<HardwareSerial> synth;
#endif

#ifdef SEEED_XIAO_M0
	extern SAM2695Synth<Uart> synth;
#elif defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
	extern SAM2695Synth<Uart> synth;
#endif

#if defined(NRF52840_XXAA)
	extern SAM2695Synth<Uart> synth;
#endif
extern bool entryFlag;
extern bool channel_1_on_off_flag;
extern bool channel_2_on_off_flag;
extern bool channel_3_on_off_flag;
extern bool channel_4_on_off_flag;
extern bool drum_on_off_flag;

extern unsigned long btnPressStartTime;
extern uint8_t buttonPressCount;
extern bool isRecording;

// AuditionMode Mode 1 (default)
// AuditionMode is a derived class from State that represents a specific state in the state machine.
// It defines unique behavior for this state by overriding the virtual methods of the base class State.
//
// Enum `ID` defines a unique identifier for this state (ID = 1). This allows the state to be easily referenced by its ID.
// 
// Methods:
// - `onEnter()`: This method is called when the state is entered, to initialize or reset any necessary conditions for the state.
// - `onExit()`: This method is called when the state is exited, allowing for any cleanup or finalization when leaving the state.
// - `handleEvent()`: This method handles events while the state is active. It receives a `StateMachine*` and an `Event*`, allowing it to interact with other components and decide what actions to take based on the event.
// - `getID()`: Returns the unique ID of this state, defined as `1`. This is used by the state manager to identify the state.
// - `getName()`: Returns the name of the state as a string. This provides an easily readable identifier for logging or debugging purposes.
class AuditionMode : public State {
	public:
		enum {ID = 1};  // Unique ID for this state.
	
		AuditionMode();  // Constructor that initializes the state.
		
		virtual void onEnter();  // Called when entering this state. Override for custom entry logic.
		virtual void onExit();   // Called when exiting this state. Override for custom exit logic.
	
		// Handles events while in this state. Override to process specific events.
		virtual bool handleEvent(StateMachine* machine, Event* event);
	
		virtual int getID() const;  // Returns the ID for this state (1 in this case).
		virtual const char* getName() const;  // Returns the name of the state as a string.
	};
	


#endif //BUTTONSTATE_H
