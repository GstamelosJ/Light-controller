#pragma once

#include "../storage/config_manager.h"
#include "../outputs/relay_manager.h"
#include "../time/time_manager.h"
#include "../astro/astro_manager.h"
#include "../scheduler/scheduler.h"

#include "../menu/ui_context.h"
#include "../menu/lcd_manager.h"
#include "../menu/menu_manager.h"
#include "../drivers/remote_buttons.h"
#include "../drivers/buttons.h"

extern MenuManager Menu;
extern LCDManager LCD;

extern ConfigManager Config;
extern RelayManager Relays;

extern TimeManager TimeMgr;
extern AstroManager Astro;
extern Scheduler Sched;

extern UIContext UI;

extern Buttons Btns;
extern RemoteButtons RemoteKeys;