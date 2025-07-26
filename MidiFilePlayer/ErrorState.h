//
// Created by Administrator on 25-4-9.
//

#ifndef ERRORSTATE_H
#define ERRORSTATE_H

#include "AuditionMode.h"

// ErrorState is a derived class from State, representing an error state in a state machine.
// It includes error code and message management to store and report error information.
// The class provides custom logic for state transitions, entry, and exit when an error occurs.

class ErrorState : public State {
    public:
        enum {ID = 100};  // Unique ID for this state (ID = 100, indicating an error state).
        
        ErrorState();  // Constructor to initialize the state.
        
        // Sets the error code and message for this state.
        // The code is an integer representing the error type, 
        // and the message is a human-readable string describing the error.
        void setError(int code, const char* msg);
    
        virtual void onEnter();  // Called when the error state is entered. Override to handle logic when entering error state.
        virtual void onExit();   // Called when exiting the error state. Override for any necessary cleanup.
        
        // Handles events while in the error state.
        // In this case, you can define specific error recovery events or logging events while in this state.
        virtual bool handleEvent(StateMachine* machine, Event* event);
        
        virtual int getID() const;  // Returns the unique ID of the error state (100).
        virtual const char* getName() const;  // Returns the name of the error state, typically "Error".
        
    private:
        int _errorCode;  // Stores the error code (an integer representing the error).
        const char* _errorMsg;  // Stores the error message (a string with detailed error description).
    };
    




#endif //ERRORSTATE_H
