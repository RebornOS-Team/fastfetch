#pragma once

#include "fastfetch.h"

#define FF_GPU_MODULE_NAME "GPU"

void ffPrintGPU(FFGPUOptions* options);
void ffInitGPUOptions(FFGPUOptions* options);
bool ffParseGPUCommandOptions(FFGPUOptions* options, const char* key, const char* value);
void ffDestroyGPUOptions(FFGPUOptions* options);
void ffParseGPUJsonObject(yyjson_val* module);
