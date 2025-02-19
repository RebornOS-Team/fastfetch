extern "C"
{
#include "brightness.h"
#include "detection/displayserver/displayserver.h"
}
#include "util/windows/wmi.hpp"
#include "util/windows/unicode.hpp"

#include <highlevelmonitorconfigurationapi.h>
#include <physicalmonitorenumerationapi.h>

static const char* detectWithWmi(FFlist* result)
{
    FFWmiQuery query(L"SELECT CurrentBrightness, InstanceName FROM WmiMonitorBrightness WHERE Active = true", nullptr, FFWmiNamespace::WMI);
    if(!query)
        return "Query WMI service failed";

    while(FFWmiRecord record = query.next())
    {
        if(FFWmiVariant vtValue = record.get(L"CurrentBrightness"))
        {
            FFBrightnessResult* brightness = (FFBrightnessResult*) ffListAdd(result);
            brightness->value = vtValue.get<uint8_t>();

            ffStrbufInit(&brightness->name);
            if (FFWmiVariant vtName = record.get(L"InstanceName"))
            {
                ffStrbufSetWSV(&brightness->name, vtName.get<std::wstring_view>());
                ffStrbufSubstrAfterFirstC(&brightness->name, '\\');
                ffStrbufSubstrBeforeFirstC(&brightness->name, '\\');
            }
        }
    }
    return NULL;
}

static char* detectWithDdcci(const FFDisplayServerResult* displayServer, FFlist* result)
{
    FF_LIST_FOR_EACH(FFDisplayResult, display, displayServer->displays)
    {
        PHYSICAL_MONITOR physicalMonitor;
        if (GetPhysicalMonitorsFromHMONITOR((HMONITOR)(uintptr_t) display->id, 1, &physicalMonitor))
        {
            DWORD min = 0, curr = 0, max = 0;
            if (GetMonitorBrightness(physicalMonitor.hPhysicalMonitor, &min, &curr, &max))
            {
                FFBrightnessResult* brightness = (FFBrightnessResult*) ffListAdd(result);

                if (display->name.length)
                    ffStrbufInitCopy(&brightness->name, &display->name);
                else
                    ffStrbufInitWS(&brightness->name, physicalMonitor.szPhysicalMonitorDescription);

                brightness->value = (float) (curr - min) * 100.f / (float) (max - min);
            }
        }
    }
    return NULL;
}

static bool hasBuiltinDisplay(const FFDisplayServerResult* displayServer)
{
    FF_LIST_FOR_EACH(FFDisplayResult, display, displayServer->displays)
    {
        if (display->type == FF_DISPLAY_TYPE_BUILTIN || display->type == FF_DISPLAY_TYPE_UNKNOWN)
            return true;
    }
    return false;
}

extern "C"
const char* ffDetectBrightness(FFlist* result)
{
    const FFDisplayServerResult* displayServer = ffConnectDisplayServer();

    if (hasBuiltinDisplay(displayServer))
        detectWithWmi(result);

    if (instance.config.allowSlowOperations && result->length < displayServer->displays.length)
        detectWithDdcci(displayServer, result);
    return NULL;
}
