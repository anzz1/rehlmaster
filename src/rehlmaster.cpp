#include <extdll.h>
#include <meta_api.h>

#include "ex_rehlds_api.h"
#include "server_info.h"

cvar_t cv_sv_master[5] = {
	{"sv_master1", "", FCVAR_EXTDLL, 0.0f, NULL},
	{"sv_master2", "", FCVAR_EXTDLL, 0.0f, NULL},
	{"sv_master3", "", FCVAR_EXTDLL, 0.0f, NULL},
	{"sv_master4", "", FCVAR_EXTDLL, 0.0f, NULL},
	{"sv_master5", "", FCVAR_EXTDLL, 0.0f, NULL}
};

cvar_t* pcv_sv_master[5];

void craft_master_reply(uint8* buf, uint32* len, uint32 challenge)
{
	char tmp[12];
	CSizeBuf szbuf(buf, *len);
	szbuf.WriteString("0\n\\protocol\\48\\challenge\\", false);
	szbuf.WriteString(__itoa(challenge, tmp), false);
	szbuf.WriteString("\\players\\", false);
	szbuf.WriteString(__itoa(g_ServerInfo->getPlayersCount(), tmp), false);
	szbuf.WriteString("\\max\\", false);
	szbuf.WriteString(__itoa(g_ServerInfo->getVisibleMaxPlayers(), tmp), false);
	szbuf.WriteString("\\bots\\", false);
	szbuf.WriteString(__itoa(g_ServerInfo->getBotsCount(), tmp), false);
	szbuf.WriteString("\\gamedir\\", false);
	szbuf.WriteString(g_ServerInfo->getGameDir(), false);
	szbuf.WriteString("\\map\\", false);
	szbuf.WriteString(g_ServerInfo->getMapName(), false);
	szbuf.WriteString("\\type\\d\\password\\", false);
	szbuf.WriteChar(g_ServerInfo->getIsPasswordSet() ? '1' : '0');
	szbuf.WriteString("\\os\\", false);
	szbuf.WriteChar(g_ServerInfo->getOS());
	szbuf.WriteString("\\secure\\", false);
	szbuf.WriteChar(g_ServerInfo->getSecure() ? '1' : '0');
	szbuf.WriteString("\\lan\\", false);
	szbuf.WriteChar(g_ServerInfo->getIsLan() ? '1' : '0');
	szbuf.WriteString("\\version\\", false);
	szbuf.WriteString(g_ServerInfo->getAppVersion(), false);
	szbuf.WriteString("\\region\\255\\product\\", false);
	szbuf.WriteString(g_ServerInfo->getProductName(), false);
	szbuf.WriteChar('\n');
	*len = szbuf.GetCurSize();
}

bool PreprocessPacket_hook(IRehldsHook_PreprocessPacket* chain, uint8* data, unsigned int len, const netadr_t& from) {
	if (len >= 10 && *(uint32_t*)data == 0xFFFFFFFF && data[4] == 0x73 && data[5] == 0x0A) { // MASTER_CHALLENGE_REPLY
		uint8 buf[1024];
		uint32 len = sizeof(buf);
		craft_master_reply(buf, &len, *(uint32*)(data + 6));
		g_RehldsFuncs->NET_SendPacket(len, buf, from);
		ALERT(at_console, "[rehlmaster] replied to master challenge from (%hhu.%hhu.%hhu.%hhu:%hu)\n", from.ip[0], from.ip[1], from.ip[2], from.ip[3], PORT_MASTER);

		return false;
	}

	return chain->callNext(data, len, from);
}

void SV_Frame_hook(IRehldsHook_SV_Frame* chain) {
	chain->callNext();

	double dCurTime = g_RehldsFuncs->GetRealTime();
	static double s_dLastHeartbeat = -HEARTBEAT_TIME + 5.0;

	if (dCurTime - s_dLastHeartbeat > HEARTBEAT_TIME)
	{
		s_dLastHeartbeat = dCurTime;

		for (int i = 0; i < 5; i++) {
			if (!pcv_sv_master[i]->string[0]) continue;
			uint32 ip = host2ip(pcv_sv_master[i]->string);
			if (ip == 0 || ip == -1) {
				ALERT(at_console, "[rehlmaster] sv_master%d:\"%s\" host could not be resolved\n", (i+1), pcv_sv_master[i]->string);
				continue;
			}

			netadr_t netAdr;
			*((uint32*)&netAdr.ip[0]) = ip;
			netAdr.port = (PORT_MASTER >> 8) | (PORT_MASTER << 8); // htons(port)
			netAdr.type = NA_IP;

			g_RehldsFuncs->NET_SendPacket(1, (void*)"q", netAdr);
			ALERT(at_console, "[rehlmaster] sent master heartbeat to (%d) (%hhu.%hhu.%hhu.%hhu:%hu)\n", (i+1), ((unsigned char *)&ip)[0], ((unsigned char *)&ip)[1], ((unsigned char *)&ip)[2], ((unsigned char *)&ip)[3], PORT_MASTER);
		}
	}
}

void rehlmaster_init() {
	for (int i = 0; i < 5; i++) {
		g_engfuncs.pfnCvar_RegisterVariable(&cv_sv_master[i]);
		pcv_sv_master[i] = g_engfuncs.pfnCVarGetPointer(cv_sv_master[i].name);
	}

	g_RehldsHookchains->PreprocessPacket()->registerHook(&PreprocessPacket_hook);
	g_RehldsHookchains->SV_Frame()->registerHook(&SV_Frame_hook);
}

void rehlmaster_uninit() {
	g_RehldsHookchains->PreprocessPacket()->unregisterHook(&PreprocessPacket_hook);
	g_RehldsHookchains->SV_Frame()->unregisterHook(&SV_Frame_hook);
}
