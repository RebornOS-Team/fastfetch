#include "common/printing.h"
#include "common/jsonconfig.h"
#include "detection/media/media.h"
#include "modules/media/media.h"
#include "util/stringUtils.h"

#include <ctype.h>

#define FF_MEDIA_NUM_FORMAT_ARGS 5

static inline bool shouldIgnoreChar(char c)
{
    return isblank(c) || c == '-' || c == '.';
}

static bool artistInSongTitle(const FFstrbuf* song, const FFstrbuf* artist)
{
    uint32_t artistIndex = 0;
    uint32_t songIndex = 0;

    while(true)
    {
        while(shouldIgnoreChar(song->chars[songIndex]))
            ++songIndex;

        while(shouldIgnoreChar(artist->chars[artistIndex]))
            ++artistIndex;

        if(artist->chars[artistIndex] == '\0')
            return true;

        if(song->chars[songIndex] == '\0')
            return false;

        if(tolower(song->chars[songIndex]) != tolower(artist->chars[artistIndex]))
            return false;

        ++artistIndex;
        ++songIndex;
    }

    //Unreachable
    return false;
}

void ffPrintMedia(FFMediaOptions* options)
{
    const FFMediaResult* media = ffDetectMedia();

    if(media->error.length > 0)
    {
        ffPrintError(FF_MEDIA_MODULE_NAME, 0, &options->moduleArgs, "%s", media->error.chars);
        return;
    }

    FF_STRBUF_AUTO_DESTROY songPretty = ffStrbufCreateCopy(&media->song);
    const char* removeStrings[] = {
        "(Official Music Video)", "(Official Video)", "(Music Video)", "(Official HD Video)",
        "[Official Music Video]", "[Official Video]", "[Music Video]", "[Official HD Video]",
        "| Official Music Video", "| Official Video", "| Music Video", "| Official HD Video",
        "[Official Audio]", "[Audio]", "(Audio)", "| Official Audio", "| Audio", "| OFFICIAL AUDIO",
        "(Lyric Video)", "(Official Lyric Video)", "(Lyrics)",
        "[Lyric Video]", "[Official Lyric Video]", "[Lyrics]",
        "| Lyric Video", "| Official Lyric Video", "| Lyrics",
    };
    ffStrbufRemoveStrings(&songPretty, sizeof(removeStrings) / sizeof(removeStrings[0]), removeStrings);
    ffStrbufTrimRight(&songPretty, ' ');

    if(songPretty.length == 0)
        ffStrbufAppend(&songPretty, &media->song);

    if(options->moduleArgs.outputFormat.length == 0)
    {
        //We don't expose artistPretty to the format, as it might be empty (when the think that the artist is already in the song title)
        FF_STRBUF_AUTO_DESTROY artistPretty = ffStrbufCreateCopy(&media->artist);
        ffStrbufRemoveIgnCaseEndS(&artistPretty, " - Topic");
        ffStrbufRemoveIgnCaseEndS(&artistPretty, "VEVO");
        ffStrbufTrimRight(&artistPretty, ' ');

        if(artistInSongTitle(&songPretty, &artistPretty))
            ffStrbufClear(&artistPretty);

        ffPrintLogoAndKey(FF_MEDIA_MODULE_NAME, 0, &options->moduleArgs.key, &options->moduleArgs.keyColor);

        if(artistPretty.length > 0)
        {
            ffStrbufWriteTo(&artistPretty, stdout);
            fputs(" - ", stdout);
        }

        if (media->status.length > 0)
            ffStrbufAppendF(&songPretty, " (%s)", media->status.chars);

        ffStrbufPutTo(&songPretty, stdout);
    }
    else
    {
        ffPrintFormat(FF_MEDIA_MODULE_NAME, 0, &options->moduleArgs, FF_MEDIA_NUM_FORMAT_ARGS, (FFformatarg[]){
            {FF_FORMAT_ARG_TYPE_STRBUF, &songPretty},
            {FF_FORMAT_ARG_TYPE_STRBUF, &media->song},
            {FF_FORMAT_ARG_TYPE_STRBUF, &media->artist},
            {FF_FORMAT_ARG_TYPE_STRBUF, &media->album},
            {FF_FORMAT_ARG_TYPE_STRBUF, &media->status}
        });
    }
}

void ffInitMediaOptions(FFMediaOptions* options)
{
    options->moduleName = FF_MEDIA_MODULE_NAME;
    ffOptionInitModuleArg(&options->moduleArgs);
}

bool ffParseMediaCommandOptions(FFMediaOptions* options, const char* key, const char* value)
{
    const char* subKey = ffOptionTestPrefix(key, FF_MEDIA_MODULE_NAME);
    if (!subKey) return false;
    if (ffOptionParseModuleArgs(key, subKey, value, &options->moduleArgs))
        return true;

    return false;
}

void ffDestroyMediaOptions(FFMediaOptions* options)
{
    ffOptionDestroyModuleArg(&options->moduleArgs);
}

void ffParseMediaJsonObject(yyjson_val* module)
{
    FFMediaOptions __attribute__((__cleanup__(ffDestroyMediaOptions))) options;
    ffInitMediaOptions(&options);

    if (module)
    {
        yyjson_val *key_, *val;
        size_t idx, max;
        yyjson_obj_foreach(module, idx, max, key_, val)
        {
            const char* key = yyjson_get_str(key_);
            if(ffStrEqualsIgnCase(key, "type"))
                continue;

            if (ffJsonConfigParseModuleArgs(key, val, &options.moduleArgs))
                continue;

            ffPrintError(FF_MEDIA_MODULE_NAME, 0, &options.moduleArgs, "Unknown JSON key %s", key);
        }
    }

    ffPrintMedia(&options);
}
