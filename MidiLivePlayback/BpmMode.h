//
// Created by Administrator on 25-4-9.
//

#ifndef BPMMODE_H
#define BPMMODE_H

#include "AuditionMode.h"

//BpmMode Mode 2
// BpmMode is a derived class from State that represents a state in a state machine for BPM (Beats Per Minute).
// It provides its own implementations for the virtual methods of the base State class to handle state-specific behavior.
//
// Enum `ID` defines a unique identifier for this state (ID = 2), which allows it to be easily referenced by its ID in the state machine.
//
// Methods:
// - `onEnter()`: This method is called when the state is entered. Override this method to define custom logic for when the state starts.
// - `onExit()`: This method is called when exiting the state. Override this to handle any cleanup or finalization when leaving the state.
// - `handleEvent()`: This method is called to handle events while the state is active. It allows you to define behavior based on events that occur while in the BPM mode state.
// - `getID()`: Returns the unique ID of this state, which is `2` in this case. This ID is used for identifying the state in the state machine.
// - `getName()`: Returns the name of the state as a string. This can be useful for debugging or logging to identify which state is currently active.
class BpmMode : public State {
	public:
		enum {ID = 2};  // Unique ID for this state (ID = 2).
	
		BpmMode();  // Constructor that initializes the state.
	
		virtual void onEnter();  // Called when entering the BPM mode state. Override this for custom entry logic.
		virtual void onExit();   // Called when exiting the BPM mode state. Override this for custom exit logic.
	
		// Handles events while in this state. Override this to define custom event handling logic for BPM mode.
		virtual bool handleEvent(StateMachine* machine, Event* event);
	
		virtual int getID() const;  // Returns the ID for this state (2 in this case).
		virtual const char* getName() const;  // Returns the name of the state as a string, typically "BPM Mode".
	};
	




#endif //BPMMODE_H
