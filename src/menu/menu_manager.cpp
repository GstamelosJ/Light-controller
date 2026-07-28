#include "../common/event_factory.h"
#include "menu_manager.h"
#include "../common/globals.h"
#include "../common/types.h"

void MenuManager::begin()
{
    UI.currentScreen = UI_STATUS;

    UI.selectedChannel = 0;

    UI.selectedEvent = 0;

    UI.editField = EDIT_TRIGGER;
}
void MenuManager::update()
{
	if(UI.currentScreen != UI.lastScreen)
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
				Config.config.channels[
					UI.selectedChannel]
					.mode;

			UI.currentScreen =
				UI_CHANNEL;

			break;
		}

        case UI_CHANNEL:
		{
			ChannelConfig& ch =
				Config.config.channels[
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

            UI.editField =
                EDIT_TRIGGER;

            UI.currentScreen =
                UI_EVENT_EDIT;

            break;

        case UI_EVENT_EDIT:

            if(UI.editField ==
               EDIT_ENABLED)
            {
                UI.editField =
                    EDIT_TRIGGER;

                Config.save();
            }
            else
            {
                UI.editField =
                    (EventEditField)
                    ((int)UI.editField + 1);
            }

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

        default:

            break;
    }
}
void MenuManager::applyChannelMode(
    uint8_t channel)
{
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
            ScheduleEvent& ev =
                Config.config
                      .channels[
                        UI.selectedChannel]
                      .events[
                        UI.selectedEvent];
						if(UI.editField ==
               EDIT_TRIGGER)
            {
                if(ev.trigger < SUNSET)
                {
                    ev.trigger =
                        (TriggerType)
                        ((int)ev.trigger + 1);
                }
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
                if(ev.daysMask ==
                   EVERYDAY_MASK)
                {
                    ev.daysMask =
                        WEEKDAYS;
                }
                else if(ev.daysMask ==
                        WEEKDAYS)
                {
                    ev.daysMask =
                        WEEKEND;
                }
                else
                {
                    ev.daysMask =
                        EVERYDAY_MASK;
                }
            }
			else if(UI.editField ==
                    EDIT_ENABLED)
            {
                ev.enabled =
                    !ev.enabled;
            }

            break;
        }

        default:
            break;
    }
	Config.save();
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
            ChannelConfig& ch =
                Config.config.channels[
                    UI.selectedChannel];

            if(UI.selectedEvent <
               (ch.eventCount - 1))
            {
                UI.selectedEvent++;
            }

            break;
        }
		case UI_EVENT_EDIT:
        {
            ScheduleEvent& ev =
                Config.config
                      .channels[
                        UI.selectedChannel]
                      .events[
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
                if(ev.daysMask ==
                   EVERYDAY_MASK)
                {
                    ev.daysMask =
                        WEEKEND;
                }
                else if(ev.daysMask ==
                        WEEKEND)
                {
                    ev.daysMask =
                        WEEKDAYS;
                }
                else
                {
                    ev.daysMask =
                        EVERYDAY_MASK;
                }
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

        default:
            break;
    }
}
