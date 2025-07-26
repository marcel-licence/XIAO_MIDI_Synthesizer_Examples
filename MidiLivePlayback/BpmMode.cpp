//
// Created by Administrator on 25-4-9.
//

#include "BpmMode.h"

BpmMode::BpmMode()
{

}

void BpmMode::onEnter()
{
    Serial.println("enter BpmMode");
}

void BpmMode::onExit()
{
    Serial.println("exit BpmMode");
    drum_on_off_flag = false;
}

bool BpmMode::handleEvent(StateMachine* machine, Event* event)
{
    if (machine == nullptr || event == nullptr){
        return false;
    }

    switch (event->getType()){
        case EventType::APressed:{
            Serial.println("BpmMode Button A  Pressed");
            //first press
            if (!isRecording){
                btnPressStartTime = millis(); //record the start time
                buttonPressCount = 1;
                isRecording = true;
            }
            else{
                if (millis() - btnPressStartTime <= MaxTimeLimit){
                    buttonPressCount++;
                }
                else{
                    isRecording = false;
                    buttonPressCount = 0;
                }
            }
            if (buttonPressCount == 4){
                //calculate the bpm
                int bpm = BASIC_TIME / (millis() - btnPressStartTime);
                synth.setBpm(bpm);
                Serial.println("BPM: " + String(bpm));
                isRecording = false;
                btnPressStartTime = 0;
            }
            return true;
        };
        case EventType::BPressed:{
            Serial.println("BpmMode Button B  Pressed");
            synth.increaseBpm();
            return true;
        };
        case EventType::CPressed:{
            Serial.println("BpmMode Button C  Pressed");
            synth.decreaseBpm();
            return true;
        };
        case EventType::DPressed:{
            Serial.println("BpmMode Button D  Pressed");
            drum_on_off_flag = !drum_on_off_flag;
            return true;
        };
        case EventType::ALongPressed:{
            Serial.println("BpmMode Button A Long Pressed");
            return true;
        };
        case EventType::BLongPressed:{
            Serial.println("BpmMode Button B Long Pressed");
            synth.increaseVelocity();
            return true;
        };
        case EventType::CLongPressed:{
            Serial.println("BpmMode Button C Long Pressed");
            synth.decreaseVelocity();
            return true;
        };
        case EventType::DLongPressed:{
            Serial.println("BpmMode Button D Long Pressed");
            if (entryFlag == false){
                return false;
            }
            int index = 3;
            State* nextState = StateManager::getInstance()->getState(index);
            if (nextState != nullptr){
                machine->changeState(nextState);
                entryFlag = true;
                return true;
            }
            entryFlag = false;
            return true;
        }
        case EventType::None:{
            entryFlag = true;
        };
        default:
            return false;
    }
}

int BpmMode::getID() const
{
    return ID;
}

const char* BpmMode::getName() const
{
    return "BpmMode";
}
