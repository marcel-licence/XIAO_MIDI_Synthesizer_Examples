//
// Created by Administrator on 25-4-9.
//

#ifndef TRACKMODE_H
#define TRACKMODE_H

#include "AuditionMode.h"

// TrackMode mode 3
// TrackMode is a derived class from State, representing a mode where tracking functionality is active.
// It provides methods for handling entry, exit, events, and identifying the state.
class TrackMode : public State {
	public:
		enum { ID = 3 };  // Unique identifier for this state (ID = 3 for the "Track Mode" state).
		
		TrackMode();  // Constructor to initialize the state object.
		
		// Called when the state machine enters the TrackMode state. 
		// Override this method to implement any logic when entering the "TrackMode" state.
		virtual void onEnter();  
		
		// Called when the state machine exits the TrackMode state.
		// Override this method to implement any cleanup or exit logic.
		virtual void onExit();   
	
		// Handles events in the TrackMode state. The method returns a boolean value 
		// indicating whether the event was successfully processed or not.
		virtual bool handleEvent(StateMachine* machine, Event* event);  
	
		// Returns the unique ID associated with this state. It helps the state machine identify this state.
		virtual int getID() const;  
		
		// Returns the name of the state as a string. Typically, this could return "TrackMode".
		virtual const char* getName() const;
	};
	


#endif //TRACKMODE_H
