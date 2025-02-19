#pragma once

#include "fastfetch.h"

#define FF_CPU_MODULE_NAME "CPU"

void ffPrintCPU(FFCPUOptions* options);
void ffInitCPUOptions(FFCPUOptions* options);
bool ffParseCPUCommandOptions(FFCPUOptions* options, const char* key, const char* value);
void ffDestroyCPUOptions(FFCPUOptions* options);
void ffParseCPUJsonObject(yyjson_val* module);
