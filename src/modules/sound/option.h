#pragma once

// This file will be included in "fastfetch.h", do NOT put unnecessary things here

#include "common/option.h"

typedef enum FFSoundType
{
    FF_SOUND_TYPE_MAIN,
    FF_SOUND_TYPE_ACTIVE,
    FF_SOUND_TYPE_ALL,
} FFSoundType;

typedef struct FFSoundOptions
{
    const char* moduleName;
    FFModuleArgs moduleArgs;

    FFSoundType soundType;
} FFSoundOptions;
