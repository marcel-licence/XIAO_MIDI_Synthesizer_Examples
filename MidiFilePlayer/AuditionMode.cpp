//
// Created by Administrator on 25-4-9.
//

#include "AuditionMode.h"

bool entryFlag = true;
bool channel_1_on_off_flag = false;
bool channel_2_on_off_flag = false;
bool channel_3_on_off_flag = false;
bool channel_4_on_off_flag = false;
bool drum_on_off_flag = false;

unsigned long btnPressStartTime  = 0;
uint8_t buttonPressCount = 0;
bool isRecording = false;
uint8_t randomNote = 0;

AuditionMode::AuditionMode()
{

}

void AuditionMode::onEnter()
{
    Serial.println("enter AuditionMode");
}

void AuditionMode::onExit()
{
    Serial.println("exit AuditionMode");
    drum_on_off_flag = false;
}

bool AuditionMode::handleEvent(StateMachine* machine, Event* event)
{
    if(machine == nullptr || event == nullptr){
        return false;
    }

    switch(event->getType()){
        case EventType::APressed:{
            Serial.println("AuditionMode Button A Pressed");
            static uint8_t instrument = unit_synth_instrument_t::GrandPiano_1;
            instrument++;
            if(instrument>Gunshot)
            {
                instrument = GrandPiano_1;
            }
            synth.setInstrument(0,CHANNEL_0,instrument);
            synth.setNoteOn(CHANNEL_0,NOTE_C4,VELOCITY_MAX);
            
            // synth.setNoteOff(CHANNEL_0,NOTE_C4,VELOCITY_MAX);
            return true;
        };
        case EventType::BPressed:{
            Serial.println("AuditionMode Button B Pressed");
            synth.decreasePitch();
            synth.setNoteOn(CHANNEL_0, synth.getPitch(), VELOCITY_MAX);
            return true;
        };
        case EventType::CPressed:{
            Serial.println("AuditionMode Button C Pressed");
            synth.increasePitch();
            synth.setNoteOn(CHANNEL_0, synth.getPitch(), VELOCITY_MAX);
            return true;
        };
        case EventType::DPressed:{
            Serial.println("AuditionMode Button D Pressed");
            drum_on_off_flag = !drum_on_off_flag;
            return true;
        };
        case EventType::ALongPressed:{
            Serial.println("AuditionMode Button A Long Pressed");
            return true;
        };
        case EventType::BLongPressed:{
            Serial.println("AuditionMode Button B Long Pressed");
            synth.increaseVelocity();
            return true;
        };
        case EventType::CLongPressed:{
            Serial.println("AuditionMode Button C Long Pressed");
            synth.decreaseVelocity();
            return true;
        };
        case EventType::DLongPressed:{
            Serial.println("AuditionMode Button D Long Pressed");
            //next mode index
            int index = 2;
            State* nextState = StateManager::getInstance()->getState(index);
            if(nextState != nullptr){
                machine->changeState(nextState);
                entryFlag = true;
                return true;
            }
            entryFlag = false;
            return true;
        }
        case EventType::BtnReleased:{
            delay(50);
            synth.setAllNotesOff(CHANNEL_0);
            
            entryFlag = true;
        }
        default:
            return false;
    }
}

int AuditionMode::getID() const
{
    return ID;
}

const char* AuditionMode::getName() const
{
    return "AuditionMode";
}
