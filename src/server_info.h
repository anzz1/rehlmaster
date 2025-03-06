#pragma once

#include <extdll.h>
#include <meta_api.h>
#include "ex_rehlds_api.h"

typedef struct GameToAppIDMapItem_s
{
	unsigned int iAppID;
	const char *pGameDir;
} GameToAppIDMapItem_t;

class CServerInfo;

class CServerInfo
{
public:
	CServerInfo();

	void serverActivate(int maxclients);
	int getMaxPlayers() const;
	const char* getHostName() const;
	static const char* getMapName();
	const char* getGameDir() const;
	const char* getGameDescription() const;
	size_t getPlayersCount();
	size_t getBotsCount() const;
	int getVisibleMaxPlayers() const;
	static char getOS();
	uint8 getIsPasswordSet() const;
	static uint8 getSecure();
	unsigned short getPort() const;
	unsigned short getAppId() const;
	const char* getAppVersion() const;
	const char* getProductName() const;
	uint8 getIsLan() const;

private:
	unsigned short parseAppId();
	void parseAppVersion();

private:
	int m_maxPlayers;
	cvar_t *m_pcv_hostname;
	cvar_t *m_pcv_sv_visiblemaxplayers;
	cvar_t *m_pcv_sv_password;
	cvar_t *m_pcv_sv_lan;
	DLL_FUNCTIONS *m_pEntityInterface;
	unsigned short m_port;
	unsigned short m_appId;
	char m_appVersion[32];
	char m_productName[32];
	char m_gameDir[MAX_PATH];
};

extern CServerInfo* g_ServerInfo;
extern void serverinfo_init();
extern void serverinfo_uninit();