//
// Modified copy from AuditionMode.cpp
//

#include "MidiPlayerMode.h"


MidiPlayerMode::MidiPlayerMode()
{

}

void MidiPlayerMode::onEnter()
{
    Serial.println("enter MidiPlayerMode");
}

void MidiPlayerMode::onExit()
{
    Serial.println("exit MidiPlayerMode");
}

void app_play_next_song(void);
void app_play_prev_song(void);
void app_play_pause_song(void);
void app_rewind_song(void);

bool MidiPlayerMode::handleEvent(StateMachine *machine, Event *event)
{
    if (machine == nullptr || event == nullptr)
    {
        return false;
    }

    switch (event->getType())
    {
    case EventType::APressed:
        {
            Serial.println("MidiPlayerMode Button A Pressed");
            app_play_next_song();
            return true;
        };
    case EventType::BPressed:
        {
            Serial.println("MidiPlayerMode Button B Pressed");
            app_play_prev_song();
            return true;
        };
    case EventType::CPressed:
        {
            Serial.println("MidiPlayerMode Button C Pressed");
            app_play_pause_song();
            return true;
        };
    case EventType::DPressed:
        {
            Serial.println("MidiPlayerMode Button D Pressed");
            app_rewind_song();
            return true;
        };
    case EventType::ALongPressed:
        {
            Serial.println("MidiPlayerMode Button A Long Pressed");
            return true;
        };
    case EventType::BLongPressed:
        {
            Serial.println("MidiPlayerMode Button B Long Pressed");
            synth.increaseVelocity();
            return true;
        };
    case EventType::CLongPressed:
        {
            Serial.println("MidiPlayerMode Button C Long Pressed");
            synth.decreaseVelocity();
            return true;
        };
    case EventType::DLongPressed:
        {
            Serial.println("MidiPlayerMode Button D Long Pressed");
            //next mode index
            int index = 2;
            State *nextState = StateManager::getInstance()->getState(index);
            if (nextState != nullptr)
            {
                machine->changeState(nextState);

                return true;
            }

            return true;
        }
    case EventType::BtnReleased:
        {
            /* nothing to do */
        }
    default:
        return false;
    }
}

int MidiPlayerMode::getID() const
{
    return ID;
}

const char *MidiPlayerMode::getName() const
{
    return "MidiPlayerMode";
}
