#pragma once

#include "fastfetch.h"

#define FF_WM_MODULE_NAME "WM"

void ffPrintWM(FFWMOptions* options);
void ffInitWMOptions(FFWMOptions* options);
bool ffParseWMCommandOptions(FFWMOptions* options, const char* key, const char* value);
void ffDestroyWMOptions(FFWMOptions* options);
void ffParseWMJsonObject(yyjson_val* module);
