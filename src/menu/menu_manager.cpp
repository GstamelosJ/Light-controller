#include "../common/event_factory.h"
#include "menu_manager.h"
#include "../common/globals.h"
#include "../common/types.h"

static bool isCustomDays(uint8_t mask)
{
    return
        mask != EVERYDAY_MASK &&
        mask != WEEKDAYS &&
        mask != WEEKEND;
}

static EventEditField firstFieldFor(
    TriggerType trigger)
{
    return EDIT_TRIGGER;
}

static EventEditField nextFieldFor(
    TriggerType trigger,
    EventEditField current)
{
    if(trigger == FIXED_TIME)
    {
        switch(current)
        {
            case EDIT_TRIGGER:
                return EDIT_HOUR;

            case EDIT_HOUR:
                return EDIT_MINUTE;

            case EDIT_MINUTE:
                return EDIT_ACTION;

            case EDIT_ACTION:
                return EDIT_DAYS;

            case EDIT_DAYS:
                return EDIT_ENABLED;

            case EDIT_ENABLED:
            default:
                return EDIT_TRIGGER;
        }
    }

    switch(current)
    {
        case EDIT_TRIGGER:
            return EDIT_OFFSET;

        case EDIT_OFFSET:
            return EDIT_ACTION;

        case EDIT_ACTION:
            return EDIT_DAYS;

        case EDIT_DAYS:
            return EDIT_ENABLED;

        case EDIT_ENABLED:
        default:
            return EDIT_TRIGGER;
    }
}

static void cycleDaysPresetForward(
    ScheduleEvent& ev)
{
    if(ev.daysMask == EVERYDAY_MASK)
    {
        ev.daysMask = WEEKDAYS;
    }
    else if(ev.daysMask == WEEKDAYS)
    {
        ev.daysMask = WEEKEND;
    }
    else if(ev.daysMask == WEEKEND)
    {
        ev.daysMask = 0;
    }
    else
    {
        ev.daysMask = EVERYDAY_MASK;
    }
}

static void cycleDaysPresetBackward(
    ScheduleEvent& ev)
{
    if(ev.daysMask == EVERYDAY_MASK)
    {
        ev.daysMask = 0;
    }
    else if(ev.daysMask == WEEKDAYS)
    {
        ev.daysMask = EVERYDAY_MASK;
    }
    else if(ev.daysMask == WEEKEND)
    {
        ev.daysMask = WEEKDAYS;
    }
    else
    {
        ev.daysMask = WEEKEND;
    }
}
//Method for deleting events
static void deleteSelectedEvent()
{
    ChannelConfig& cfg =
        Config.config.channels[
            UI.selectedChannel];

    if(UI.selectedEvent >=
       cfg.eventCount)
    {
        return;
    }

    for(uint8_t i =
            UI.selectedEvent;
        i + 1 <
            cfg.eventCount;
        i++)
    {
        cfg.events[i] =
            cfg.events[i + 1];
    }

    if(cfg.eventCount > 0)
    {
        cfg.eventCount--;
    }

    if(cfg.eventCount == 0)
    {
        UI.selectedEvent = 0;
    }
    else if(UI.selectedEvent >=
            cfg.eventCount)
    {
        UI.selectedEvent =
            cfg.eventCount - 1;
    }

    Config.save();
}


void MenuManager::begin()
{
    UI.currentScreen =
        UI_STATUS;

    UI.lastScreen =
        UI_STATUS;

    UI.selectedChannel = 0;
    UI.selectedEvent = 0;
    UI.selectedDay = 0;

    UI.editField =
        EDIT_TRIGGER;
}

void MenuManager::update()
{
    if(UI.currentScreen !=
       UI.lastScreen)
    {
        LCD.clear();

        UI.lastScreen =
            UI.currentScreen;
    }

    switch(UI.currentScreen)
    {
        case UI_STATUS:
            LCD.showStatus();
            break;

        case UI_CHANNEL_LIST:
            LCD.showChannelList();
            break;

        case UI_CHANNEL:
            LCD.showChannel();
            break;

        case UI_EVENT_LIST:
            LCD.showEventList();
            break;

        case UI_EVENT_EDIT:
            LCD.showEventEditor();
            break;

        case UI_DAYS_EDIT:
            LCD.showDaysEditor();
            break;
		case UI_EVENT_ACTION:
			LCD.showEventAction();
			break;

        default:
            break;
    }
}

void MenuManager::applyChannelMode(
    uint8_t channel)
{
    if(channel >= MAX_CHANNELS)
    {
        return;
    }

    ChannelConfig& ch =
        Config.config.channels[channel];

    switch(ch.mode)
    {
        case MANUAL_ON:

            Relays.set(
                channel,
                true);

            break;

        case MANUAL_OFF:

            Relays.set(
                channel,
                false);

            break;

        case DISABLED_MODE:

            Relays.set(
                channel,
                false);

            break;

        case AUTO_MODE:
        default:

            break;
    }
}

void MenuManager::buttonOK()
{
    switch(UI.currentScreen)
    {
        case UI_STATUS:

            UI.currentScreen =
                UI_CHANNEL_LIST;

            break;

        case UI_CHANNEL_LIST:
        {
            UI.pendingMode =
                Config.config
                    .channels[
                        UI.selectedChannel]
                    .mode;

            UI.currentScreen =
                UI_CHANNEL;

            break;
        }

        case UI_CHANNEL:
        {
            ChannelConfig& ch =
                Config.config
                    .channels[
                        UI.selectedChannel];

            if(ch.mode !=
               UI.pendingMode)
            {
                ch.mode =
                    UI.pendingMode;

                applyChannelMode(
                    UI.selectedChannel);

                Config.save();
            }
            else
            {
                UI.selectedEvent = 0;

                UI.currentScreen =
                    UI_EVENT_LIST;
            }

            break;
        }

		case UI_EVENT_LIST:
		{
			ChannelConfig& cfg =
				Config.config.channels[
					UI.selectedChannel];

			/*
			 * + ADD EVENT
			 */
			if(UI.selectedEvent ==
			   cfg.eventCount)
			{
				if(cfg.eventCount <
				   MAX_EVENTS)
				{
					cfg.events[
						cfg.eventCount] =
						createFixedEvent(
							18,
							0,
							true);

					UI.selectedEvent =
						cfg.eventCount;

					cfg.eventCount++;

					UI.editField =
						EDIT_TRIGGER;

					Config.save();

					UI.currentScreen =
						UI_EVENT_EDIT;
				}

				break;
			}

			/*
			 * Existing event:
			 * EDIT / DELETE / BACK
			 */
			if(UI.selectedEvent <
			   cfg.eventCount)
			{
				UI.selectedEventAction =
					EVENT_ACTION_EDIT;

				UI.currentScreen =
					UI_EVENT_ACTION;
			}

			break;
		}

        case UI_EVENT_EDIT:
        {
            ChannelConfig& cfg =
                Config.config
                    .channels[
                        UI.selectedChannel];

            if(UI.selectedEvent >=
               cfg.eventCount)
            {
                UI.currentScreen =
                    UI_EVENT_LIST;

                break;
            }

            ScheduleEvent& ev =
                cfg.events[
                    UI.selectedEvent];

            if(UI.editField ==
               EDIT_DAYS)
            {
                if(isCustomDays(
                       ev.daysMask))
                {
                    UI.selectedDay = 0;

                    UI.currentScreen =
                        UI_DAYS_EDIT;
                }
                else
                {
                    UI.editField =
                        nextFieldFor(
                            ev.trigger,
                            UI.editField);
                }

                break;
            }

                UI.editField =
                nextFieldFor(
                    ev.trigger,
                    UI.editField);

            Config.save();

            break;
        }

        case UI_DAYS_EDIT:
        {
            ChannelConfig& cfg =
                Config.config
                    .channels[
                        UI.selectedChannel];

            if(UI.selectedEvent >=
               cfg.eventCount)
            {
                UI.currentScreen =
                    UI_EVENT_LIST;

                break;
            }

            ScheduleEvent& ev =
                cfg.events[
                    UI.selectedEvent];

            if(UI.selectedDay == 0)
            {
                ev.daysMask =
                    EVERYDAY_MASK;

                Config.save();

                break;
            }

            static const uint8_t masks[7] =
            {
                SUNDAY_MASK,
                MONDAY_MASK,
                TUESDAY_MASK,
                WEDNESDAY_MASK,
                THURSDAY_MASK,
                FRIDAY_MASK,
                SATURDAY_MASK
            };

            ev.daysMask ^=
                masks[
                    UI.selectedDay - 1];

            Config.save();

            break;
        }
		case UI_EVENT_ACTION:
		{
			switch(UI.selectedEventAction)
			{
				case EVENT_ACTION_EDIT:

					UI.editField =
						EDIT_TRIGGER;

					UI.currentScreen =
						UI_EVENT_EDIT;

					break;

				case EVENT_ACTION_DELETE:

					deleteSelectedEvent();

					UI.currentScreen =
						UI_EVENT_LIST;

					break;

				case EVENT_ACTION_BACK:

					UI.currentScreen =
						UI_EVENT_LIST;

					break;

				default:
					break;
			}

			break;
		}

        default:
            break;
    }
}

void MenuManager::buttonESC()
{
    switch(UI.currentScreen)
    {
        case UI_CHANNEL_LIST:

            UI.currentScreen =
                UI_STATUS;

            break;

        case UI_CHANNEL:

            UI.currentScreen =
                UI_CHANNEL_LIST;

            break;

        case UI_EVENT_LIST:

            UI.currentScreen =
                UI_CHANNEL;

            break;

        case UI_EVENT_EDIT:

            UI.currentScreen =
                UI_EVENT_LIST;

            break;

        case UI_DAYS_EDIT:
		{
			ScheduleEvent& ev =
				Config.config
					.channels[
						UI.selectedChannel]
					.events[
						UI.selectedEvent];

			UI.editField =
				nextFieldFor(
					ev.trigger,
					EDIT_DAYS);

			UI.currentScreen =
				UI_EVENT_EDIT;

			break;
		}
		case UI_EVENT_ACTION:

			UI.currentScreen =
				UI_EVENT_LIST;

			break;
        default:
            break;
    }
}

void MenuManager::buttonUp()
{
    switch(UI.currentScreen)
    {
        case UI_CHANNEL_LIST:
        {
            if(UI.selectedChannel > 0)
            {
                UI.selectedChannel--;
            }

            break;
        }

        case UI_CHANNEL:
        {
            if(UI.pendingMode >
               AUTO_MODE)
            {
                UI.pendingMode =
                    (ChannelMode)
                    ((int)UI.pendingMode - 1);
            }

            break;
        }

        case UI_EVENT_LIST:
        {
            if(UI.selectedEvent > 0)
            {
                UI.selectedEvent--;
            }

            break;
        }

        case UI_EVENT_EDIT:
        {
            ChannelConfig& cfg =
                Config.config
                    .channels[
                        UI.selectedChannel];

            if(UI.selectedEvent >=
               cfg.eventCount)
            {
                break;
            }

            ScheduleEvent& ev =
                cfg.events[
                    UI.selectedEvent];

            if(UI.editField ==
               EDIT_TRIGGER)
            {
                if(ev.trigger <
                   SUNSET)
                {
                    ev.trigger =
                        (TriggerType)
                        ((int)ev.trigger + 1);
                }

                UI.editField =
                    firstFieldFor(
                        ev.trigger);
            }
            else if(UI.editField ==
                    EDIT_HOUR)
            {
                ev.hour++;

                if(ev.hour > 23)
                {
                    ev.hour = 0;
                }
            }
            else if(UI.editField ==
                    EDIT_MINUTE)
            {
                ev.minute++;

                if(ev.minute > 59)
                {
                    ev.minute = 0;
                }
            }
            else if(UI.editField ==
                    EDIT_OFFSET)
            {
                if(ev.offsetMinutes < 180)
                {
                    ev.offsetMinutes++;
                }
            }
            else if(UI.editField ==
                    EDIT_ACTION)
            {
                ev.action =
                    !ev.action;
            }
            else if(UI.editField ==
                    EDIT_DAYS)
            {
                cycleDaysPresetForward(
                    ev);
            }
            else if(UI.editField ==
                    EDIT_ENABLED)
            {
                ev.enabled =
                    !ev.enabled;
            }

            Config.save();

            break;
        }

        case UI_DAYS_EDIT:
        {
            if(UI.selectedDay > 0)
            {
                UI.selectedDay--;
            }

            break;
        }
		case UI_EVENT_ACTION:
		{
			if(UI.selectedEventAction >
			   EVENT_ACTION_EDIT)
			{
				UI.selectedEventAction =
					(EventActionItem)
					((int)UI.selectedEventAction - 1);
			}

			break;
		}
        default:
            break;
    }
}

void MenuManager::buttonDown()
{
    switch(UI.currentScreen)
    {
        case UI_CHANNEL_LIST:
        {
            if(UI.selectedChannel <
               (MAX_CHANNELS - 1))
            {
                UI.selectedChannel++;
            }

            break;
        }

        case UI_CHANNEL:
        {
            if(UI.pendingMode <
               DISABLED_MODE)
            {
                UI.pendingMode =
                    (ChannelMode)
                    ((int)UI.pendingMode + 1);
            }

            break;
        }

        case UI_EVENT_LIST:
        {
            ChannelConfig& cfg =
                Config.config
                    .channels[
                        UI.selectedChannel];

            uint8_t maxSelectable = 0;

            if(cfg.eventCount < MAX_EVENTS)
            {
                maxSelectable =
                    cfg.eventCount;
            }
            else if(cfg.eventCount > 0)
            {
                maxSelectable =
                    cfg.eventCount - 1;
            }

            if(UI.selectedEvent <
               maxSelectable)
            {
                UI.selectedEvent++;
            }

            break;
        }

        case UI_EVENT_EDIT:
        {
            ChannelConfig& cfg =
                Config.config
                    .channels[
                        UI.selectedChannel];

            if(UI.selectedEvent >=
               cfg.eventCount)
            {
                break;
            }

            ScheduleEvent& ev =
                cfg.events[
                    UI.selectedEvent];

            if(UI.editField ==
               EDIT_TRIGGER)
            {
                if(ev.trigger >
                   FIXED_TIME)
                {
                    ev.trigger =
                        (TriggerType)
                        ((int)ev.trigger - 1);
                }

                UI.editField =
                    firstFieldFor(
                        ev.trigger);
            }
            else if(UI.editField ==
                    EDIT_HOUR)
            {
                if(ev.hour == 0)
                {
                    ev.hour = 23;
                }
                else
                {
                    ev.hour--;
                }
            }
            else if(UI.editField ==
                    EDIT_MINUTE)
            {
                if(ev.minute == 0)
                {
                    ev.minute = 59;
                }
                else
                {
                    ev.minute--;
                }
            }
            else if(UI.editField ==
                    EDIT_OFFSET)
            {
                if(ev.offsetMinutes > -180)
                {
                    ev.offsetMinutes--;
                }
            }
            else if(UI.editField ==
                    EDIT_ACTION)
            {
                ev.action =
                    !ev.action;
            }
            else if(UI.editField ==
                    EDIT_DAYS)
            {
                cycleDaysPresetBackward(
                    ev);
            }
            else if(UI.editField ==
                    EDIT_ENABLED)
            {
                ev.enabled =
                    !ev.enabled;
            }

            Config.save();

            break;
        }

        case UI_DAYS_EDIT:
        {
            if(UI.selectedDay < 7)
            {
                UI.selectedDay++;
            }

            break;
        }
		case UI_EVENT_ACTION:
		{
			if(UI.selectedEventAction <
			   EVENT_ACTION_BACK)
			{
				UI.selectedEventAction =
					(EventActionItem)
					((int)UI.selectedEventAction + 1);
			}

			break;
		}
        default:
            break;
    }
}