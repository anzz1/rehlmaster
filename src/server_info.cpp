#include "server_info.h"

CServerInfo* g_ServerInfo = nullptr;

static GameToAppIDMapItem_t g_GameToAppIDMap[11] = {
	{ 0x0A, "cstrike" },
	{ 0x14, "tfc" },
	{ 0x1E, "dod" },
	{ 0x28, "dmc" },
	{ 0x32, "gearbox" },
	{ 0x3C, "ricochet" },
	{ 0x46, "valve" },
	{ 0x50, "czero" },
	{ 0x64, "czeror" },
	{ 0x82, "bshift" },
	{ 0x96, "cstrike_beta" },
};

CServerInfo::CServerInfo()
{
	m_maxPlayers = 0;
	m_pcv_hostname = nullptr;
	m_pcv_sv_visiblemaxplayers = nullptr;
	m_pcv_sv_password = nullptr;
	m_pcv_sv_lan = nullptr;
	m_pEntityInterface = nullptr;
	m_port = 0;
	m_appId = 0;
	memset(m_appVersion, 0, sizeof(m_appVersion));
	memset(m_productName, 0, sizeof(m_productName));
	memset(m_gameDir, 0, sizeof(m_gameDir));
}

void CServerInfo::serverActivate(int maxclients)
{
	m_maxPlayers = maxclients;

	GET_HOOK_TABLES(PLID, nullptr, &m_pEntityInterface, nullptr);

	m_pcv_hostname = g_engfuncs.pfnCVarGetPointer("hostname");
	m_pcv_sv_visiblemaxplayers = g_engfuncs.pfnCVarGetPointer("sv_visiblemaxplayers");
	m_pcv_sv_password = g_engfuncs.pfnCVarGetPointer("sv_password");
	m_pcv_sv_lan = g_engfuncs.pfnCVarGetPointer("sv_lan");

	m_appId = parseAppId();
	parseAppVersion();
	m_port = atoi(g_engfuncs.pfnCVarGetString("hostport"));

	if (!m_port) {
		m_port = atoi(g_engfuncs.pfnCVarGetString("port"));
		if (!m_port) {
			m_port = PORT_SERVER;
		}
	}

	g_engfuncs.pfnGetGameDir(m_gameDir);
}

const char* CServerInfo::getHostName() const
{
	return m_pcv_hostname->string;
}

const char* CServerInfo::getMapName()
{
	return STRING(gpGlobals->mapname);
}

const char* CServerInfo::getGameDir() const
{
	return m_gameDir;
}

const char* CServerInfo::getGameDescription() const
{
	if (m_pEntityInterface->pfnGetGameDescription)
	{
		return m_pEntityInterface->pfnGetGameDescription();
	}

	return MDLL_GetGameDescription();
}

size_t CServerInfo::getPlayersCount()
{
	size_t count = 0;

	for (int i = 0; i < m_maxPlayers; i++) {
		if (g_RehldsSvs->GetClient(i)->IsConnected())
			count++;
	}

	return count;
}

size_t CServerInfo::getBotsCount() const
{
	size_t count = 0;

	for (int i = 0; i < m_maxPlayers; i++) {
		if (g_RehldsSvs->GetClient(i)->IsFakeClient())
			count++;
	}

	return count;
}

int CServerInfo::getMaxPlayers() const
{
	return m_maxPlayers;
}

int CServerInfo::getVisibleMaxPlayers() const
{
	int visible = atoi(m_pcv_sv_visiblemaxplayers->string);
	return (visible < 0) ? m_maxPlayers : visible;
}

char CServerInfo::getOS()
{
#ifdef _WIN32
	return 'w';
#else
	return 'l';
#endif
}

uint8 CServerInfo::getIsPasswordSet() const
{
	return (m_pcv_sv_password->string[0] && strcmp(m_pcv_sv_password->string, "none")) ? TRUE : FALSE;
}

uint8 CServerInfo::getSecure()
{
	return g_RehldsFuncs->GSBSecure();
}

unsigned short CServerInfo::getPort() const
{
	return m_port;
}

unsigned short CServerInfo::getAppId() const
{
	return m_appId;
}

const char* CServerInfo::getAppVersion() const
{
	return m_appVersion;
}

const char* CServerInfo::getProductName() const
{
	return m_productName[0] ? m_productName : m_gameDir;
}

uint8 CServerInfo::getIsLan() const
{
	return (m_pcv_sv_lan->value != 0.0f) ? TRUE : FALSE;
}

unsigned short CServerInfo::parseAppId()
{
	unsigned short appId = 0;
	char filename[MAX_PATH];
	_snprintf(filename, sizeof(filename), "./%s/steam_appid.txt", m_gameDir);
	filename[sizeof(filename)-1] = 0;

	FILE* fp = fopen(filename, "rt");
	if (fp) {
		char line[256];

		if (fgets(line, sizeof line, fp)) {
			trimbuf(line);
			appId = atoi(line);
		}

		fclose(fp);
	}

	if (!appId) {
		for (int i = 0; i < ARRAYSIZE(g_GameToAppIDMap); i++) {
			if (!stricmp(g_GameToAppIDMap[i].pGameDir, m_gameDir)) {
				appId = g_GameToAppIDMap[i].iAppID;
				break;
			}
		}
	}

	if (!appId) {
		appId = HLDS_APPID;
	}

	return appId;
}

void CServerInfo::parseAppVersion()
{
	char filename[MAX_PATH];
	_snprintf(filename, sizeof(filename), "./%s/steam.inf", m_gameDir);
	filename[sizeof(filename)-1] = 0;

	FILE* fp = fopen(filename, "rt");
	if (fp) {
		char line[256];

		while (fgets(line, sizeof line, fp)) {
			char* value = strchr(line, '=');
			if (!value) {
				continue;
			}

			*value++ = '\0';
			trimbuf(line);
			trimbuf(value);

			if (!strcmp(line, "PatchVersion=")) {
				_snprintf(m_appVersion, sizeof(m_appVersion)-1, "%s/Stdio", value);
				m_appVersion[sizeof(m_appVersion)-1] = 0;
			}
			else if (!strcmp(line, "ProductName=")) {
				strncpy(m_productName, value, sizeof(m_productName)-1);
				m_productName[sizeof(m_productName)-1] = 0;
			}
		}
		fclose(fp);
	}

	if (!m_appVersion[0]) {
		strncpy(m_appVersion, HLDS_APPVERSION, sizeof(m_appVersion)-1);
		m_appVersion[sizeof(m_appVersion)-1] = 0;
	}
}

void serverinfo_init()
{
	if (!g_ServerInfo) {
		g_ServerInfo = new CServerInfo();
	}
}

void serverinfo_uninit()
{
	if (g_ServerInfo) {
		delete g_ServerInfo;
		g_ServerInfo = nullptr;
	}
}
