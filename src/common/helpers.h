#pragma once

#include "types.h"
inline const char* triggerToString(
        TriggerType t)
{
    switch(t)
    {
        case FIXED_TIME:
            return "Fixed";

        case SUNRISE:
            return "Sunrise";

        case SUNSET:
            return "Sunset";
    }

    return "";
}
inline const char* modeToString(
        ChannelMode m)
{
    switch(m)
    {
        case AUTO_MODE:
            return "AUTO";

        case MANUAL_ON:
            return "MAN ON";

        case MANUAL_OFF:
            return "MAN OFF";

        case DISABLED_MODE:
            return "DISABLED";
    }

    return "";
}