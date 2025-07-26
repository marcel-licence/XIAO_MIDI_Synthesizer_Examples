/** 
 * This is a basic example code. The purpose of the code is to send MIDI signals via serial
 * from the XIAO series development board to make the SAM2695 chip produce sound.
 * You will need to define the serial port and its pins. This example uses the XIAO_ESP32S3.
 * For pin definitions, please refer to https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/
 * 
 * This example has the following features:
 * Mode 1:(Audition Mode)
 *      The indicator blinks every 2 seconds, ON for 2s, OFF for 2s
 *      Button A: 
 *              Short press: Switch tone          
 *              Long press: None
 *      Button B: 
 *              Short press: Decrease pitch       
 *              Long press: Decrease overall volume
 *      Button C: 
 *              Short press: Increase pitch       
 *              Long press: Increase overall volume
 *      Button D:
 *              Short press: Play/Stop drumbeat music  
 *              Long press: Switch to the next mode
 * Mode 2:(Bpm Mode)
 *      The indicator blinks every 0.5 seconds, ON for 0.5s, OFF for 0.5s
 *      Button A: 
 *              Short press: None                
 *              Long press: None
 *      Button B: 
 *              Short press: Decrease BPM         
 *              Long press: Decrease overall volume
 *      Button C: 
 *              Short press: Increase BPM         
 *              Long press: Increase overall volume
 *      Button D: 
 *              Short press: Play/Stop drumbeat music  
 *              Long press: Switch to the next mode
 * Mode 3:(Track Mode)
 *      The indicator blinks every 0.1 seconds, ON for 0.1s, OFF for 0.1s
 *      Button A: 
 *              Short press: Play/Stop chord 1           
 *              Long press: None
 *      Button B: 
 *              Short press: Play/Stop chord 2           
 *              Long press: Decrease overall volume
 *      Button C: 
 *              Short press: Play/Stop chord 3           
 *              Long press: Increase overall volume
 *      Button D: 
 *              Short press: Play/Stop a melody made of three chords  
 *              Long press: Switch to the next mode (Mode 1)
 */


#include <Arduino.h>
#include "AuditionMode.h"
#include "SAM2695Synth.h"
#include "Button.h"
#include "BpmMode.h"
#include "TrackMode.h"
#include "ErrorState.h"
#include "music.h"

//LED toggle events corresponding to different modes
#define STATE_1_LED_TIME 2000
#define STATE_2_LED_TIME 500
#define STATE_3_LED_TIME 100

#ifdef __AVR__
    #include <SoftwareSerial.h>
    SoftwareSerial SSerial(2, 3); // RX, TX
    #define COM_SERIAL SSerial
    #define SHOW_SERIAL Serial
    SAM2695Synth<SoftwareSerial> synth = SAM2695Synth<SoftwareSerial>::getInstance();
#endif

#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) ||  defined(ARDUINO_XIAO_RA4M1) 
    #include <SoftwareSerial.h>
    SoftwareSerial SSerial(D7, D6); // RX, TX
    #define COM_SERIAL SSerial
    #define SHOW_SERIAL Serial
    SAM2695Synth<SoftwareSerial> synth = SAM2695Synth<SoftwareSerial>::getInstance();
#endif

#if  defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32S3)
    #define COM_SERIAL Serial0
    #define SHOW_SERIAL Serial
    SAM2695Synth<HardwareSerial> synth = SAM2695Synth<HardwareSerial>::getInstance();
#endif

#if defined(NRF52840_XXAA)
    #ifdef USE_TINYUSB
    #include <Adafruit_TinyUSB.h>
    #endif
    #define COM_SERIAL Serial1
    #define SHOW_SERIAL Serial

    SAM2695Synth<Uart> synth = SAM2695Synth<Uart>::getInstance();
#endif

#ifdef SEEED_XIAO_M0
    #define COM_SERIAL Serial1
    #define SHOW_SERIAL Serial
    SAM2695Synth<Uart> synth = SAM2695Synth<Uart>::getInstance();
#elif defined(ARDUINO_SAMD_VARIANT_COMPLIANCE)
    #define COM_SERIAL Serial1
    #define SHOW_SERIAL SerialUSB
    SAM2695Synth<Uart> synth = SAM2695Synth<Uart>::getInstance();
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3)
    #define BUTTON_A_PIN 4
    #define BUTTON_B_PIN 3
    #define BUTTON_C_PIN 2 
    #define BUTTON_D_PIN 1 
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    #define BUTTON_A_PIN 5
    #define BUTTON_B_PIN 4
    #define BUTTON_C_PIN 3 
    #define BUTTON_D_PIN 2 
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
    #define BUTTON_A_PIN 21
    #define BUTTON_B_PIN 2
    #define BUTTON_C_PIN 1 
    #define BUTTON_D_PIN 0
#else //You need to refer to the corresponding manual...
    #define BUTTON_A_PIN 0 //You need to refer to the corresponding manual...
    #define BUTTON_B_PIN 1 //You need to refer to the corresponding manual...
    #define BUTTON_C_PIN 2 //You need to refer to the corresponding manual...
    #define BUTTON_D_PIN 3 //You need to refer to the corresponding manual...
#endif

#if defined(CONFIG_IDF_TARGET_ESP32C3)
    #define LED_PIN 10              //You need to connect a light yourself
#else
    #define LED_PIN LED_BUILTIN
#endif


//Define the structure needed for the button
BtnState btnA = {HIGH, HIGH, 0, 0, false};
BtnState btnB = {HIGH, HIGH, 0, 0, false};
BtnState btnC = {HIGH, HIGH, 0, 0, false};
BtnState btnD = {HIGH, HIGH, 0, 0, false};

// Define an array of buttons and state machine events
ButtonFlags buttonFlags[] = {
    {shortPressFlag_A, longPressFlag_A, releaseFlag_A, EventType::APressed, EventType::ALongPressed},
    {shortPressFlag_B, longPressFlag_B, releaseFlag_B, EventType::BPressed, EventType::BLongPressed},
    {shortPressFlag_C, longPressFlag_C, releaseFlag_C, EventType::CPressed, EventType::CLongPressed},
    {shortPressFlag_D, longPressFlag_D, releaseFlag_D, EventType::DPressed, EventType::DLongPressed}
};

//create state machine
StateMachine stateMachine;
StateManager* manager = StateManager::getInstance();

int beatCount = 0;                                  // Beat counter
int noteType = QUATER_NOTE;                         // Note type selection: 0 (quarter note), 1 (eighth note), 2 (sixteenth note)
int beatsPerBar = BEATS_BAR_DEFAULT;                // Beats per measure, can be 2, 3, or 4

unsigned long preMillisCh_1 = 0;                    // Record the time of the last MIDI signal sent on track 1
unsigned long preMillisCh_2 = 0;                    // Record the time of the last MIDI signal sent on track 2
unsigned long preMillisCh_3 = 0;                    // Record the time of the last MIDI signal sent on track 3
unsigned long preMillisCh_4 = 0;                    // Record the time of the last MIDI signal sent on track 4
unsigned long preMillisCh_drup = 0;                 // Record the time of the last MIDI signal sent on track drup
uint8_t drupCount = 0;                              // drup track count
uint8_t countBytrack1 = 0;                          // music 1 count
uint8_t countBytrack2 = 0;                          // music 2 count

//LED data
uint8_t  modeID = AuditionMode::ID;                 // state mode id 
int ledTime = STATE_1_LED_TIME;                     // LED toggle events TIME
unsigned long previousMillisLED = 0;                // Record the time of  the last LED toggle               

void setup()
{
    //  serial init to usb
    SHOW_SERIAL.begin(USB_SERIAL_BAUD_RATE);
    // Synth initialization. Since a hardware serial port is used here, the software serial port is commented out.
    synth.begin(COM_SERIAL, MIDI_SERIAL_BAUD_RATE);
    synth.setInstrument(0,CHANNEL_0,unit_synth_instrument_t::GrandPiano_1);
    // initialize the led
    pinMode(LED_PIN, OUTPUT);
    // Initialize the buttons you are using.
    initButtons(BUTTON_A_PIN);
    initButtons(BUTTON_B_PIN);
    initButtons(BUTTON_C_PIN);
    initButtons(BUTTON_D_PIN);
    delay(3000);
    //regist three mode state
    manager->registerState(new AuditionMode());
    manager->registerState(new BpmMode());
    manager->registerState(new TrackMode());
    //regist error state
    ErrorState* errorState = new ErrorState();
    manager->registerState(errorState);
    //init state machine
    if(!(stateMachine.init(manager->getState(AuditionMode::ID), errorState)))
    {
        StateManager::releaseInstance();
        return ;
    }
    Serial.println("synth and state machine ready!");
}

void loop()
{
    Event* event = getNextEvent();
    if(event != nullptr)
    {
        stateMachine.handleEvent(event);
        // set the event type is None
        stateMachine.recycleEvent(event);
    }
    multiTrackPlay();
    ledShow();
}

Event* getNextEvent()
{
    // detectButtonEvents(BUTTON_A_PIN, btnA, act);
    detectButtonEvents(BUTTON_A_PIN, btnA, shortPressFlag_A, longPressFlag_A, releaseFlag_A);
    detectButtonEvents(BUTTON_B_PIN, btnB, shortPressFlag_B, longPressFlag_B, releaseFlag_B);
    detectButtonEvents(BUTTON_C_PIN, btnC, shortPressFlag_C, longPressFlag_C, releaseFlag_C);
    detectButtonEvents(BUTTON_D_PIN, btnD, shortPressFlag_D, longPressFlag_D, releaseFlag_D);

    for (const auto& flags : buttonFlags) {
        if (flags.shortPress) {
            flags.shortPress = false;
            return stateMachine.getEvent(flags.shortPressType);
        }
        if (flags.longPress) {
            flags.longPress = false;
            return stateMachine.getEvent(flags.longPressType);
        }
    }

    bool anyReleased = false;
    for (auto& flags : buttonFlags) {
        if (flags.release) {
            anyReleased = true;
            flags.release = false;
        }
    }

    if (anyReleased) {
        return stateMachine.getEvent(EventType::BtnReleased);
    }

    return nullptr;
}

//LED show for diffent mode
void ledShow()
{
    modeID = stateMachine.getCurrentState()->getID();
    if(modeID == AuditionMode::ID)
    {
        ledTime = STATE_1_LED_TIME;
    }
    else if(modeID == BpmMode::ID)
    {
        ledTime = STATE_2_LED_TIME;
    }
    else if(modeID == TrackMode::ID)
    {
        ledTime = STATE_3_LED_TIME;
    }
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillisLED >= ledTime)
    {
        previousMillisLED = millis();
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
}

//Multi-track chord play"
void multiTrackPlay()
{
    unsigned long currentMillis = millis();
    if(channel_1_on_off_flag)
    {
        if (currentMillis - preMillisCh_1 >= channel_1_chord.delay)
        {
            preMillisCh_1 = currentMillis;
            synth.playChord(channel_1_chord);
        }
    }

    if(channel_2_on_off_flag)
    {
        if(currentMillis - preMillisCh_2 >= channel_2_chord.delay)
        {
            preMillisCh_2 = currentMillis;
            synth.playChord(channel_2_chord);
        }
    }

    if(channel_3_on_off_flag)
    {
        if(currentMillis - preMillisCh_3 >= track1[countBytrack1].delay)
        {
            preMillisCh_3 = currentMillis;
            if(track1[countBytrack1].index == countBytrack1)
            {
                synth.playChord(track1[countBytrack1]);
                countBytrack1 = (countBytrack1+1) % (sizeof(track1)/sizeof(track1[0]));
            }
        }
    }

    if(channel_4_on_off_flag)
    {
        if(currentMillis - preMillisCh_4 >= track2[countBytrack2].delay)
        {
            preMillisCh_4 = currentMillis;
            if(track2[countBytrack2].index == countBytrack2)
            {
                synth.playChord(track2[countBytrack2]);
                countBytrack2 = (countBytrack2+1) % (sizeof(track2)/sizeof(track2[0]));
            }
        }
    }

    unsigned long interval = (BASIC_TIME / synth.getBpm()) / (noteType + 1);
    if (drum_on_off_flag)
    {
        if(currentMillis - preMillisCh_drup >= interval)
        {
            preMillisCh_drup = currentMillis;
            if(track3[drupCount].index == drupCount )
            {
                synth.playChord(track3[drupCount]);
            }
            drupCount = (drupCount + 1) % (sizeof(track3)/sizeof(track3[0]));
        }
    }
}


