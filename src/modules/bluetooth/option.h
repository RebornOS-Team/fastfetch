#pragma once

// This file will be included in "fastfetch.h", do NOT put unnecessary things here

#include "common/option.h"

typedef struct FFBluetoothOptions
{
    const char* moduleName;
    FFModuleArgs moduleArgs;

    bool showDisconnected;
} FFBluetoothOptions;
