# MidiLivePlayback Firmware for XIAO_MIDI_Synthesizer 

This firmware is based on the StateMachine example for the XIAO_MIDI_Synthesizer. It enables the XIAO ESP32C3 to listen for MIDI events on its serial RX pin and forward detected MIDI messages to the SAM2695 synthesizer chip.

## Features

- **MIDI Event Forwarding:** Listens for MIDI messages on the serial RX pin and forwards them to the SAM2695.
- **Controller Mapping:** Supports a MIDI controller map to assign control change inputs to specific functions.
- **Helper Functions:** Includes utilities to send RPN, NRPN, and SYSEX messages.
- **SAM2695 Parameter Control:** Provides functions to modify SAM2695 parameters such as:
    - MasterKeyShift
    - TVFCutoffFreqModify
    - FinTuningInCents
    - EqLowBand
    - EqMidLowBand
    - EqMidHighBand
    - EqHighBand

## MIDI Input Monitoring

To verify your MIDI input, you can use the [ml_midi_monitor](https://github.com/marcel-licence/ML_SynthTools/tree/main/examples/ml_midi_monitor) tool. It displays received MIDI messages from your controller.

## Hardware Setup

You may need to build your own circuit for MIDI input. Refer to the [midi_input.md](https://github.com/marcel-licence/ML_SynthTools/blob/main/extras/midi_input.md) guide for instructions.
