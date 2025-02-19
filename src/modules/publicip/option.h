#pragma once

// This file will be included in "fastfetch.h", do NOT put unnecessary things here

#include "common/option.h"

typedef struct FFPublicIpOptions
{
    const char* moduleName;
    FFModuleArgs moduleArgs;

    FFstrbuf url;
    uint32_t timeout;
} FFPublicIpOptions;
