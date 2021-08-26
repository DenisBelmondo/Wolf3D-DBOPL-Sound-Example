#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_mixer.h>
#include <cstdio>

#include "main.hpp"

AdlibSound::~AdlibSound()
{
    if (data)
    {
        delete[] data;
    }
}

static void _init_opl3()
{
    _opl.Init(SAMPLE_RATE);

    // idk what this shit do i think these OPL regs need to be zeroed out???
    for (auto i = 1; i < 0xF6; i++)
    {
        _opl.WriteReg(i, 0);
    }

    // waveform select enable
    // otherwise, the instrument params would be ignored and everything will be
    // played with a plain sine wave (a-la OPL1)
    _opl.WriteReg(1, 0x20);
}

static void _shutdown_opl3()
{

}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        puts("please supply path to .adl file");
        return 1;
    }

    auto err = SDL_Init(SDL_INIT_AUDIO);

    if (err)
    {
        printf("could not initialize sdl: %s\n", SDL_GetError());
        return err;
    }

    err = Mix_OpenAudio(SAMPLE_RATE, AUDIO_S16LSB, 2, BUFFER_SIZE);

    if (err)
    {
        printf("could not initialize sdl_mixer: %s\n", Mix_GetError());
        return err;
    }

    auto fp = fopen(argv[1], "rb");

    if (!fp)
    {
        printf("could not open \"%s\"\n", argv[1]);
        return 1;
    }

    _init_opl3();

    AdlibSound adlsnd;

    std::fread(&adlsnd.length, 4, 1, fp);
    std::fread(&adlsnd.priority, 2, 1, fp);
    std::fread(&adlsnd.instrument, 1, sizeof(adlsnd.instrument), fp);
    std::fread(&adlsnd.block, 1, 1, fp);
    adlsnd.data = new uint8_t[adlsnd.length];
    std::fread(adlsnd.data, 1, adlsnd.length, fp);

    for (auto i = 0; i < sizeof(_INSTRUMENT_REGISTER_MAP); i++)
    {
        _opl.WriteReg(_INSTRUMENT_REGISTER_MAP[i], adlsnd.instrument[i]);
    }

    auto adlsnd_length = adlsnd.length * 2;

    auto src_buffer = new int32_t[adlsnd_length * SAMPLES_PER_ADLIB_TICK];
    auto dst_buffer = new int16_t[adlsnd_length * SAMPLES_PER_ADLIB_TICK * 2];

    int32_t *src_ptr = src_buffer;

    auto block = (adlsnd.block & 7) << 2;
    auto note_on = false;

    for (uint32_t i = 0, dst_buffer_idx = 0; i < adlsnd_length; i++)
    {
        //
        // https://moddingwiki.shikadi.net/wiki/Adlib_sound_effect#Pitch_Data
        //

        auto b = adlsnd.data[i];

        if (!b)
        {
            _opl.WriteReg(0xB0, b);
            note_on = false;
        }
        else
        {
            _opl.WriteReg(0xA0, b);

            if (!note_on)
            {
                _opl.WriteReg(0xB0, block | 0x20);
                note_on = true;
            }
        }

        // GenerateBlock2 = OPL2, GenerateBlock3 = OPL3
        _opl.chip.GenerateBlock2(SAMPLES_PER_ADLIB_TICK, src_ptr);

        // for every 32 bit mono sample from src_ptr, write a 16 bit stereo sample into dst_buffer
        for (auto p = src_ptr; p < src_ptr + SAMPLES_PER_ADLIB_TICK; p++, dst_buffer_idx++)
        {
            // take the sample and multiply by 4 to make it nice n loud
            auto sample = *p << 2;

            if (sample > SHRT_MAX)
            {
                sample = SHRT_MAX;
            }
            else if (sample < SHRT_MIN)
            {
                sample = SHRT_MIN;
            }

            // convert 32 bit mono samples to 16 bit stereo
            dst_buffer[dst_buffer_idx * 2] = dst_buffer[dst_buffer_idx * 2 + 1] = sample;
        }

        // start writing SAMPLES_PER_ADLIB_TICK ahead the next iteration
        src_ptr += SAMPLES_PER_ADLIB_TICK;
    }

    Mix_Chunk chunk;
    chunk.abuf = (uint8_t *)dst_buffer;
    chunk.alen = adlsnd_length * SAMPLES_PER_ADLIB_TICK * 2;
    chunk.allocated = false;
    chunk.volume = 128;

    Mix_PlayChannel(0, &chunk, 0);

    puts("press any key to continue");
    std::getchar();

    delete[] dst_buffer;
    delete[] src_buffer;

    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
