/*
 * Copyright (c) 2025 Marcel Licence
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
 * veröffentlichten Version, weiter verteilen und/oder modifizieren.
 *
 * Dieses Programm wird in der Hoffnung bereitgestellt, dass es nützlich sein wird, jedoch
 * OHNE JEDE GEWÄHR,; sogar ohne die implizite
 * Gewähr der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Einzelheiten.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <https://www.gnu.org/licenses/>.
 */

/**
 * @file MidiInterface.ino
 * @author Marcel Licence
 * @date 26.07.2025
 *
 * @brief Implementation of an interface between serial MIDI input and serial TX to the SAM2695 sound chip.
 *        Includes helper functions for sending RPN, NRPN, and SYSEX messages.
 *        In addition to that you will find the MIDI player control functions here
 *
 * @note Requires ML_SynthTools library (https://github.com/marcel-licence/ML_SynthTools).
 *       Building without this library will cause errors.
 *
 * @see https://docs.dream.fr/pdf/Serie2000/SAM_Datasheets/SAM2695.pdf
 */


#include <Arduino.h>

#include <FS.h>
#include <LittleFS.h>

#include <ml_midi_player.h> /* requires ML_SynthTools_Lib library from https://github.com/marcel-licence/ML_SynthTools_Lib */
#include <ml_utils.h> /* requires ML_SynthTools library from https://github.com/marcel-licence/ML_SynthTools */


#define FORMAT_LITTLEFS_IF_FAILED true
#define BUFFER_SIZE (96 * 1024)


// --- MIDI Controller Defines ---
#define MIDI_CC_RPN_MSB         0x65U
#define MIDI_CC_RPN_LSB         0x64U
#define MIDI_CC_NRPN_MSB        0x63U
#define MIDI_CC_NRPN_LSB        0x62U
#define MIDI_CC_DATA_ENTRY_MSB  0x06U
#define MIDI_CC_DATA_ENTRY_LSB  0x26U /* optional, not used here */
#define MIDI_CC_NULL_VALUE      0xFFU

#define SAM2695_NRPN_VOLUME 0x3707U


#define ML_SYNTH_INLINE_DECLARATION
#define ML_SYNTH_INLINE_DEFINITION
#define MIDI_FMT_INT
#include <midi_interface.h> /* requires ML_SynthTools library from https://github.com/marcel-licence/ML_SynthTools */

static struct midi_port_s comPort;

static uint8_t currentChannel = 0;

static uint8_t raw[BUFFER_SIZE];
static int maxFileCount = 0;

static bool contains_mt32(const char *str);
static bool hasExtension(const char *filename, const char *extension);
static bool parseMidiFiles(fs::FS &fs, const char *dirPath, const char *extension, String &foundFilePath);
static bool findMidiFile(fs::FS &fs, const char *dirPath, const char *extension, int targetIndex, String &foundFilePath);


/**
 * @brief Check if filename contains "mt32".
 * @param str Filename string
 * @return true if contains, false otherwise
 */
static bool contains_mt32(const char *str)
{
    if (!str)
    {
        return false;
    }

    while (*str)
    {
        if (tolower(str[0]) == 'm' &&
                tolower(str[1]) == 't' &&
                str[2] == '3' &&
                str[3] == '2')
        {
            return true;
        }
        str++;
    }
    return false;
}

/**
 * @brief Setup MIDI player with file.
 * @param filename Path to MIDI file
 */
void midi_player_setup(const char *filename)
{
    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
    {
        SHOW_SERIAL.println("LittleFS Mount Failed");
        return;
    }

    fs::FS &fs = LittleFS;

    SHOW_SERIAL.printf("Reading file: %s\r\n", filename);

    File file = fs.open(filename);
    if (!file || file.isDirectory())
    {
        SHOW_SERIAL.println("- failed to open file for reading");
        return;
    }

    SHOW_SERIAL.println("- read from file:");

    int bytesRead = file.read(raw, sizeof(raw));
    SHOW_SERIAL.printf("read int %d bytes\n", bytesRead);
    file.close();

    ml_midi_player_setup(raw, bytesRead);

    if (contains_mt32(filename))
    {
        SHOW_SERIAL.printf("use mt-32 sound variation!\n");
        ml_midi_player_set_mt32_sound_variation();
    }

    SHOW_SERIAL.printf("Filename: %s\n", filename);
    SHOW_SERIAL.printf("Size: %u\n", bytesRead);
}

/**
 * @brief Check if filename has given extension.
 * @param filename Filename string
 * @param extension Extension string (e.g. ".mid")
 * @return true if matches, false otherwise
 */
static bool hasExtension(const char *filename, const char *extension)
{
    const char *ext = strrchr(filename, '.');
    return ext && strcasecmp(ext, extension) == 0;
}

/**
 * @brief Parse MIDI files in directory.
 * @param fs Filesystem object
 * @param dirPath Directory path
 * @param extension File extension
 * @param foundFilePath Output file path
 * @return true if found, false otherwise
 */
static bool parseMidiFiles(fs::FS &fs, const char *dirPath, const char *extension, String &foundFilePath)
{
    File root = fs.open(dirPath);
    if (!root || !root.isDirectory())
    {
        SHOW_SERIAL.println("Invalid directory.");
        return false;
    }

    File file;

    // Loop through directory
    while ((file = root.openNextFile()))
    {
        if (file.isDirectory())
        {
            // Recursive search if subdirectory
            String subPath = String(dirPath) + "" + file.name();
            if (parseMidiFiles(fs, subPath.c_str(), extension, foundFilePath))
            {
                file.close();
                return true;
            }
        }
        else
        {
            if (hasExtension(file.name(), extension))
            {
                maxFileCount++;
            }
        }
        file.close();
    }

    // Not enough matches found
    return false;
}

/**
 * @brief Find MIDI file by index.
 * @param fs Filesystem object
 * @param dirPath Directory path
 * @param extension File extension
 * @param targetIndex File index
 * @param foundFilePath Output file path
 * @return true if found, false otherwise
 */
static bool findMidiFile(fs::FS &fs, const char *dirPath, const char *extension, int targetIndex, String &foundFilePath)
{
    File root = fs.open(dirPath);
    if (!root || !root.isDirectory())
    {
        SHOW_SERIAL.println("Invalid directory.");
        return false;
    }

    File file;
    int currentIndex = 0;

    // Loop through directory
    while ((file = root.openNextFile()))
    {
        if (file.isDirectory())
        {
            // Recursive search if subdirectory
            String subPath = String(dirPath) + "" + file.name();
            if (findMidiFile(fs, subPath.c_str(), extension, targetIndex - currentIndex, foundFilePath))
            {
                file.close();
                return true;
            }
        }
        else
        {
            if (hasExtension(file.name(), extension))
            {
                if (currentIndex == targetIndex)
                {
                    foundFilePath = String(dirPath) + "" + file.name();
                    file.close();
                    return true;
                }
                currentIndex++;
            }
        }
        file.close();
    }

    // Not enough matches found
    return false;
}

/**
 * @brief Setup MIDI player by file index.
 * @param fileIndex Index of MIDI file
 * @return true if successful, false otherwise
 */
bool midi_player_setup(int fileIndex)
{
    String midiFile;

    sendNRPN3707Volume(0, 96);

    if (maxFileCount == 0)
    {
        parseMidiFiles(LittleFS, "/", ".mid", midiFile);
    }
    SHOW_SERIAL.printf("Files Found: %u", maxFileCount);

    if (findMidiFile(LittleFS, "/", ".mid", fileIndex, midiFile))
    {
        SHOW_SERIAL.print("Selected MIDI file: ");
        SHOW_SERIAL.println(midiFile);

        midi_player_setup(midiFile.c_str());
        return true;
    }
    else
    {
        SHOW_SERIAL.println("No more MIDI files found.");
        return false;
    }
}

/**
 * @brief Send GM Reset SysEx message.
 */
void midi_player_send_gm_reset_msg(void)
{
    send_gm_reset_msg();
}

/**
 * @brief Send raw MIDI data.
 * @param msg Pointer to message
 * @param len Length of message
 */
void midi_player_send_data(uint8_t *msg, int len)
{
    COM_SERIAL.write(msg, len);
}

/**
 * @brief Send a MIDI RPN message.
 * @param channel MIDI channel (0-15)
 * @param rpn Registered Parameter Number
 * @param value Value to send (0-127)
 */
void sendRPN(uint8_t channel, uint16_t rpn, uint8_t value)
{
    if (channel > 15 || value > 127)
    {
        return;
    }

    uint8_t status = 0xB0U | (channel & 0x0FU); // Control Change on channel
    uint8_t msg[] = {status, MIDI_CC_RPN_MSB, (uint8_t)((rpn >> 8U) & 0xFFU), MIDI_CC_RPN_LSB, (uint8_t)(rpn & 0xFFU), 0x06U, value};

    COM_SERIAL.write(msg, sizeof(msg));
}

/**
 * @brief Send a MIDI NRPN message.
 * @param channel MIDI channel (0-15)
 * @param nrpn Non-Registered Parameter Number
 * @param value Value to send (0-127)
 */
void sendNRPN(uint8_t channel, uint16_t nrpn, uint8_t value)
{
    if (channel > 15 || value > 127)
    {
        return;
    }

    uint8_t status = 0xB0 | (channel & 0x0F); // Control Change on channel
    uint8_t msg[] = {status, MIDI_CC_NRPN_MSB, (uint8_t)((nrpn >> 8U) & 0xFF), MIDI_CC_NRPN_LSB, (uint8_t)(nrpn & 0xFFU), MIDI_CC_DATA_ENTRY_MSB, value};

    COM_SERIAL.write(msg, sizeof(msg));
}

/**
 * @brief Send a Roland GS SysEx Data Set message.
 * @param parameterAddr GS parameter address
 * @param value Value to set (0-127)
 */
void send_data_set_gs_sysex(uint16_t parameterAddr, uint8_t value)
{
    if (value > 127)
    {
        return;
    }

    const uint8_t SYSEX_START = 0xF0;
    const uint8_t SYSEX_END = 0xF7;
    const uint8_t MANUFACTURER_ID = 0x41; // Roland
    const uint8_t DEVICE_ID = 0x00; // Device ID (set to 0)
    const uint8_t MODEL_ID = 0x42; // GS
    const uint8_t COMMAND_ID = 0x12; // Data Set 1 (Write)
    const uint8_t ADDRESS_MSB = 0x40; // Address prefix for GS parameters

    uint8_t addr_high = (parameterAddr >> 8) & 0x7F;
    uint8_t addr_low = parameterAddr & 0x7F;
    uint8_t checksum = (128 - ((ADDRESS_MSB + addr_high + addr_low + value) % 128)) & 0x7F;

    uint8_t sysex_msg[] =
    {
        SYSEX_START,
        MANUFACTURER_ID,
        DEVICE_ID,
        MODEL_ID,
        COMMAND_ID,
        ADDRESS_MSB,
        addr_high,
        addr_low,
        value,
        checksum,
        SYSEX_END
    };

    COM_SERIAL.write(sysex_msg, sizeof(sysex_msg));
}

/**
 * @brief Send NRPN 0x3707 (Volume) message.
 * @param channel MIDI channel (0-15)
 * @param value Volume value (0-127)
 */
void sendNRPN3707Volume(uint8_t channel, uint8_t value)
{
    if (channel > 15 || value > 127)
    {
        return;
    }

    sendNRPN(channel, 0x3707, value);
}

/**
 * @brief Send GM Reset SysEx message.
 */
void send_gm_reset_msg(void)
{
    uint8_t gm_reset_msg[] = {0xF0, 0x7E, 0x7F, 0x09, 0x01, 0xF7};
    COM_SERIAL.write(gm_reset_msg, sizeof(gm_reset_msg));
}

/**
 * @brief Send MIDI Note On message.
 * @param ch MIDI channel
 * @param note Note number
 * @param vel Velocity
 */
void App_NoteOn(uint8_t ch, uint8_t note, uint8_t vel)
{
    currentChannel = ch;
    uint8_t midiMsg[] = {(uint8_t)(ch | 0x90U), note, vel};
    COM_SERIAL.write(midiMsg, sizeof(midiMsg));
}

/**
 * @brief Send MIDI Note Off message.
 * @param ch MIDI channel
 * @param note Note number
 */
void App_NoteOff(uint8_t ch, uint8_t note)
{
    uint8_t midiMsg[] = {(uint8_t)(ch | 0x80U), note, 0U};
    COM_SERIAL.write(midiMsg, sizeof(midiMsg));
}

/**
 * @brief Send MIDI Pitch Bend message.
 * @param ch MIDI channel
 * @param amount Pitch bend value (0-16383)
 */
void App_PitchBend(uint8_t ch, uint16_t amount)
{
    uint8_t midiMsg[] = {(uint8_t)(ch | 0xE0U), (uint8_t)(amount & 0x7FU), (uint8_t)((amount >> 7) & 0x7FU)};
    COM_SERIAL.write(midiMsg, sizeof(midiMsg));
}

/**
 * @brief Send MIDI Program Change message.
 * @param ch MIDI channel
 * @param program Program number
 */
void App_ProgramChange(uint8_t ch, uint8_t program)
{
    uint8_t midiMsg[] = {(uint8_t)(ch | 0xC0U), program};
    COM_SERIAL.write(midiMsg, sizeof(midiMsg));
}

/**
 * @brief Send MIDI Control Change message.
 * @param ch MIDI channel
 * @param data0 Controller number
 * @param data1 Controller value
 */
void midi_send_cc(uint8_t ch, uint8_t data0, uint8_t data1)
{
    uint8_t midiMsg[] = {(uint8_t)(ch | 0xB0U), data0, data1};
    COM_SERIAL.write(midiMsg, sizeof(midiMsg));
}

/**
 * @brief Handle button A1..A9 press.
 * @param param Button parameter
 */
void AppBtnA(uint8_t param)
{
    if (param < 8)
    {
        ml_midi_player_toggle_track_mute(param);
    }
    else
    {
        app_play_prev_song();
    }
}

/**
 * @brief Handle button A1..A9 with value.
 * @param param Button parameter
 * @param value Value
 */
void AppBtnA(uint8_t param, uint8_t value)
{
    if (value >= 64)
    {
        AppBtnA(param);
    }
}

/**
 * @brief Handle button B1..B9 press.
 * @param param Button parameter
 */
void AppBtnB(uint8_t param)
{
    if (param < 8)
    {
        if (param == 7)
        {
            COM_SERIAL.write(0xFF);
        }
        else
        {
            ml_midi_player_toggle_track_mute(param + 8);
        }
    }
    else
    {
        app_play_next_song();
    }
}

/**
 * @brief Handle button B1.B9 with value.
 * @param param Button parameter
 * @param value Value
 */
void AppBtnB(uint8_t param, uint8_t value)
{
    if (value >= 64)
    {
        AppBtnB(param);
    }
}

/**
 * @brief Handle rewind action.
 * @param param Unused
 * @param value Value
 */
void App_Rewind(uint8_t param, uint8_t value)
{
    if (value >= 64)
    {
        ml_midi_player_rewind();
    }
}

/**
 * @brief Handle stop action.
 * @param param Unused
 * @param value Value
 */
void App_Stop(uint8_t param, uint8_t value)
{
    if (value >= 64)
    {
        ml_midi_player_stop();
    }
}

/**
 * @brief Handle play action.
 * @param param Unused
 * @param value Value
 */
void App_Play(uint8_t param, uint8_t value)
{
    if (value >= 64)
    {
        ml_midi_player_play();
    }
}

/**
 * @brief Set tempo.
 * @param param Unused
 * @param value Tempo value
 */
void App_SetTempo(uint8_t param, uint8_t value)
{
    float tempo = floatFromU7(value) * (240.0f - 60.0f) + 60.0f;
    ml_midi_player_set_tempo(tempo);
}

/**
 * @brief Set volume using NRPN 0x3707.
 * @param param Unused
 * @param value Volume value
 */
void App_SetVolume(uint8_t param, uint8_t value)
{
    sendNRPN3707Volume(0, value);
}

/**
 * @brief Set pan using MIDI CC 10.
 * @param param Unused
 * @param value Pan value
 */
void App_SetPan(uint8_t param, uint8_t value)
{
    midi_send_cc(currentChannel, 10, value);
}

/**
 * @brief Set channel volume using MIDI CC 7.
 * @param param Unused
 * @param value Volume value
 */
void App_SetChannelVolume(uint8_t param, uint8_t value)
{
    midi_send_cc(currentChannel, 7, value);
}

/**
 * @brief Send generic control change.
 * @param param Controller number
 * @param value Controller value
 */
void App_SendControlChange(uint8_t param, uint8_t value)
{
    midi_send_cc(currentChannel, param, value);
}

/**
 * @brief Set EQ Low Band using NRPN 0x3700.
 * @param param Unused
 * @param value EQ value
 */
void SAM2695_Set_EqLowBand(uint8_t param, uint8_t value)
{
    sendNRPN(0, 0x3700, value);
}

/**
 * @brief Set EQ Mid Low Band using NRPN 0x3701.
 * @param param Unused
 * @param value EQ value
 */
void SAM2695_Set_EqMidLowBand(uint8_t param, uint8_t value)
{
    sendNRPN(0, 0x3701, value);
}

/**
 * @brief Set EQ Mid High Band using NRPN 0x3702.
 * @param param Unused
 * @param value EQ value
 */
void SAM2695_Set_EqMidHighBand(uint8_t param, uint8_t value)
{
    sendNRPN(0, 0x3702, value);
}

/**
 * @brief Set EQ High Band using NRPN 0x3703.
 * @param param Unused
 * @param value EQ value
 */
void SAM2695_Set_EqHighBand(uint8_t param, uint8_t value)
{
    sendNRPN(0, 0x3703, value);
}

/**
 * @brief Set Main Echo Right Volume using NRPN 0x3734.
 * @param param Unused
 * @param value Echo volume
 */
void SAM2695_Set_MainEchoRightVolume(uint8_t param, uint8_t value)
{
    sendNRPN(0, 0x3734, value);
}

/**
 * @brief Set Spatial Effect Volume using NRPN 0x3720.
 * @param param Unused
 * @param value Effect volume
 */
void SAM2695_Set_SpatialEffectVolume(uint8_t param, uint8_t value)
{
    sendNRPN(0, 0x3720, value);
}

/**
 * @brief Select Reverb Program using MIDI CC 0x50.
 * @param param Unused
 * @param value Program number (0-7)
 */
void SAM2695_Set_ReverbProgramSelect(uint8_t param, uint8_t value)
{
    midi_send_cc(0, 0x50, value % 8);
}

/**
 * @brief Select Chorus Program using MIDI CC 81.
 * @param param Unused
 * @param value Program number (0-7)
 */
void SAM2695_Set_ChorusProgramSelect(uint8_t param, uint8_t value)
{
    midi_send_cc(0, 81, value % 8);
}

/**
 * @brief Set Master Key Shift using GS SysEx.
 * @param param Unused
 * @param value Key shift value
 */
void SAM2695_Set_MasterKeyShift(uint8_t param, uint8_t value)
{
    send_data_set_gs_sysex(0x0005, value);
}

/**
 * @brief Set TVF Cutoff Frequency Modify using NRPN 0x0120.
 * @param param Unused
 * @param value Cutoff value
 */
void SAM2695_Set_TVFCutoffFreqModify(uint8_t param, uint8_t value)
{
    sendNRPN(0x00, 0x0120, value);
}

/**
 * @brief Set Fine Tuning in cents using RPN 0x0001.
 * @param param Unused
 * @param value Tuning value
 */
void SAM2695_Set_FineTuningInCents(uint8_t param, uint8_t value)
{
    sendRPN(0x00, 0x0001, value);
}

struct midiControllerMapping edirolMapping[] =
{
    /* general MIDI */
    { 0x0, 0x40, "sustain", NULL, NULL, 0},

    /* transport buttons */
    { 0x8, 0x52, "back", NULL, App_Rewind, 0},
    { 0xD, 0x52, "stop", NULL, App_Stop, 0},
    { 0xe, 0x52, "start", NULL, App_Play, 0},
    { 0xa, 0x52, "rec", NULL, NULL, 0},

    /* upper row of buttons */
    { 0x0, 0x50, "A1", NULL, AppBtnA, 0},
    { 0x1, 0x50, "A2", NULL, AppBtnA, 1},
    { 0x2, 0x50, "A3", NULL, AppBtnA, 2},
    { 0x3, 0x50, "A4", NULL, AppBtnA, 3},

    { 0x4, 0x50, "A5", NULL, AppBtnA, 4},
    { 0x5, 0x50, "A6", NULL, AppBtnA, 5},
    { 0x6, 0x50, "A7", NULL, AppBtnA, 6},
    { 0x7, 0x50, "A8", NULL, AppBtnA, 7},

    { 0x0, 0x53, "A9", NULL, AppBtnA, 8},

    /* lower row of buttons */
    { 0x0, 0x51, "B1", NULL, AppBtnB, 0},
    { 0x1, 0x51, "B2", NULL, AppBtnB, 1},
    { 0x2, 0x51, "B3", NULL, AppBtnB, 2},
    { 0x3, 0x51, "B4", NULL, AppBtnB, 3},

    { 0x4, 0x51, "B5", NULL, AppBtnB, 4},
    { 0x5, 0x51, "B6", NULL, AppBtnB, 5},
    { 0x6, 0x51, "B7", NULL, AppBtnB, 6},
    { 0x7, 0x51, "B8", NULL, AppBtnB, 7},

    { 0x1, 0x53, "B9", NULL, AppBtnB, 8},

    /* rotary */
    { 0x0, 0x10, "R1", NULL, App_SetTempo, 0},
    { 0x1, 0x10, "R2", NULL, App_SetVolume, 0},
    { 0x2, 0x10, "R2", NULL, App_SetPan, 0},
    { 0x3, 0x10, "R3", NULL, App_SetChannelVolume, 0},

    { 0x4, 0x10, "R4", NULL, App_SendControlChange, 91},
    { 0x5, 0x10, "R5", NULL, App_SendControlChange, 93},
    { 0x6, 0x10, "R6", NULL, App_SendControlChange, 71},
    { 0x7, 0x10, "R7", NULL, App_SendControlChange, 74},

    { 0x8, 0x10, "R8", NULL, App_SendControlChange, 5},

    /* slider */
    { 0x0, 0x11, "S1", NULL, SAM2695_Set_EqLowBand, 0},
    { 0x1, 0x11, "S2", NULL, SAM2695_Set_EqMidLowBand, 0},
    { 0x2, 0x11, "S3", NULL, SAM2695_Set_EqMidHighBand, 0},
    { 0x3, 0x11, "S4", NULL, SAM2695_Set_EqHighBand, 0},

    { 0x4, 0x11, "S5", NULL, SAM2695_Set_MainEchoRightVolume, 0},
    { 0x5, 0x11, "S6", NULL, SAM2695_Set_SpatialEffectVolume, 0},
    { 0x6, 0x11, "S7", NULL, SAM2695_Set_FineTuningInCents, 0},
    { 0x7, 0x11, "S8", NULL, SAM2695_Set_TVFCutoffFreqModify, 0},

    { 0x1, 0x12, "S9", NULL, SAM2695_Set_MasterKeyShift, 65},
};

struct midiMapping_s midiMapping =
{
    .rawMsg = NULL,
    .noteOn = App_NoteOn,
    .noteOff = App_NoteOff,
    .pitchBend = App_PitchBend,
    .modWheel = NULL,
    .programChange = App_ProgramChange,
    .rttMsg = NULL,
    .songPos = NULL,
    .controlMapping = edirolMapping,
    .mapSize = sizeof(edirolMapping) / sizeof(edirolMapping[0]),
};

/**
 * @brief Setup MIDI communication port.
 */
void midi_com_setup(void)
{
    comPort.serial = &COM_SERIAL;
}

/**
 * @brief MIDI communication loop handler.
 */
void midi_com_loop(void)
{
    Midi_CheckMidiPort(&comPort, 0);
}
