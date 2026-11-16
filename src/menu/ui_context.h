#pragma once

#include <stdint.h>

#include "../common/types.h"

/*
 * Available UI screens.
 */
enum UIScreen
{
    UI_STATUS,
    UI_CHANNEL_LIST,
    UI_CHANNEL,
    UI_EVENT_LIST,
    UI_EVENT_ACTION,
    UI_EVENT_EDIT,
    UI_DAYS_EDIT
};

/*
 * Editable fields of a schedule event.
 *
 * DELETE is intentionally not included here.
 * Event deletion is handled by UI_EVENT_ACTION.
 */
enum EventEditField
{
    EDIT_TRIGGER,
    EDIT_HOUR,
    EDIT_MINUTE,
    EDIT_OFFSET,
    EDIT_ACTION,
    EDIT_DAYS,
    EDIT_ENABLED
};

/*
 * Actions available after selecting
 * an existing event from the event list.
 */
enum EventActionItem
{
    EVENT_ACTION_EDIT,
    EVENT_ACTION_DELETE,
    EVENT_ACTION_BACK
};

/*
 * Runtime UI state.
 *
 * This structure is not stored in Preferences.
 * It only keeps the current navigation and
 * editing state of the local user interface.
 */
struct UIContext
{
    /*
     * Current and previous screens.
     *
     * lastScreen is used by MenuManager
     * to clear the LCD only when the
     * displayed screen changes.
     */
    UIScreen currentScreen =
        UI_STATUS;

    UIScreen lastScreen =
        UI_STATUS;

    /*
     * Current channel selection:
     * 0...7 correspond to CH1...CH8.
     */
    uint8_t selectedChannel = 0;

    /*
     * Current event selection:
     * 0...15 correspond to Event 1...16.
     *
     * In UI_EVENT_LIST, the value may equal
     * eventCount to represent "+ ADD EVENT".
     */
    uint8_t selectedEvent = 0;

    /*
     * Selected item in the Days Editor:
     *
     * 0 = ALL DAYS
     * 1 = SUN
     * 2 = MON
     * 3 = TUE
     * 4 = WED
     * 5 = THU
     * 6 = FRI
     * 7 = SAT
     */
    uint8_t selectedDay = 0;

    /*
     * Current editable event field.
     */
    EventEditField editField =
        EDIT_TRIGGER;

    /*
     * Current selection in the event
     * action screen:
     *
     * EDIT / DELETE / BACK
     */
    EventActionItem selectedEventAction =
        EVENT_ACTION_EDIT;

    /*
     * Temporary channel mode selection.
     *
     * UP/DOWN modify pendingMode only.
     * The actual ChannelConfig.mode and
     * relay state change only after OK.
     */
    ChannelMode pendingMode =
        AUTO_MODE;
};