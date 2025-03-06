#pragma once

#include <rehlds_api.h>
#include "sizebuf.h"
#include "utils.h"

#define HLDS_APPID				90
#define HLDS_APPVERSION			"1.1.2.7/Stdio"
#define PORT_SERVER				27015
#define PORT_MASTER				27010
#define HEARTBEAT_TIME			180.0

extern IRehldsApi* g_RehldsApi;
extern const RehldsFuncs_t* g_RehldsFuncs;
extern IRehldsHookchains* g_RehldsHookchains;
extern IRehldsServerStatic* g_RehldsSvs;

extern bool meta_init_rehlds_api();
