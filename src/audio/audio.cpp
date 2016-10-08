﻿//
//  Lol Engine
//
//  Copyright © 2010—2016 Sam Hocevar <sam@hocevar.net>
//
//  Lol Engine is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#include <lol/engine-internal.h>

#if LOL_USE_SDL_MIXER
#   if HAVE_SDL_SDL_H
#       include <SDL/SDL.h>
#       include <SDL/SDL_mixer.h>
#   elif HAVE_SDL2_SDL_H
#       include <SDL2/SDL.h>
#       include <SDL2/SDL_mixer.h>
#   else
#       include <SDL.h>
#       include <SDL_mixer.h>
#   endif
#endif

namespace lol
{

/*
 * Public audio class
 */

void audio::init()
{
#if defined LOL_USE_SDL_MIXER
    Mix_OpenAudio(22050, AUDIO_S16, 8, 1024);
#endif
}

void audio::set_channels(int channels)
{
#if defined LOL_USE_SDL_MIXER
    Mix_AllocateChannels(channels);
#else
    UNUSED(channels);
#endif
}

void audio::set_volume(int channel, int volume)
{
#if defined LOL_USE_SDL_MIXER
    Mix_Volume(channel,volume);
#else
    UNUSED(channel);
#endif
}

void audio::mute_all()
{
#if defined LOL_USE_SDL_MIXER
    Mix_Volume(-1,0);
#else
    UNUSED(false);
#endif
}

void audio::unmute_all()
{
#if defined LOL_USE_SDL_MIXER
    Mix_Volume(-1,MIX_MAX_VOLUME);
#else
    UNUSED(false);
#endif
}

} /* namespace lol */
