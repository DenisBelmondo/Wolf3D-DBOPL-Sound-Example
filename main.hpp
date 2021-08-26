#ifndef MAIN_H
#define MAIN_H

#include <dbopl.h>

static const auto SAMPLE_RATE = 44100;

// - every OPL instruction in AdlibSound.data is played per tick
// - the tick rate of Wolf3d IMF is 700 and adlib sounds play every 5 IMF tick
// - 700 / 5 = 140
// - therefore, the adlib sound rate is 140
static const auto ADLIB_SOUND_RATE = 140;

// - there are SAMPLE_RATE samples in 1 second of audio
// - there are 140 adlib ticks per second
// - how many samples are in an adlib tick?
// - samples = SAMPLE_RATE / ADLIB_SOUND_RATE
static const auto SAMPLES_PER_ADLIB_TICK = (SAMPLE_RATE / ADLIB_SOUND_RATE);

// this is just for SDL2_Mixer
static const auto BUFFER_SIZE = 512;

struct AdlibSound
{
    uint32_t length;
    uint16_t priority;
    uint8_t  instrument[16];
    uint8_t  block;
    uint8_t  *data;

    ~AdlibSound();
};

// refer to https://moddingwiki.shikadi.net/wiki/Adlib_sound_effect#Instrument_Data
static const uint8_t _INSTRUMENT_REGISTER_MAP[] = {
    0x20, 0x23, 0x40, 0x43, 0x60, 0x63, 0x80, 0x83, 0xE0, 0xE3,
};

// DBOPL::Handler encapsulates and takes care of most of the initialization of
// DBOPL::Chip. i'll let it do some of the dirty work.
static DBOPL::Handler _opl;

static void _init_opl3();
static void _shutdown_opl3();

#endif // MAIN_H
