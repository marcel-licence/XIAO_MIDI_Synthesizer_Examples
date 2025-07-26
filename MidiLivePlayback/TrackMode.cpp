//
// Created by Administrator on 25-4-9.
//

#include "TrackMode.h"

TrackMode::TrackMode()
{
}

void TrackMode::onEnter()
{
    Serial.println("enter TrackMode");
}

void TrackMode::onExit()
{
    Serial.println("exit TrackMode");
    channel_1_on_off_flag = false;
    channel_2_on_off_flag = false;
    channel_3_on_off_flag = false;
    channel_4_on_off_flag = false;
}

bool TrackMode::handleEvent(StateMachine* machine, Event* event)
{
    if (machine == nullptr || event == nullptr){
        return false;
    }

    switch (event->getType()){
        case EventType::APressed:{
            Serial.println("TrackMode Button A Pressed");
            channel_1_on_off_flag = !channel_1_on_off_flag;
            return true;
        };
        case EventType::BPressed:{
            Serial.println("TrackMode Button B Pressed");
            channel_2_on_off_flag = !channel_2_on_off_flag;
            return true;
        };
        case EventType::CPressed:{
            Serial.println("TrackMode Button C Pressed");
            channel_3_on_off_flag = !channel_3_on_off_flag;
            return true;
        };
        case EventType::DPressed:{
            Serial.println("TrackMode Button D Pressed");
            channel_4_on_off_flag = !channel_4_on_off_flag;
            return true;
        };
        case EventType::ALongPressed:{
            Serial.println("TrackMode Button A Long Pressed");
            return true;
        };
        case EventType::BLongPressed:{
            Serial.println("TrackMode Button B Long Pressed");
            synth.decreaseVelocity();
            return true;
        };
        case EventType::CLongPressed:{
            Serial.println("TrackMode Button C Long Pressed");
            synth.increaseVelocity();
            return true;
        };
        case EventType::DLongPressed:{
            Serial.println("TrackMode Button D Long Pressed");
            if (entryFlag == false)
                return false;
            int index = 1;
            State* nextState = StateManager::getInstance()->getState(index);
            if (nextState != nullptr){
                machine->changeState(nextState);
                entryFlag = true;
                return true;
            }
            return true;
        }
        case EventType::None:{
            entryFlag = true;
        };
        default:
            return false;
    }
}

int TrackMode::getID() const
{
    return ID;
}

const char* TrackMode::getName() const
{
    return "TrackMode";
}
