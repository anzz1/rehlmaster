#include <extdll.h>
#include <meta_api.h>

#include "ex_rehlds_api.h"
#include "rehlmaster.h"
#include "server_info.h"

meta_globals_t *gpMetaGlobals;
gamedll_funcs_t *gpGamedllFuncs;
mutil_funcs_t *gpMetaUtilFuncs;
enginefuncs_t *g_pengfuncsTable;

plugin_info_t Plugin_info =
{
	META_INTERFACE_VERSION,						// ifvers
	"rehlmaster",								// name
	"0.0.1",									// version
	__DATE__,									// date
	"anzz1",									// author
	"http://github.com/anzz1/rehlmaster",		// url
	"MAST",										// logtag
	PT_STARTUP,									// (when) loadable
	PT_ANYTIME,									// (when) unloadable
};

C_DLLEXPORT int Meta_Query(char *interfaceVersion, plugin_info_t **plinfo, mutil_funcs_t *pMetaUtilFuncs)
{
	*plinfo = &Plugin_info;
	gpMetaUtilFuncs = pMetaUtilFuncs;
	return TRUE;
}

META_FUNCTIONS gMetaFunctionTable =
{
	NULL,						// pfnGetEntityAPI				HL SDK; called before game DLL
	NULL,						// pfnGetEntityAPI_Post			META; called after game DLL
	GetEntityAPI2,				// pfnGetEntityAPI2				HL SDK2; called before game DLL
	NULL,						// pfnGetEntityAPI2_Post		META; called after game DLL
	NULL,						// pfnGetNewDLLFunctions		HL SDK2; called before game DLL
	NULL,						// pfnGetNewDLLFunctions_Post	META; called after game DLL
	GetEngineFunctions,			// pfnGetEngineFunctions		META; called before HL engine
	NULL,						// pfnGetEngineFunctions_Post	META; called after HL engine
};

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable, meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs)
{
	gpMetaGlobals = pMGlobals;
	gpGamedllFuncs = pGamedllFuncs;

	if(!meta_init_rehlds_api()) {
		SERVER_PRINT("ReHLDS API failed to initialize!\n");
		return FALSE;
	}

	serverinfo_init();
	rehlmaster_init();

	memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));
	return TRUE;
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason)
{
	rehlmaster_uninit();
	serverinfo_uninit();

	return TRUE;
}

