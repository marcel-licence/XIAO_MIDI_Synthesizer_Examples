//
// Created by Administrator on 25-4-9.
//

#include "ErrorState.h"

ErrorState::ErrorState():_errorCode(0),_errorMsg("unknown error")
{
}

void ErrorState::setError(int code, const char* msg)
{
    _errorCode = code;
    _errorMsg = msg;
}

void ErrorState::onEnter()
{

}

void ErrorState::onExit()
{

}

bool ErrorState::handleEvent(StateMachine* machine, Event* event)
{
    if(machine == nullptr || event == nullptr)
    {
        return false;
    }
    return true;
}

int ErrorState::getID() const
{
    return ID;
}

const char* ErrorState::getName() const
{
    return "Error";
}
