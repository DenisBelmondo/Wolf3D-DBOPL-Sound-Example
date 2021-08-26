#ifndef MAIN_H
#define MAIN_H

#include <dbopl.h>

#define SAMPLE_RATE 44100
#define ADLIB_SOUND_RATE 140
#define SAMPLES_PER_MUSIC_TICK (SAMPLE_RATE / ADLIB_SOUND_RATE)
#define BUFFER_SIZE 512

struct AdlibSound
{
    uint32_t length;
    uint16_t priority;
    uint8_t  instrument[16];
    uint8_t  block;
    uint8_t  *data;

    ~AdlibSound();
};

int sign(int x)
{
    return (x > 0) - (x < 0);
}

static const uint8_t _INSTRUMENT_REGISTER_MAP[] = {
    0x20, 0x23, 0x40, 0x43, 0x60, 0x63, 0x80, 0x83, 0xE0, 0xE3,
};

static DBOPL::Handler _opl;

static void _init_opl3();
static void _shutdown_opl3();

#endif // MAIN_H
