#include "stdafx.h"
#include "KingSystem.h"
#include "KnightsManager.h"

#include "../shared/ClientSocketMgr.h"
#include "../shared/Ini.h"
#include "../shared/DateTime.h"
#include "../shared/HardwareInformation.h"

#include <time.h>
#include <iostream>
#include <fstream>
#include "Map.h"

#include "AISocket.h"

#include "DBAgent.h"

using namespace std;

#define NUM_FLAG_VICTORY    4
#define AWARD_GOLD          100000
#define AWARD_EXP			5000

std::vector<Thread *> g_timerThreads;

WORD	g_increase_serial = 1;

HardwareInformation g_HardwareInformation;


CGameServerDlg::CGameServerDlg()
{
	DateTime now;

	m_sYear = now.GetYear();
	m_sMonth = now.GetMonth();
	m_sDate = now.GetDay();
	m_sHour = now.GetHour();
	m_sMin = now.GetMinute();

	m_ReloadKnightAndUserRanksMinute = 0;

	m_byWeather = 0;
	m_sWeatherAmount = 0;
	m_byKingWeatherEvent = 0;
	m_byKingWeatherEvent_Day = 0;
	m_byKingWeatherEvent_Hour = 0;
	m_byKingWeatherEvent_Minute = 0;

	m_byNPEventAmount = 0;
	m_byExpEventAmount = 0;
	m_byCoinEventAmount = 0;

	m_sPartyIndex = 0;

	m_nCastleCapture = 0;

	m_bFirstServerFlag = false;
	m_bPointCheckFlag = true;

	m_nServerNo = 0;
	m_nServerGroupNo = 0;
	m_nServerGroup = 0;
	m_sDiscount = 0;

	m_bPermanentChatMode = false;
	m_bSantaOrAngel = FLYING_NONE;
}

/**
* @brief	Loads config, table data, initialises sockets and generally
* 			starts up the server.
*
* @return	true if it succeeds, false if it fails.
*/
bool CGameServerDlg::Startup()
{
	DateTime time;

	m_sZoneCount = 0;
	m_sErrorSocketCount = 0;

	m_bFirstServerFlag = false;	
	m_bServerCheckFlag = false;

	GetTimeFromIni();

	// License System Start
	/*m_HardwareIDArray.push_back(4467444930295366338); // Heros
	m_HardwareIDArray.push_back(4344044418295366338);
	m_HardwareIDArray.push_back(236302953922330); // 5.175.225.30

	if (!g_HardwareInformation.IsValidHardwareID(m_HardwareIDArray))
	return false;
	// License System End*/

	//bool IsValidLicense = false;
	//std::string LicensedIPAddresses = "127.0.0.1";
	//std::string ServerIPAddress = "0.0.0.0";
	//
	//foreach_stlmap_nolock (itr,m_ServerArray)
	//{
	//	if (LicensedIPAddresses.find(itr->second->strServerIP) == std::string::npos)
	//		continue;
	//
	//	ServerIPAddress = itr->second->strServerIP;
	//	IsValidLicense = true;
	//	break;
	//}
	//
	//if (!IsValidLicense)
	//{
	//	printf("ERROR:IP Address is not have a license.\n");
	//	return false;
	//}
	// License System End

	if (!g_pMain->m_socketMgr.Listen(m_GameServerPort, MAX_USER))
	{
		printf(_T("ERROR : Failed to listen on server port (%d).\n"), m_GameServerPort);
		return false;
	}

	// Bit tacky, but there's no reason we can't reuse the existing completion port for our AI socket
	m_aiSocketMgr.SetCompletionPort(g_pMain->m_socketMgr.GetCompletionPort());
	m_aiSocketMgr.InitSessions(1);

	if (!g_DBAgent.Startup(m_bMarsEnabled,
		m_strAccountDSN, m_strAccountUID, m_strAccountPWD,
		m_strGameDSN, m_strGameUID, m_strGamePWD)
		|| !LoadItemTable()
		|| !LoadSetItemTable()
		|| !LoadItemExchangeTable()
		|| !LoadItemUpgradeTable()
		|| !LoadItemOpTable()
		|| !LoadExpiration()
		|| !LoadServerResourceTable()
		|| !LoadQuestHelperTable()
		|| !LoadQuestMonsterTable()
		|| !LoadMagicTable()
		|| !LoadMagicType1()
		|| !LoadMagicType2()
		|| !LoadMagicType3()
		|| !LoadMagicType4()
		|| !LoadMagicType5()
		|| !LoadMagicType6()
		|| !LoadMagicType7()
		|| !LoadMagicType8()
		|| !LoadMagicType9()
		|| !LoadObjectPosTable()
		|| !LoadRentalList()
		|| !LoadCoefficientTable()
		|| !LoadLevelUpTable()
		|| !LoadAllKnights()
		|| !LoadAllKnightsUserData()
		|| !LoadKnightsAllianceTable()
		|| !LoadKnightsSiegeWarsTable()
		|| !LoadUserRankings()
		|| !LoadKnightsCapeTable()
		|| !LoadKnightsRankTable()
		|| !LoadStartPositionTable()
		|| !LoadStartPositionRandomTable()
		|| !LoadBattleTable()
		|| !MapFileLoad()
		|| !LoadKingSystem()
		|| !LoadEventTriggerTable()
		|| !LoadMonsterChallengeTable()
		|| !LoadMonsterChallengeSummonListTable()
		|| !LoadMonsterSummonListTable()
		|| !LoadMonsterSummonListZoneTable()
		|| !LoadMonsterRespawnListTable()
		|| !LoadMonsterRespawnListInformationTable()
		|| !LoadPremiumItemTable()
		|| !LoadPremiumItemExpTable()
		|| !LoadUserDailyOpTable()
		|| !LoadUserItemTable())
		return false;

	// Clear any remaining users in the currently logged in list
	// that may be left as a result of an improper shutdown.
	g_DBAgent.ClearRemainUsers();

	// Logs Start
	CreateDirectory("Logs",NULL);

	m_fpDeathUser = fopen(string_format("./Logs/DeathUser_%d_%d_%d.log",time.GetDay(),time.GetMonth(),time.GetYear()).c_str(), "a");
	if (m_fpDeathUser == nullptr)
	{
		printf("ERROR: Unable to open death user log file.\n");
		return false;
	}

	m_fpMerchant = fopen(string_format("./Logs/Merchant_%d_%d_%d.log",time.GetDay(),time.GetMonth(),time.GetYear()).c_str(), "a");
	if (m_fpMerchant == nullptr)
	{
		printf("ERROR: Unable to open merchant log file.\n");
		return false;
	}

	m_fpTrade = fopen(string_format("./Logs/Trade_%d_%d_%d.log",time.GetDay(),time.GetMonth(),time.GetYear()).c_str(), "a");
	if (m_fpTrade == nullptr)
	{
		printf("ERROR: Unable to open trade log file.\n");
		return false;
	}

	m_fpDeathNpc = fopen(string_format("./Logs/DeathNpc_%d_%d_%d.log",time.GetDay(),time.GetMonth(),time.GetYear()).c_str(), "a");
	if (m_fpDeathNpc == nullptr)
	{
		printf("ERROR: Unable to open death npc log file.\n");
		return false;
	}

	m_fpChat = fopen(string_format("./Logs/Chat_%d_%d_%d.log",time.GetDay(),time.GetMonth(),time.GetYear()).c_str(), "a");
	if (m_fpChat == nullptr)
	{
		printf("ERROR: Unable to open chat log file.\n");
		return false;
	}

	m_fpCheat = fopen(string_format("./Logs/Cheat_%d_%d_%d.log",time.GetDay(),time.GetMonth(),time.GetYear()).c_str(), "a");
	if (m_fpCheat == nullptr)
	{
		printf("ERROR: Unable to open cheat log file.\n");
		return false;
	}
	// Logs End

	LoadNoticeData();

	printf("\n");
	if (!m_luaEngine.Initialise())
		return false;

	AIServerConnect();

	// Initialise the command tables
	InitServerCommands();
	CUser::InitChatCommands();


	g_pMain->m_socketMgr.RunServer();

	return true; 
}

uint16 CGameServerDlg::CalcEventRemainingTime()
{
	DateTime now(&g_localTime);
	uint16 Saat, Dakika, SaatFarki, DakikaFarki, xSaatFarki, BaslSaati;
	bool BugunVarMi, isFirst;
	xSaatFarki = -1;
	BaslSaati = -1;
	SaatFarki = -1;
	DakikaFarki = -1;
	isFirst = true;
	BugunVarMi = false;
	Saat = now.GetHour();
	Dakika = now.GetMinute();

	for (int i = 0; i < BIFROST_EVENT_COUNT; i++){
		if (m_nBifrostTime[i] > 23)
			continue;
		if (m_nBifrostTime[i] > Saat)
			BugunVarMi = true;

	}
	// Gün bitmeden olup olmadığı bilgisini çektik
	for (int i = 0; i < BIFROST_EVENT_COUNT; i++){
		if (m_nBifrostTime[i] < 24 )
			xSaatFarki = m_nBifrostTime[i] - Saat;
		else
			continue;

		if(!BugunVarMi)
			xSaatFarki += 24;

		if (xSaatFarki > 0 && BaslSaati == -1) {
			SaatFarki = xSaatFarki - 1;
			DakikaFarki = (60-Dakika);
			BaslSaati = m_nBifrostTime[i];
		}
		if (xSaatFarki < (SaatFarki+1) && BaslSaati > -1){
			SaatFarki = xSaatFarki - 1;
			DakikaFarki = (60-Dakika);
			BaslSaati = m_nBifrostTime[i];
		}



	}

	if (DakikaFarki == -1 && SaatFarki == -1)
		SaatFarki=12;

	if (SaatFarki > 15)
		SaatFarki=15;

	if (DakikaFarki > 59)
		DakikaFarki=0;

	return ((DakikaFarki+(SaatFarki * 60)) * MINUTE);
}

/**
* @brief	Loads the server's config from the INI file.
*/
void CGameServerDlg::GetTimeFromIni()
{
	CIni ini(CONF_GAME_SERVER);
	int year=0, month=0, date=0, hour=0, server_count=0, sgroup_count = 0, i=0;
	char ipkey[20];

	// This is so horrible.
	ini.GetString("ODBC", "GAME_DSN", "KO_GAME", m_strGameDSN, false);
	ini.GetString("ODBC", "GAME_UID", "username", m_strGameUID, false);
	ini.GetString("ODBC", "GAME_PWD", "password", m_strGamePWD, false);

	m_bMarsEnabled = ini.GetBool("ODBC", "GAME_MARS", true);

	ini.GetString("ODBC", "ACCOUNT_DSN", "KO_MAIN", m_strAccountDSN, false);
	ini.GetString("ODBC", "ACCOUNT_UID", "username", m_strAccountUID, false);
	ini.GetString("ODBC", "ACCOUNT_PWD", "password", m_strAccountPWD, false);

	bool bMarsEnabled = ini.GetBool("ODBC", "ACCOUNT_MARS", true);

	// Both need to be enabled to use MARS.
	if (!m_bMarsEnabled || !bMarsEnabled)
		m_bMarsEnabled = false;

	m_byWeather = ini.GetInt("TIMER", "WEATHER", 1);
	KCaktifmi = ini.GetBool("GAME", "NPtoKCSystem", 0);
	onbesinde = ini.GetBool("GAME", "ResetLoyaltyMonthlyAt15", 0);
	KCmiktari = ini.GetInt("GAME", "KCto1000NP", 0);
	m_bIsPKServer = ini.GetInt("GAME", "ISPK", 1);
	MAXLVLINI = ini.GetInt("GAME", "MAX_LEVEL", 80);
	ini.GetString("BATTLE", "DAYS","1,6", m_sBattleZoneOpenDays, false);
	CZBowlAktifMi = ini.GetBool("GAME", "CZ_BOWL", false);
	CzBowlItem = ini.GetInt("GAME", "CZ_BOWL_REWARD_ITEM", 0);
	CzBowlItemCount = ini.GetInt("GAME", "CZ_BOWL_REWARD_ITEM_COUNT", 0);
	CzBowlItemTimeSec = ini.GetInt("GAME", "CZ_BOWL_REWARD_ITEM_SECOND", 0);
	CzBowlKC = ini.GetInt("GAME", "CZ_BOWL_REWARD_KC", 0);
	CzBowlKCTimeSec = ini.GetInt("GAME", "CZ_BOWL_REWARD_KC_SECOND", 0);
	CzBowlNP = ini.GetInt("GAME", "CZ_BOWL_REWARD_NP", 0);
	CzBowlNpTimeSec = ini.GetInt("GAME", "CZ_BOWL_REWARD_NP_SECOND", 0);
	CzBowlReloadTimeSec = ini.GetInt("GAME", "CZ_BOWL_RELOAD", 0);

	for (int i = 0; i < WAR_TIME_COUNT; i++)
		m_nBattleZoneOpenHourStart[i] = ini.GetInt("BATTLE",string_format("START_TIME%d",i).c_str(), (i+1) * 7);

	for (int i = 0; i < WAR_ZONE_COUNT; i++)
	{
		m_nBattlezoneOpenWarZone[i] = ini.GetInt("BATTLE",string_format("WAR_ZONE%d",i).c_str(), ZONE_BATTLE + i);
		if (m_nBattlezoneOpenWarZone[i] > ZONE_BATTLE_BASE)
			m_nBattlezoneOpenWarZone[i] = m_nBattlezoneOpenWarZone[i] - ZONE_BATTLE_BASE;
	}
	///war snow visualdev

	ini.GetString("SNOW-WAR", "DAYS","1,2,4", m_sBattleZoneSnowOpenDays, false);

	for (int i = 0; i < SNOW_TIME_COUNT; i++)
		m_nBattleZoneOpenHourSnowStart[i] = ini.GetInt("SNOW-WAR",string_format("HORA_INICIO%d",i).c_str(), (i+1) * 7);

	for (int i = 0; i < SNOW_ZONE_COUNT; i++)
	{
		m_nBattlezoneOpenSnowWarZone[i] = ini.GetInt("SNOW-WAR",string_format("MAPA_SNOW%d",i).c_str(), ZONE_SNOW_BATTLE );

	}
	/// end


	m_nCastleCapture = ini.GetInt("CASTLE", "NATION", 1);
	m_nServerNo = ini.GetInt("ZONE_INFO", "MY_INFO", 1);
	m_nServerGroup = ini.GetInt("ZONE_INFO", "SERVER_NUM", 0);
	server_count = ini.GetInt("ZONE_INFO", "SERVER_COUNT", 1);
	if (server_count < 1)
	{
		printf("ERROR: Invalid SERVER_COUNT property in INI.\n");
		return;
	}

	for (i = 0; i < server_count; i++)
	{
		_ZONE_SERVERINFO *pInfo = new _ZONE_SERVERINFO;
		sprintf(ipkey, "SERVER_%02d", i);
		pInfo->sServerNo = ini.GetInt("ZONE_INFO", ipkey, 1);
		sprintf(ipkey, "SERVER_IP_%02d", i);
		ini.GetString("ZONE_INFO", ipkey, "127.0.0.1", pInfo->strServerIP);
		m_ServerArray.PutData(pInfo->sServerNo, pInfo);
	}

	if (m_nServerGroup != 0)
	{
		m_nServerGroupNo = ini.GetInt("SG_INFO", "GMY_INFO", 1);
		sgroup_count = ini.GetInt("SG_INFO", "GSERVER_COUNT", 1);
		if (server_count < 1)
		{
			printf("ERROR: Invalid GSERVER_COUNT property in INI.\n");
			return;
		}

		for (i = 0; i < sgroup_count; i++)
		{
			_ZONE_SERVERINFO *pInfo = new _ZONE_SERVERINFO;
			sprintf(ipkey, "GSERVER_%02d",i );
			pInfo->sServerNo = ini.GetInt("SG_INFO", ipkey, 1);
			sprintf(ipkey, "GSERVER_IP_%02d", i);
			ini.GetString("SG_INFO", ipkey, "127.0.0.1", pInfo->strServerIP);

			m_ServerGroupArray.PutData(pInfo->sServerNo, pInfo);
		}
	}

	ini.GetString("AI_SERVER", "IP", "127.0.0.1", m_AIServerIP);
	m_AIServerPort = ini.GetInt("AI_SERVER","PORT", 10020);

	m_GameServerPort = ini.GetInt("SETTINGS","PORT", 15001);
	m_nGameMasterRHitDamage = ini.GetInt("SETTINGS","GAME_MASTER_R_HIT_DAMAGE", 30000);
	m_nPlayerRankingResetTime = ini.GetInt("SETTINGS","PLAYER_RANKINGS_RESET_TIME", 12);
	ini.GetString("SETTINGS", "PLAYER_RANKINGS_REWARD_ZONES","71,72,73", m_sPlayerRankingsRewardZones, false);
	m_nPlayerRankingKnightCashReward = ini.GetInt("SETTINGS","PLAYER_RANKINGS_KNIGHT_CASH_REWARD", 0);
	m_nPlayerRankingLoyaltyReward = ini.GetInt("SETTINGS","PLAYER_RANKINGS_LOYALTY_REWARD", 0);
	m_byExpEventAmount = ini.GetInt("BONUS","EXP",0);
	Reskillmoney = ini.GetInt("BONUS", "RESKILL_NOMONEY_ACTIVE", 1);
	Restatmoney = ini.GetInt("BONUS", "RESTAT_NOMONEY_ACTIVE", 1);
	ini.GetString("SERVER_NOTICE", "SERVER_NAME", "", m_bServerName, false);
	m_byPKZoneExpEventAmount = ini.GetInt("SETTINGS","PK_EXP",0);
	// gold 2, Gold 10 , Gold 50, Gold 100, Gold 500, Gold 1000, Gold TOTAL

	m_byGoldx2 = ini.GetInt("JACKPOINTS","GOLD_2",1100); // 2500 
	m_byGoldx10 = ini.GetInt("JACKPOINTS","GOLD_10",200); // 1000
	m_byGoldx10 += m_byGoldx2;
	m_byGoldx50 = ini.GetInt("JACKPOINTS","GOLD_50",50); // 100
	m_byGoldx50 += m_byGoldx10;
	m_byGoldx100 = ini.GetInt("JACKPOINTS","GOLD_100",25);
	m_byGoldx100 += m_byGoldx50;
	m_byGoldx500 = ini.GetInt("JACKPOINTS","GOLD_500",10);
	m_byGoldx500 += m_byGoldx100;
	m_byGoldx1000 = ini.GetInt("JACKPOINTS","GOLD_1000",5);
	m_byGoldx1000 += m_byGoldx500;
	m_byGoldTotal = m_byGoldx1000;
	printf("Jackpot[GOLD] rates:\n");
	printf("x2 Jackpot [%d],[%d]\n",m_byGoldx2,ini.GetInt("JACKPOINTS","GOLD_2",0));
	printf("x10 Jackpot [%d],[%d]\n",m_byGoldx10,ini.GetInt("JACKPOINTS","GOLD_10",0));
	printf("x50 Jackpot [%d],[%d]\n",m_byGoldx50,ini.GetInt("JACKPOINTS","GOLD_50",0));
	printf("x100 Jackpot [%d],[%d]\n",m_byGoldx100,ini.GetInt("JACKPOINTS","GOLD_100",0));
	printf("x500 Jackpot [%d],[%d]\n",m_byGoldx500,ini.GetInt("JACKPOINTS","GOLD_500",0));
	printf("x1000 Jackpot [%d],[%d]\n",m_byGoldx1000,ini.GetInt("JACKPOINTS","GOLD_1000",0));

	// EXP 2, EXP 10 , EXP 50, EXP 100, EXP 500, EXP 1000, EXP TOTAL
	m_byExpx2 = ini.GetInt("JACKPOINTS","EXP_2",1100);
	m_byExpx10 = ini.GetInt("JACKPOINTS","EXP_10",200);
	m_byExpx10 += m_byExpx2;
	m_byExpx50 = ini.GetInt("JACKPOINTS","EXP_50",50);
	m_byExpx50 += m_byExpx10;
	m_byExpx100 = ini.GetInt("JACKPOINTS","EXP_100",25);
	m_byExpx100 += m_byExpx50;
	m_byExpx500 = ini.GetInt("JACKPOINTS","EXP_500",10);
	m_byExpx500 += m_byExpx100;
	m_byExpx1000 = ini.GetInt("JACKPOINTS","EXP_1000",5);
	m_byExpx1000 += m_byExpx500;
	m_byExpTotal = m_byExpx1000;

	printf("Jackpot[EXP] rates:\n");
	printf("x2 Jackpot [%d],[%d]\n",m_byExpx2,ini.GetInt("JACKPOINTS","EXP_2",0));
	printf("x10 Jackpot [%d],[%d]\n",m_byExpx10,ini.GetInt("JACKPOINTS","EXP_10",0));
	printf("x50 Jackpot [%d],[%d]\n",m_byExpx50,ini.GetInt("JACKPOINTS","EXP_50",0));
	printf("x100 Jackpot [%d],[%d]\n",m_byExpx100,ini.GetInt("JACKPOINTS","EXP_100",0));
	printf("x500 Jackpot [%d],[%d]\n",m_byExpx500,ini.GetInt("JACKPOINTS","EXP_500",0));
	printf("x1000 Jackpot [%d],[%d]\n",m_byExpx1000,ini.GetInt("JACKPOINTS","EXP_1000",0));



	m_byCoinEventAmount = ini.GetInt("BONUS","MONEY",0);
	m_byNPEventAmount = ini.GetInt("BONUS","NP",0);
	m_Loyalty_Clan_G4 = 	ini.GetInt("SETTINGS","KNIGHTS_NP_GRADE4",77000);
	m_Loyalty_Clan_G3 = 	ini.GetInt("SETTINGS","KNIGHTS_NP_GRADE3",144000);
	m_Loyalty_Clan_G2 = 	ini.GetInt("SETTINGS","KNIGHTS_NP_GRADE2",360000);
	m_Loyalty_Clan_G1 = 	ini.GetInt("SETTINGS","KNIGHTS_NP_GRADE1",720000);
	m_Loyalty_Ardream_Source = 	ini.GetInt("NATIONAL_POINTS","ARDREAM_SOURCE",32);
	m_Loyalty_Ardream_Target = ini.GetInt("NATIONAL_POINTS","ARDREAM_TARGET",-25);
	m_Loyalty_Ronark_Land_Base_Source = ini.GetInt("NATIONAL_POINTS","RONARK_LAND_BASE_SOURCE",64);
	m_Loyalty_Ronark_Land_Base_Target = ini.GetInt("NATIONAL_POINTS","RONARK_LAND_BASE_TARGET",-50);
	m_Loyalty_Ronark_Land_Source = ini.GetInt("NATIONAL_POINTS","RONARK_LAND_SOURCE",64);
	m_Loyalty_Ronark_Land_Target = ini.GetInt("NATIONAL_POINTS","RONARK_LAND_TARGET",-50);
	m_Loyalty_Other_Zone_Source = ini.GetInt("NATIONAL_POINTS","OTHER_ZONE_SOURCE",64);
	m_Loyalty_Other_Zone_Target = ini.GetInt("NATIONAL_POINTS","OTHER_ZONE_TARGET",-50);

	ini.GetString("CHAT_COMMANDS", "TEST", "test", Cmd1, false);
	Command1 = new char[Cmd1.length() + 1];
	std::strcpy(Command1, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "give_item", "give_item", Cmd1, false);
	Command2 = new char[Cmd1.length() + 1];
	std::strcpy(Command2, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "zonechange", "zonechange", Cmd1, false);
	Command3 = new char[Cmd1.length() + 1];
	std::strcpy(Command3, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "zonecount", "zonecount", Cmd1, false);
	Command4 = new char[Cmd1.length() + 1];
	std::strcpy(Command4, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "monsummon", "monsummon", Cmd1, false);
	Command5 = new char[Cmd1.length() + 1];
	std::strcpy(Command5, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "npcsummon", "npcsummon", Cmd1, false);
	Command6 = new char[Cmd1.length() + 1];
	std::strcpy(Command6, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "monkill", "monkill", Cmd1, false);
	Command7 = new char[Cmd1.length() + 1];
	std::strcpy(Command7, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "open1", "open1", Cmd1, false);
	Command8 = new char[Cmd1.length() + 1];
	std::strcpy(Command8, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "open2", "open2", Cmd1, false);
	Command9 = new char[Cmd1.length() + 1];
	std::strcpy(Command9, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "open3", "open3", Cmd1, false);
	Command10 = new char[Cmd1.length() + 1];
	std::strcpy(Command10, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "open4", "open4", Cmd1, false);
	Command11 = new char[Cmd1.length() + 1];
	std::strcpy(Command11, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "open5", "open5", Cmd1, false);
	Command12 = new char[Cmd1.length() + 1];
	std::strcpy(Command12, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "open6", "open6", Cmd1, false);
	Command13 = new char[Cmd1.length() + 1];
	std::strcpy(Command13, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "captain", "captain", Cmd1, false);
	Command14 = new char[Cmd1.length() + 1];
	std::strcpy(Command14, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "snowopen", "snowopen", Cmd1, false);
	Command15 = new char[Cmd1.length() + 1];
	std::strcpy(Command15, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "siegewarfare", "siegewarfare", Cmd1, false);
	Command16 = new char[Cmd1.length() + 1];
	std::strcpy(Command16, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "close", "close", Cmd1, false);
	Command17 = new char[Cmd1.length() + 1];
	std::strcpy(Command17, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "np_change", "np_change", Cmd1, false);
	Command18 = new char[Cmd1.length() + 1];
	std::strcpy(Command18, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "exp_change", "exp_change", Cmd1, false);
	Command19 = new char[Cmd1.length() + 1];
	std::strcpy(Command19, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "gold_change", "gold_change", Cmd1, false);
	Command20 = new char[Cmd1.length() + 1];
	std::strcpy(Command20, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "np_add", "np_add", Cmd1, false);
	Command21 = new char[Cmd1.length() + 1];
	std::strcpy(Command21, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "exp_add", "exp_add", Cmd1, false);
	Command22 = new char[Cmd1.length() + 1];
	std::strcpy(Command22, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "money_add", "money_add", Cmd1, false);
	Command23 = new char[Cmd1.length() + 1];
	std::strcpy(Command23, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "unban_char", "unban_char", Cmd1, false);
	Command24 = new char[Cmd1.length() + 1];
	std::strcpy(Command24, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "tp_all", "tp_all", Cmd1, false);
	Command25 = new char[Cmd1.length() + 1];
	std::strcpy(Command25, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "summonknights", "summonknights", Cmd1, false);
	Command26 = new char[Cmd1.length() + 1];
	std::strcpy(Command26, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "warresult", "warresult", Cmd1, false);
	Command27 = new char[Cmd1.length() + 1];
	std::strcpy(Command27, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "resetranking", "resetranking", Cmd1, false);
	Command28 = new char[Cmd1.length() + 1];
	std::strcpy(Command28, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "noticepm", "noticepm", Cmd1, false);
	Command29 = new char[Cmd1.length() + 1];
	std::strcpy(Command29, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "noticeclan", "noticeclan", Cmd1, false);
	Command30 = new char[Cmd1.length() + 1];
	std::strcpy(Command30, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "noticeally", "noticeally", Cmd1, false);
	Command31 = new char[Cmd1.length() + 1];
	std::strcpy(Command31, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "noticecaptain", "noticecaptain", Cmd1, false);
	Command32 = new char[Cmd1.length() + 1];
	std::strcpy(Command32, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "noticeparty", "noticeparty", Cmd1, false);
	Command33 = new char[Cmd1.length() + 1];
	std::strcpy(Command33, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "bdw", "bdw", Cmd1, false);
	Command34 = new char[Cmd1.length() + 1];
	std::strcpy(Command34, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "nptokc", "nptokc", Cmd1, false);
	Command35 = new char[Cmd1.length() + 1];
	std::strcpy(Command35, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "ban_char", "ban_char", Cmd1, false);
	Command36 = new char[Cmd1.length() + 1];
	std::strcpy(Command36, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "ban_account", "ban_account", Cmd1, false);
	Command37 = new char[Cmd1.length() + 1];
	std::strcpy(Command37, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "unban_client", "unban_client", Cmd1, false);
	Command38 = new char[Cmd1.length() + 1];
	std::strcpy(Command38, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "access_gm", "access_gm", Cmd1, false);
	Command39 = new char[Cmd1.length() + 1];
	std::strcpy(Command39, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "remove_gm", "remove_gm", Cmd1, false);
	Command40 = new char[Cmd1.length() + 1];
	std::strcpy(Command40, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "ipban_char", "ipban_char", Cmd1, false);
	Command41 = new char[Cmd1.length() + 1];
	std::strcpy(Command41, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "clan_item", "clan_item", Cmd1, false);
	Command42 = new char[Cmd1.length() + 1];
	std::strcpy(Command42, Cmd1.c_str());

	ini.GetString("CHAT_COMMANDS", "item_online", "item_online", Cmd1, false);
	Command43 = new char[Cmd1.length() + 1];
	std::strcpy(Command43, Cmd1.c_str());

	for (int i = 0; i < BIFROST_EVENT_COUNT; i++){
		m_nBifrostTime[i] = ini.GetInt("BIFROST",string_format("START_TIME%d",i+1).c_str(), 0);	

		printf ("[BIFROST#%d] Event start hour: %d\n",(i+1),m_nBifrostTime[i]);
	}
	// Bf kalan zaman editlencek

	for (int i = 0; i < BORDER_DEFENSE_WAR_EVENT_COUNT; i++)
	{
		m_nBorderDefenseWarTime[i] = ini.GetInt("BDW",string_format("START_TIME%d",i+1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonItemNo1[i] = ini.GetInt("BDW",string_format("ITEM_WON_PRIZE1_%d",i+1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonItemNo2[i] = ini.GetInt("BDW",string_format("ITEM_WON_PRIZE2_%d",i+1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonItemNo3[i] = ini.GetInt("BDW",string_format("ITEM_WON_PRIZE3_%d",i+1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonItemNo4_K[i] = ini.GetInt("BDW",string_format("ITEM_WON_PRIZE4KARUS_%d",i+1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonItemNo4_H[i] = ini.GetInt("BDW",string_format("ITEM_WON_PRIZE4HUMAN_%d",i+1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonLoyalty[i] = ini.GetInt("BDW",string_format("ITEM_WON_PRIZE_LOYALTY_%d",i+1).c_str(), 0);
		m_nBorderDefenseWarPrizeWonExp[i] = ini.GetInt("BDW",string_format("ITEM_WON_PRIZE_EXP_%d",i+1).c_str(), 0);
		m_nBorderDefenseWarPrizeLoserLoyalty[i] = ini.GetInt("BDW",string_format("ITEM_LOSE_PRIZE_LOYALTY_%d",i+1).c_str(), 0);
		m_nBorderDefenseWarPrizeLoserItem[i] = ini.GetInt("BDW",string_format("ITEM_LOSE_PRIZE_ITEM_%d",i+1).c_str(), 0);
		m_nBorderDefenseWarPrizeLoserExp[i] = ini.GetInt("BDW",string_format("ITEM_LOSE_PRIZE_EXP_%d",i+1).c_str(), 0);	
		m_nBorderDefenseWarMAXLEVEL[i] = ini.GetInt("BDW",string_format("MAX_LEVEL_%d",i+1).c_str(), 80);
		m_nBorderDefenseWarMINLEVEL[i] = ini.GetInt("BDW",string_format("MIN_LEVEL_%d",i+1).c_str(), 1);
		printf ("[BDW#%d] Event start hour: %d\n",(i+1),m_nBorderDefenseWarTime[i]);

	}
	for (int i = 0; i < CHAOS_EVENT_COUNT; i++)
		m_nChaosTime[i] = ini.GetInt("CHAOS",string_format("START_TIME%d",i+1).c_str(), 0);

	m_nTempleEventRemainSeconds = 0;
	m_sRankResetHour = 0;

	m_UserRankingArray[KARUS_ARRAY].DeleteAllData();
	m_UserRankingArray[ELMORAD_ARRAY].DeleteAllData();

	m_byBattleOpenedTime = 0;
	m_byBattleTime = (2 * 60) * 60; // 2 Hours

	m_xBifrostRemainingTime = CalcEventRemainingTime();  // Bifrost remaining time ( 2 hour ).
	printf("Bifrost remaining time: %d\n",CalcEventRemainingTime());
	m_xBifrostMonumentAttackTime = (30 * MINUTE); // Players is attack a monument last 30 minute.
	m_xBifrostTime = (120 * MINUTE); // Victory nation time ( 2 hour )
	m_xJoinOtherNationBifrostTime = (60 * MINUTE); // Other nation join time ( last 1 hour )

	m_bAttackBifrostMonument = false;
	m_BifrostVictory = 0;
	m_sBifrostRemainingTime = m_xBifrostRemainingTime + 60;
	m_sBifrostTime = 0;
	m_sBifrostWarStart = false;
	m_sBifrostVictoryAll = 3;
	m_sBifrostVictoryNoticeAll = false;

	m_IsMagicTableInUpdateProcess = false;
	m_IsPlayerRankingUpdateProcess = false;

	m_nPVPMonumentNation[ZONE_ARDREAM] = 0; 
	m_nPVPMonumentNation[ZONE_RONARK_LAND_BASE] = 0;
	m_nPVPMonumentNation[ZONE_RONARK_LAND] = 0;

	pTempleEvent.ActiveEvent = -1;
	pTempleEvent.ZoneID = 0;
	pTempleEvent.LastEventRoom = 1;
	pTempleEvent.StartTime = 0;
	pTempleEvent.AllUserCount = 0;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;
	pTempleEvent.KarusKillCount = 0;
	pTempleEvent.ElMoradKillCount = 0;
	pTempleEvent.m_sKarusBdw = 0;
	pTempleEvent.m_sElmoBdw = 0;
	for (int b = 0; b < MAX_TEMPLE_EVENT_ROOM; b++)
		pTempleEvent.isAttackable[b+1] = false;
	pTempleEvent.isActive = false;
	memset(pTempleEvent.KarusDeathCount , 0 , sizeof(pTempleEvent.KarusDeathCount));
	memset(pTempleEvent.ElmoDeathCount , 0 , sizeof(pTempleEvent.ElmoDeathCount));
	memset(pTempleEvent.OurRoomTeleportTime , 0 , sizeof(pTempleEvent.OurRoomTeleportTime));
	for (int ab = 0; ab < MAX_TEMPLE_EVENT_ROOM; ab++){
		pTempleEvent.OurRoomIsTeleported[ab+1] = false;
		pTempleEvent.waiting4TP[ab+1] = false;
	}
	m_bForgettenTempleIsActive = false;
	m_nForgettenTempleStartHour = 0;
	m_nForgettenTempleLevelMin = 0;
	m_nForgettenTempleLevelMax = 0;
	m_nForgettenTempleStartTime = 0;
	m_nForgettenTempleChallengeTime = 0;
	m_bForgettenTempleSummonMonsters = false;
	m_nForgettenTempleCurrentStage = 0;
	m_nForgettenTempleLastStage = 0;
	m_nForgettenTempleLastSummonTime = 0;
	m_nForgettenTempleBanishFlag = false;
	m_nForgettenTempleBanishTime = 0;

	g_timerThreads.push_back(new Thread(Timer_CheckGameEvents));
	g_timerThreads.push_back(new Thread(Timer_BifrostTime));
	g_timerThreads.push_back(new Thread(Timer_UpdateGameTime)); 
	g_timerThreads.push_back(new Thread(Timer_UpdateSessions));
	g_timerThreads.push_back(new Thread(Timer_UpdateConcurrent));
}

/**
* @brief	Gets & formats a cached server resource (_SERVER_RESOURCE entry).
*
* @param	nResourceID	Identifier for the resource.
* @param	result	   	The string to store the formatted result in.
*/
void CGameServerDlg::GetServerResource(int nResourceID, string * result, ...)
{
	_SERVER_RESOURCE *pResource = m_ServerResourceArray.GetData(nResourceID);
	if (pResource == nullptr)
	{
		*result = nResourceID;
		return;
	}

	va_list args;
	va_start(args, result);
	_string_format(pResource->strResource, result, args);
	va_end(args);
}

/**
* @brief	Gets the starting positions (for both nations) 
* 			for the specified zone.
*
* @param	nZoneID	Identifier for the zone.
*/
_START_POSITION *CGameServerDlg::GetStartPosition(int nZoneID)
{
	return m_StartPositionArray.GetData(nZoneID);
}

/**
* @brief	Gets the experience points required for the 
* 			specified level.
*
* @param	nLevel	The level.
*
* @return	The experience points required to level up from 
* 			the specified level.
*/
int64 CGameServerDlg::GetExpByLevel(int nLevel)
{
	LevelUpArray::iterator itr = m_LevelUpArray.find(nLevel);
	if (itr != m_LevelUpArray.end())
		return itr->second;

	return 0;
}

/**
* @brief	Gets zone by its identifier.
*
* @param	zoneID	Identifier for the zone.
*
* @return	null if it fails, otherwise the zone.
*/
C3DMap * CGameServerDlg::GetZoneByID(int zoneID)
{
	return m_ZoneArray.GetData(zoneID);
}

/**
* @brief	Looks up a user by name.
*
* @param	findName	The name to find.
* @param	type		The type of name (account, character).
*
* @return	null if it fails, else the user pointer.
*/
CUser* CGameServerDlg::GetUserPtr(string findName, NameType type)
{
	// As findName is a copy of the string passed in, we can change it
	// without worry of affecting anything.
	STRTOUPPER(findName);

	NameMap::iterator itr;
	if (type == TYPE_ACCOUNT)
	{
		Guard lock(m_accountNameLock);
		itr = m_accountNameMap.find(findName);
		return (itr != m_accountNameMap.end() ? itr->second : nullptr);
	}
	else if (type == TYPE_CHARACTER)
	{
		Guard lock(m_characterNameLock);
		itr = m_characterNameMap.find(findName);
		return (itr != m_characterNameMap.end() ? itr->second : nullptr);
	}

	return nullptr;
}

/**
* @brief	Adds the account name & session to a hashmap (on login)
*
* @param	pSession	The session.
*/
void CGameServerDlg::AddAccountName(CUser *pSession)
{
	Guard lock(m_accountNameLock);
	string upperName = pSession->m_strAccountID;
	STRTOUPPER(upperName);
	m_accountNameMap[upperName] = pSession;
}

/**
* @brief	Adds the character name & session to a hashmap (when in-game)
*
* @param	pSession	The session.
*/
void CGameServerDlg::AddCharacterName(CUser *pSession)
{
	Guard lock(m_characterNameLock);
	string upperName = pSession->GetName();
	STRTOUPPER(upperName);
	m_characterNameMap[upperName] = pSession;
}

/**
* @brief	Removes an existing character name/session from the hashmap, 
* 			replaces the character's name and reinserts the session with 
* 			the new name into the hashmap.
*
* @param	pSession		The session.
* @param	strNewUserID	Character's new name.
*/
void CGameServerDlg::ReplaceCharacterName(CUser *pSession, std::string & strNewUserID)
{
	Guard lock(m_characterNameLock);

	// Remove the old name from the map
	string upperName = pSession->GetName();
	STRTOUPPER(upperName);
	m_characterNameMap.erase(upperName);

	// Update the character's name & re-add them to the map.
	pSession->m_strUserID = strNewUserID;
	AddCharacterName(pSession);
}

/**
* @brief	Removes the account name & character names from the hashmaps (on logout)
*
* @param	pSession	The session.
*/
void CGameServerDlg::RemoveSessionNames(CUser *pSession)
{
	string upperName = pSession->m_strAccountID;
	STRTOUPPER(upperName);

	{ // remove account name from map (limit scope)
		Guard lock(m_accountNameLock);
		m_accountNameMap.erase(upperName);
	}


	if (!pSession->GetName().empty())
	{
		NameMap::iterator itr;
		upperName = pSession->GetName();
		STRTOUPPER(upperName);
		Guard lock(m_characterNameLock);
		itr = m_characterNameMap.find(upperName);
		if (itr != m_characterNameMap.end())
			m_characterNameMap.erase(itr->first);
	}

}

CUser				* CGameServerDlg::GetUserPtr(uint16 sUserId) { return m_socketMgr[sUserId]; }
CNpc				* CGameServerDlg::GetNpcPtr(uint16 sNpcId) { return m_arNpcArray.GetData(sNpcId); }
_PARTY_GROUP		* CGameServerDlg::GetPartyPtr(uint16 sPartyID) { return m_PartyArray.GetData(sPartyID); }
CKnights			* CGameServerDlg::GetClanPtr(uint16 sClanID) { return m_KnightsArray.GetData(sClanID); }
_KNIGHTS_ALLIANCE	* CGameServerDlg::GetAlliancePtr(uint16 sAllianceID) { return m_KnightsAllianceArray.GetData(sAllianceID); }
_ITEM_TABLE			* CGameServerDlg::GetItemPtr(uint32 nItemID) { return m_ItemtableArray.GetData(nItemID); }
_KNIGHTS_SIEGE_WARFARE	* CGameServerDlg::GetSiegeMasterKnightsPtr(uint16 sCastleIndex) { return m_KnightsSiegeWarfareArray.GetData(sCastleIndex); }

Unit * CGameServerDlg::GetUnitPtr(uint16 id)
{
	if (id < NPC_BAND)
		return GetUserPtr(id);

	return GetNpcPtr(id);
}

/**
* @brief	Spawn one or more event NPC/monsters.
*
* @param	sSid	  	The database ID of the NPC/monster to spawn.
* @param	bIsMonster	true if we are spawning a monster, false for an NPC.
* @param	byZone	  	The zone ID to spawn the monster to.
* @param	fX		  	The x coordinate.
* @param	fY		  	The y coordinate.
* @param	fZ		  	The z coordinate.
* @param	sCount	  	Number of spawns to create.
* @param	sRadius	  	Spawn radius.
*/
void CGameServerDlg::SpawnEventNpc(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ, uint16 sCount /*= 1*/, uint16 sRadius /*= 0*/, uint16 sDuration /*= 0*/, uint8 nation /*= 0*/,int16 socketID /*= -1*/, uint16 nEventRoom)
{
	Packet result(AG_NPC_SPAWN_REQ);
	result	<< sSid << bIsMonster 
		<< byZone 
		<< uint16(fX * 10) << uint16(fY * 10) << uint16(fZ * 10) 
		<< sCount 
		<< sRadius
		<< sDuration
		<< nation
		<< socketID
		<< nEventRoom;
	Send_AIServer(&result);
}

void CGameServerDlg::KillNpc(uint16 sNid)
{
	Packet result(AG_NPC_KILL_REQ);
	result	<< sNid;
	Send_AIServer(&result);
}

void CGameServerDlg::NpcUpdate(uint16 sSid, bool bIsMonster, uint8 byGroup, uint16 sPid)
{
	Packet result(AG_NPC_UPDATE);
	result	<< sSid << bIsMonster << byGroup << sPid;
	Send_AIServer(&result);
}

_PARTY_GROUP * CGameServerDlg::CreateParty(CUser *pLeader)
{
	_PARTY_GROUP * pParty = new _PARTY_GROUP;

	pLeader->m_bInParty = true;
	pLeader->m_sPartyIndex = m_sPartyIndex.increment();

	pParty->wIndex = pLeader->GetPartyID();
	pParty->uid[0] = pLeader->GetSocketID();
	if (!m_PartyArray.PutData(pParty->wIndex, pParty))
	{
		delete pParty;
		pLeader->m_bInParty = false;
		pLeader->m_sPartyIndex = -1;
		pLeader->m_sPartyRequest = -1;
		pParty = nullptr;
	}

	return pParty;
}

void CGameServerDlg::DeleteParty(uint16 sIndex)
{
	m_PartyArray.DeleteData(sIndex);
}

/// snow war visualdev

///visualdev notice snow
void CGameServerDlg::BattleZoneSnowNotice()
{
	int nWeekDay = g_localTime.tm_wday;
	int nMin = g_localTime.tm_min;
	int nSecond = g_localTime.tm_sec;
	uint32 nHour2 = g_localTime.tm_hour;

	if (!isWarOpen()&& nSecond == 0)
	{
		std::list<std::string> vargs = StrSplit(m_sBattleZoneSnowOpenDays, ",");
		uint8 nDaySize = vargs.size();
		if (nDaySize > 0)
		{
			uint8 nDay = 0;
			for (int i = 0; i < nDaySize; i++)
			{
				nDay = atoi(vargs.front().c_str());
				if (nDay == nWeekDay)
				{
					for (int x = 0; x < SNOW_TIME_COUNT; x++)
					{
						if(	(nHour2==m_nBattleZoneOpenHourSnowStart[x]-1 )&& nMin == 0)
						{
							m_SnowWarNoticeTime = 60;
							Announcement(IDS_PRE_SNOWBATTLE_ANNOUNCEMENT);
						}
						if(	(nHour2==m_nBattleZoneOpenHourSnowStart[x]-1 )&& nMin == 30)
						{
							m_SnowWarNoticeTime = 30;
							Announcement(IDS_PRE_SNOWBATTLE_ANNOUNCEMENT);
						}
						if(	(nHour2==m_nBattleZoneOpenHourSnowStart[x]-1 )&& nMin == 55)
						{
							m_SnowWarNoticeTime = 5;
							Announcement(IDS_PRE_SNOWBATTLE_ANNOUNCEMENT);
						}
						if(	(nHour2==m_nBattleZoneOpenHourSnowStart[x]-1 )&& nMin == 56)
						{
							m_SnowWarNoticeTime = 4;
							Announcement(IDS_PRE_SNOWBATTLE_ANNOUNCEMENT);
						}
						if(	(nHour2==m_nBattleZoneOpenHourSnowStart[x]-1 )&& nMin == 57)
						{
							m_SnowWarNoticeTime = 3;
							Announcement(IDS_PRE_SNOWBATTLE_ANNOUNCEMENT);
						}
						if(	(nHour2==m_nBattleZoneOpenHourSnowStart[x]-1 )&& nMin == 58)
						{
							m_SnowWarNoticeTime = 2;
							Announcement(IDS_PRE_SNOWBATTLE_ANNOUNCEMENT);
						}
						if(	(nHour2==m_nBattleZoneOpenHourSnowStart[x]-1 )&& nMin == 59)
						{
							m_SnowWarNoticeTime = 1;
							Announcement(IDS_PRE_SNOWBATTLE_ANNOUNCEMENT);
						}
					}
				}
				vargs.pop_front();
			}
		}
	}
}
/// end Snow
void CGameServerDlg::SnowBattleZoneOpenTimer()
{
	BattleZoneSnowNotice();
	int nWeekDay = g_localTime.tm_wday;
	int nHour = g_localTime.tm_hour;
	int nMin = g_localTime.tm_min;

	if (!isWarOpen() && nMin == 0)
	{
		std::list<std::string> vargs = StrSplit(m_sBattleZoneSnowOpenDays, ",");
		uint8 nDaySize = vargs.size();
		if (nDaySize > 0)
		{
			uint8 nDay = 0;
			for (int i = 0; i < nDaySize; i++)
			{
				nDay = atoi(vargs.front().c_str());
				if (nDay == nWeekDay)
				{
					for (int x = 0; x < SNOW_TIME_COUNT; x++)
					{
						if (m_nBattleZoneOpenHourSnowStart[x] == nHour && nMin == 0)
							BattleZoneOpen(SNOW_BATTLEZONE_OPEN/*, m_nBattlezoneOpenSnowWarZone[x]*/);
					}
				}
				vargs.pop_front();
			}
		}
	}
	else if (m_byBattleOpen == SNOW_BATTLE)	
	{
		SnowBattleZoneCurrentUsers();

		int32 WarElapsedTime = int32(UNIXTIME) - m_byBattleOpenedTime;
		m_byBattleRemainingTime = m_byBattleTime - WarElapsedTime;
		uint8 nBattleZone = ZONE_SNOW_BATTLE;

		if (m_bVictory == 0)
		{
			if (WarElapsedTime == (m_byBattleTime / 24)) // Select captain
			{
				m_KnightsRatingArray[KARUS_ARRAY].DeleteAllData();
				m_KnightsRatingArray[ELMORAD_ARRAY].DeleteAllData();
				LoadKnightsRankTable(true, true);
			}
			else if (WarElapsedTime == (m_byBattleTime / 2)) // War half time.
			{
				if (nBattleZone == ZONE_SNOW_BATTLE)
					BattleWinnerResult(BATTLE_WINNER_KILL);
			}

			m_sBattleTimeDelay++;

			if (WarElapsedTime >= m_byBattleTime) // War is over.
			{
				m_sBattleTimeDelay++;

				if (m_sBattleTimeDelay >= (m_byBattleTime / 24))
				{
					m_sBattleTimeDelay = 0;
					SnowBattleZoneClose();
				}
			}
		}
	}
	if (m_bySnowBanishFlag)
	{
		m_sBanishDelay++;

		if (m_sBanishDelay == (m_byBattleTime / 360))
			Announcement(SNOW_BAN);
		else if (m_sBanishDelay == (m_byBattleTime / 120)) {
			m_bySnowBanishFlag = false;
			m_sBanishDelay = 0;
			SnowBanishLosers();
		}
	}
}

void CGameServerDlg::SnowBattleZoneCurrentUsers()
{
	C3DMap* pMap = GetZoneByID(ZONE_SNOW_BATTLE);
	if (pMap == nullptr || m_nServerNo != pMap->m_nServerNo)
		return;

	uint16 nKarusMan = 0, nElmoradMan = 0;
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame() || pUser->GetZoneID() != pMap->GetID())
			continue;

		if (pUser->GetNation() == KARUS)
			nKarusMan++;
		else
			nElmoradMan++;
	}

	m_sKarusCount = nKarusMan;
	m_sElmoradCount = nElmoradMan;
}


void CGameServerDlg::SnowBattleZoneClose()
{
	BattleZoneOpen(SNOW_BATTLEZONE_CLOSE);
	Packet result(AG_BATTLE_EVENT, uint8(BATTLE_EVENT_OPEN));
	result << uint8(SNOW_BATTLEZONE_CLOSE);
	Send_AIServer(&result);
	ResetBattleZone();
	m_bySnowBanishFlag = true;
}

void CGameServerDlg::SnowBanishLosers()
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser *pUser = TO_USER(itr->second); 
		if (!pUser->isInGame())
			continue;

		// Reset captains
		if (!isWarOpen())
		{
			if (pUser->GetFame() == COMMAND_CAPTAIN)
				pUser->ChangeFame(CHIEF);
		}

		if (m_byBattleOpen == SNOW_BATTLE)
		{
			// Kick out losers
			if (pUser->GetMap()->isWarZone() && m_bVictory != pUser->GetNation())
				pUser->KickOutZoneUser(true);
		}
		else if (!g_pMain->isWarOpen())
		{
			// Kick out invaders
			if ((pUser->GetZoneID() <= ELMORAD && pUser->GetZoneID() != pUser->GetNation())
				|| pUser->GetMap()->isWarZone())
				pUser->KickOutZoneUser(true);
		}
	}
}

// end

uint32 CGameServerDlg::Timer_CheckGameEvents(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->BattleZoneOpenTimer();	// Check if it's time for the BattleZone to open or end.
		g_pMain->SnowBattleZoneOpenTimer(); // SnowWar
		g_pMain->TempleEventTimer();
		g_pMain->ForgettenTempleEventTimer();
		if (g_pMain->isWarOpen() && ((UNIXTIME - g_pMain->m_lastBlessTime) >= (5 * NATION_MONUMENT_REWARD_SECOND)))
		{
			g_pMain->CheckNationMonumentRewards();
		}
		if (g_pMain->CZBowlAktifMi)
		{
			g_pMain->BowlEventTimer();
		}
		sleep(1 * SECOND);
	}
	return 0;
}

uint32 CGameServerDlg::Timer_BifrostTime(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->SendEventRemainingTime(true, nullptr, ZONE_RONARK_LAND );
		if (g_pMain->m_BifrostVictory == 0)
		{
			g_pMain->m_sBifrostRemainingTime -= 60;

			if (g_pMain->m_sBifrostRemainingTime == 0 && !g_pMain->m_sBifrostWarStart)
			{
				g_pMain->m_BifrostVictory = 0;
				//g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostRemainingTime;
				g_pMain->m_sBifrostRemainingTime = g_pMain->CalcEventRemainingTime(); 
				g_pMain->m_sBifrostTime = 0;
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_START, Nation::ALL, false);

				if (!g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = true;

				g_pMain->m_sBifrostWarStart=true;
				g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostMonumentAttackTime;
				g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);
			}
			else if (g_pMain->m_sBifrostRemainingTime == 0 && g_pMain->m_sBifrostWarStart)
			{
				g_pMain->m_sBifrostWarStart=false;
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_FINISH, Nation::ALL, false);

				//g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostRemainingTime;
				g_pMain->m_sBifrostRemainingTime = g_pMain->CalcEventRemainingTime(); 
				g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);

				if (g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = false;
			}
		}
		else if (g_pMain->m_BifrostVictory != 0)
		{
			g_pMain->m_sBifrostTime -= 60;
			g_pMain->m_sBifrostRemainingTime = g_pMain->m_sBifrostTime;

			if(g_pMain->m_sBifrostTime <= 90 * MINUTE && !g_pMain->m_sBifrostVictoryNoticeAll)
			{
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_DRAW, Nation::ALL, false);
				g_pMain->m_sBifrostVictoryNoticeAll = true;
			}

			if (g_pMain->m_sBifrostTime == 0)
			{
				g_pMain->m_BifrostVictory = 0;
				//g_pMain->m_sBifrostRemainingTime = g_pMain->m_xBifrostRemainingTime;
				g_pMain->m_sBifrostRemainingTime = g_pMain->CalcEventRemainingTime(); 
				g_pMain->m_sBifrostTime = 0;
				g_pMain->SendFormattedResource(IDS_BEEF_ROAST_FINISH, Nation::ALL, false);

				if (g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = false;

				g_pMain->m_sBifrostWarStart=false;
				g_pMain->SendEventRemainingTime(true, nullptr, ZONE_BIFROST);
				g_pMain->KickOutZoneUsers(ZONE_BIFROST,ZONE_RONARK_LAND);
			}
			else if (g_pMain->m_sBifrostTime == g_pMain->m_xJoinOtherNationBifrostTime)
			{
				if (!g_pMain->m_bAttackBifrostMonument)
					g_pMain->m_bAttackBifrostMonument = true;
			}
		} 

		sleep(60 * SECOND);
	}
	return 0;
}

uint32 CGameServerDlg::Timer_UpdateGameTime(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->UpdateGameTime();

		if (++g_pMain->m_sErrorSocketCount > 3)//error 1
			g_pMain->AIServerConnect();

		sleep(6 * SECOND);
	}
	return 0;
}

uint32 CGameServerDlg::Timer_UpdateSessions(void * lpParam)
{
	while (g_bRunning)
	{
		SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
		foreach (itr, sessMap)
		{
			CUser * pUser = TO_USER(itr->second);

#ifndef DEBUG // ignore timeouts in debug builds, as we'll probably be pausing it with the debugger.
			uint32 timeout = KOSOCKET_TIMEOUT;

			// User has authed, but isn't in-game yet (creating a character, or is waiting for the game to load).
			if (!pUser->m_strAccountID.empty() && !pUser->isInGame())
				timeout = KOSOCKET_LOADING_TIMEOUT;

			// Disconnect timed out sessions
			if ((UNIXTIME - pUser->GetLastResponseTime()) >= timeout)
			{
				pUser->Disconnect();
				continue;
			}
#endif
			// Update the player, and hence any skill effects while we're here.
			if (pUser->isInGame())
				pUser->Update();
		}
		sleep(30 * SECOND);
	}
	return 0;
}

uint32 CGameServerDlg::Timer_UpdateConcurrent(void * lpParam)
{
	while (g_bRunning)
	{
		g_pMain->ReqUpdateConcurrent();
		//printf("%d user's data has been saved to server.\n",g_pMain->ReqUpdateDatas());
		sleep(60 * SECOND);
	}
	return 0;
}

uint32 CGameServerDlg::ReqUpdateDatas()
{
	int count = 0;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser *pUser = TO_USER(itr->second);
		if (pUser != nullptr){
			if (pUser->isInGame()){
				count++;
				pUser->m_lastSaveTime = UNIXTIME; // this is set by UpdateUser(), however it may result in multiple requests unless it's set first.
				if(!pUser->isTrading() || !pUser->isMerchanting() || !pUser->isStoreOpen() || !pUser->m_bMerchantStatex == 1 ){
					g_DBAgent.UpdateUser(pUser->GetName(), UPDATE_LOGOUT, pUser);
					g_DBAgent.UpdateWarehouseData(pUser->GetAccountName(), UPDATE_LOGOUT, pUser);
					g_DBAgent.UpdateSavedMagic(pUser);
				}

				//Sleep(50);//visualdev
			}
		}
	}
	return count;

}

void CGameServerDlg::ReqUpdateConcurrent()
{
	Packet result(WIZ_ZONE_CONCURRENT);
	result	<< uint32(m_nServerNo)
		<< uint32(m_socketMgr.GetActiveSessionMap().size());
	AddDatabaseRequest(result);
}

void CGameServerDlg::AIServerConnect()
{
	// Are there any (note: we only use 1 now) idle/disconnected sessions?
	SessionMap & sessMap = m_aiSocketMgr.GetIdleSessionMap();

	// Copy the map (should only be 1 socket anyway) to avoid breaking the iterator
	SessionMap idleSessions = sessMap;

	// No idle sessions? Excellent.
	if (idleSessions.empty())
		return;

	// Attempt reconnecting to the server
	foreach (itr, idleSessions)
	{
		CAISocket *pSock = static_cast<CAISocket *>(itr->second);
		bool bReconnecting = pSock->IsReconnecting();
		if (!pSock->Connect(m_AIServerIP.c_str(), m_AIServerPort)) // couldn't connect... let's leave you alone for now
			continue;

		// Connected! Now send the connection packet.
		Packet result(AI_SERVER_CONNECT);
		result << bReconnecting;
		pSock->Send(&result);

		TRACE("**** AISocket Connect Success!! , server = %s:%d ****\n", pSock->GetRemoteIP().c_str(), pSock->GetRemotePort());
	}
}

/**
* @brief	Sends a packet to all users in the zone matching the specified class types.
*
* @param	pkt				   	The packet.
* @param	bZoneID			   	Identifier for the zone.
* @param	pExceptUser		   	The except user.
* @param	nation			   	The nation.
* @param	seekingPartyOptions	Bitmask of classes to send to.
*/
void CGameServerDlg::Send_Zone_Matched_Class(Packet *pkt, uint8 bZoneID, CUser* pExceptUser, 
											 uint8 nation, uint8 seekingPartyOptions, uint16 nEventRoom)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == pExceptUser 
			|| !pUser->isInGame()
			|| pUser->GetZoneID() != bZoneID
			|| pUser->isInParty()) // looking for users to join the party
			continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		// If we're in the neutral zone (Moradon), it doesn't matter which nation we party with.
		// For all other zones, we must party with a player of the same nation.
		if (pUser->GetZoneID() == ZONE_MORADON 
			|| pUser->GetNation() == nation)
		{
			if (	((seekingPartyOptions & 1) && pUser->JobGroupCheck(ClassWarrior))
				||	((seekingPartyOptions & 2) && pUser->JobGroupCheck(ClassRogue))
				||	((seekingPartyOptions & 4) && pUser->JobGroupCheck(ClassMage))
				||	((seekingPartyOptions & 8) && pUser->JobGroupCheck(ClassPriest)))
				pUser->Send(pkt);
		}
	}
}

/**
* @brief	Sends a packet to all players in the specified zone
* 			matching the specified criteria.
*
* @param	pkt		   	The packet.
* @param	bZoneID	   	Zone's identifier.
* @param	pExceptUser	User to except. If specified, will ignore this user.
* @param	nation	   	Nation to allow. If unspecified, will default to Nation::ALL 
* 						which will send to all/both nations.
*/
void CGameServerDlg::Send_Zone(Packet *pkt, uint8 bZoneID, CUser* pExceptUser /*= nullptr*/, uint8 nation /*= 0*/, uint16 nEventRoom, float fRange)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);

		if (!pUser->isInGame()
			|| pUser->GetZoneID() != bZoneID
			|| (nation != Nation::ALL && nation != pUser->GetNation()))
		{
			if (pExceptUser != nullptr)
			{
				if (pUser == pExceptUser
					|| (fRange > 0.0f && pUser->isInRange(pExceptUser, fRange)))
					continue;
			}

			continue;
		}

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		pUser->Send(pkt);
	}
}

/**
* @brief	Sends a packet to all users connected and logged into the server.
*
* @param	pkt		   	The packet.
* @param	pExceptUser	User to except. If specified, will ignore this user.
* @param	nation	   	Nation to allow. If unspecified, will default to Nation::ALL 
* 						which will send to all/both nations.
*/
void CGameServerDlg::Send_All(Packet *pkt, CUser* pExceptUser /*= nullptr*/, uint8 nation /*= 0*/, 
							  uint8 ZoneID /*= 0*/, bool isSendEventUsers /* false */, uint16 nEventRoom /*= -1*/)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == pExceptUser 
			|| !pUser->isInGame()
			|| (nation != Nation::ALL && nation != pUser->GetNation()))
			continue;

		if (ZoneID != 0)
			if (pUser->GetZoneID() != ZoneID) 
				continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		/*if (isSendEventUsers && !pUser->isEventUser())
		continue;*/

		pUser->Send(pkt);
	}
}

void CGameServerDlg::Send_Region(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom)
{
	foreach_region(rx, rz)
		Send_UnitRegion(pkt, pMap, rx + x, rz + z, pExceptUser, nEventRoom);
}

void CGameServerDlg::Send_UnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, CUser *pExceptUser, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(x, z);
	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;

	// Protect array changes...
	ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
	foreach (itr, cm_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| pUser == pExceptUser 
			|| !pUser->isInGame())
			continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		pUser->Send(pkt);
	}
}

// TODO: Move the following two methods into a base CUser/CNpc class
void CGameServerDlg::Send_OldRegions(Packet *pkt, int old_x, int old_z, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom)
{
	if (old_x != 0)
	{
		Send_UnitRegion(pkt, pMap, x+old_x*2, z+old_z-1, nullptr, nEventRoom);
		Send_UnitRegion(pkt, pMap, x+old_x*2, z+old_z, nullptr, nEventRoom);
		Send_UnitRegion(pkt, pMap, x+old_x*2, z+old_z+1, nullptr, nEventRoom);
	}

	if (old_z != 0)
	{
		Send_UnitRegion(pkt, pMap, x+old_x, z+old_z*2, nullptr, nEventRoom);
		if (old_x < 0)
			Send_UnitRegion(pkt, pMap, x+old_x+1, z+old_z*2, nullptr, nEventRoom);
		else if (old_x > 0)
			Send_UnitRegion(pkt, pMap, x+old_x-1, z+old_z*2, nullptr, nEventRoom);
		else
		{
			Send_UnitRegion(pkt, pMap, x+old_x-1, z+old_z*2, nullptr, nEventRoom);
			Send_UnitRegion(pkt, pMap, x+old_x+1, z+old_z*2, nullptr, nEventRoom);
		}
	}
}

void CGameServerDlg::Send_NewRegions(Packet *pkt, int new_x, int new_z, C3DMap *pMap, int x, int z, CUser* pExceptUser, uint16 nEventRoom)
{
	if (new_x != 0)
	{
		Send_UnitRegion(pkt, pMap, x+new_x, z-1, nullptr, nEventRoom);
		Send_UnitRegion(pkt, pMap, x+new_x, z, nullptr, nEventRoom);
		Send_UnitRegion(pkt, pMap, x+new_x, z+1, nullptr, nEventRoom);
	}

	if (new_z != 0)
	{
		Send_UnitRegion(pkt, pMap, x, z+new_z);

		if (new_x < 0)
			Send_UnitRegion(pkt, pMap, x+1, z+new_z, nullptr, nEventRoom);
		else if (new_x > 0)
			Send_UnitRegion(pkt, pMap, x-1, z+new_z, nullptr, nEventRoom);
		else 
		{
			Send_UnitRegion(pkt, pMap, x-1, z+new_z, nullptr, nEventRoom);
			Send_UnitRegion(pkt, pMap, x+1, z+new_z, nullptr, nEventRoom);
		}
	}
}

void CGameServerDlg::Send_NearRegion(Packet *pkt, C3DMap *pMap, int region_x, int region_z, float curx, float curz, CUser* pExceptUser, uint16 nEventRoom)
{
	int left_border = region_x * VIEW_DISTANCE, top_border = region_z * VIEW_DISTANCE;
	Send_FilterUnitRegion(pkt, pMap, region_x, region_z, curx, curz, pExceptUser, nEventRoom);
	if( ((curx - left_border) > (VIEW_DISTANCE/2.0f)) ) {			// RIGHT
		if( ((curz - top_border) > (VIEW_DISTANCE/2.0f)) ) {	// BOTTOM
			Send_FilterUnitRegion(pkt, pMap, region_x+1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z+1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x+1, region_z+1, curx, curz, pExceptUser, nEventRoom);
		}
		else {													// TOP
			Send_FilterUnitRegion(pkt, pMap, region_x+1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z-1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x+1, region_z-1, curx, curz, pExceptUser, nEventRoom);
		}
	}
	else {														// LEFT
		if( ((curz - top_border) > (VIEW_DISTANCE/2.0f)) ) {	// BOTTOM
			Send_FilterUnitRegion(pkt, pMap, region_x-1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z+1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x-1, region_z+1, curx, curz, pExceptUser, nEventRoom);
		}
		else {													// TOP
			Send_FilterUnitRegion(pkt, pMap, region_x-1, region_z, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x, region_z-1, curx, curz, pExceptUser, nEventRoom);
			Send_FilterUnitRegion(pkt, pMap, region_x-1, region_z-1, curx, curz, pExceptUser, nEventRoom);
		}
	}
}

void CGameServerDlg::Send_FilterUnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, float ref_x, float ref_z, CUser *pExceptUser, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(x, z);
	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;
	// Protect array changes...
	ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
	foreach (itr, cm_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| pUser == pExceptUser 
			|| !pUser->isInGame())
			continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		if (sqrt(pow((pUser->m_curx - ref_x), 2) + pow((pUser->m_curz - ref_z), 2)) < 32)
			pUser->Send(pkt);
	}
}

void CGameServerDlg::Send_PartyMember(int party, Packet *result)
{
	_PARTY_GROUP* pParty = GetPartyPtr(party);
	if (pParty == nullptr)
		return;

	for (int i = 0; i < MAX_PARTY_USERS; i++)
	{
		CUser *pUser = GetUserPtr(pParty->uid[i]);
		if (pUser == nullptr)
			continue;



		pUser->Send(result);
	}
}

void CGameServerDlg::Send_KnightsMember(int index, Packet *pkt)
{
	CKnights* pKnights = GetClanPtr(index);
	if (pKnights == nullptr)
		return;

	pKnights->Send(pkt);
}

void CGameServerDlg::Send_KnightsAlliance(uint16 sAllianceID, Packet *pkt)
{
	_KNIGHTS_ALLIANCE* pAlliance = GetAlliancePtr(sAllianceID);
	if (pAlliance == nullptr)
		return;

	Send_KnightsMember(pAlliance->sMainAllianceKnights, pkt);
	Send_KnightsMember(pAlliance->sSubAllianceKnights, pkt);
	Send_KnightsMember(pAlliance->sMercenaryClan_1, pkt);
	Send_KnightsMember(pAlliance->sMercenaryClan_2, pkt);
}

void CGameServerDlg::Send_AIServer(Packet *pkt)
{
	m_aiSocketMgr.SendAll(pkt);
}

void CGameServerDlg::UpdateGameTime()
{
	DateTime now(&g_localTime);

	// Check timed King events.
	foreach_stlmap (itr, m_KingSystemArray)
		itr->second->CheckKingTimer();

	// Every minute
	if (m_sMin != now.GetMinute())
	{
		m_ReloadKnightAndUserRanksMinute++;

		if (m_ReloadKnightAndUserRanksMinute == RELOAD_KNIGHTS_AND_USER_RATING)
		{
			m_ReloadKnightAndUserRanksMinute = 0;
			ReloadKnightAndUserRanks();
		}

		// Player Ranking Rewards
		std::list<std::string> vargs = StrSplit(m_sPlayerRankingsRewardZones, ",");//error 1
		uint8 nZones = vargs.size();
		if (nZones > 0)
		{
			uint8 nZoneID = 0;
			for (int i = 0; i < nZones; i++)
			{
				nZoneID = atoi(vargs.front().c_str());
				/*SetPlayerRankingRewards(nZoneID);*/
				vargs.pop_front();
			}
		}
	}

	// Every hour
	if (m_sHour != now.GetHour())
	{
		ResetPlayerRankings();
		UpdateWeather();
		SetGameTime();

		if (m_bSantaOrAngel)
			SendFlyingSantaOrAngel();
	}

	// Every day
	if (m_sDate != now.GetDay())
	{

	}

	// Every month
	if (m_sMonth != now.GetMonth() || (m_sDate == 14 && m_sDate != now.GetDay() && onbesinde))
	{
		// Reset monthly NP.
		CGameServerDlg::ResetLoyaltyMonthly();
	}

	// Update the server time
	m_sYear = now.GetYear();
	m_sMonth = now.GetMonth();
	m_sDate = now.GetDay();
	m_sHour = now.GetHour();
	m_sMin = now.GetMinute();

	Packet result(AG_TIME_WEATHER);
	result << m_sYear << m_sMonth << m_sDate << m_sHour << m_sMin << m_byWeather << m_sWeatherAmount;
	Send_AIServer(&result);
}

void CGameServerDlg::ResetLoyaltyMonthly()
{
	SessionMap sessMap = m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		pUser->m_iLoyaltyMonthly = 0;
		pUser->SendLoyaltyChange(); // update the client (note: official doesn't bother)
	}

	// Attempt to update the database in this thread directly, while the session map's locked.
	// This prevents new users from logging in before they've been reset (hence keeping last month's total).
	g_DBAgent.ResetLoyaltyMonthly();
}

void CGameServerDlg::UpdateWeather()
{
	if (m_byKingWeatherEvent)
	{
		int16 sEventExpiry;
		if (g_localTime.tm_mday == m_byKingWeatherEvent_Day)
			sEventExpiry = g_localTime.tm_min + 60 * (g_localTime.tm_hour - m_byKingWeatherEvent_Hour) - m_byKingWeatherEvent_Minute;
		else
			sEventExpiry = g_localTime.tm_min + 60 * (g_localTime.tm_hour - m_byKingWeatherEvent_Hour + 24) - m_byKingWeatherEvent_Minute;

		// Weather events last for 5 minutes
		if (sEventExpiry > 5)
		{
			m_byKingWeatherEvent = 0;
			m_byKingWeatherEvent_Day = 0;
			m_byKingWeatherEvent_Hour = 0;
			m_byKingWeatherEvent_Minute = 0;
		}
	}
	else
	{
		int weather = 0, rnd = myrand( 0, 100 );
		if (rnd < 2)		weather = WEATHER_SNOW;
		else if (rnd < 7)	weather = WEATHER_RAIN;
		else				weather = WEATHER_FINE;

		m_sWeatherAmount = myrand(0, 100);
		if (weather == WEATHER_FINE)
		{
			if (m_sWeatherAmount > 70)
				m_sWeatherAmount /= 2;
			else
				m_sWeatherAmount = 0;
		}
		m_byWeather = weather;
	}

	// Real weather data for most users.
	Packet realWeather(WIZ_WEATHER, m_byWeather);
	realWeather << m_sWeatherAmount;

	// Fake, clear weather for users in certain zones (e.g. Desp & Hell Abysses, Arena)
	Packet fakeWeather(WIZ_WEATHER, uint8(WEATHER_FINE));
	fakeWeather << m_sWeatherAmount;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame())
			continue;

		if (pUser->GetZoneID() == 32 
			|| pUser->GetZoneID() == 33
			|| pUser->GetZoneID() == 48)
			pUser->Send(&fakeWeather);
		else
			pUser->Send(&realWeather);
	}
}

void CGameServerDlg::SetGameTime()
{
	CIni ini(CONF_GAME_SERVER);
	ini.SetInt( "TIMER", "WEATHER", m_byWeather );
}

void CGameServerDlg::ResetPlayerRankings(uint8 ZoneID)
{
	if (m_sRankResetHour == m_nPlayerRankingResetTime || ZoneID > 0)
	{
		m_IsPlayerRankingUpdateProcess = true;
		m_sRankResetHour = 0;

		for (int nation = KARUS_ARRAY; nation <= ELMORAD_ARRAY; nation++)
		{
			foreach_stlmap (itr, g_pMain->m_UserRankingArray[nation])
			{
				_USER_RANKING * pRankInfo = g_pMain->m_UserRankingArray[nation].GetData(itr->first);

				if (pRankInfo == nullptr)
					continue;

				CUser * pUser = g_pMain->GetUserPtr(pRankInfo->m_socketID);

				if (pUser == nullptr 
					|| pUser->isInTempleEventZone()
					|| (ZoneID > 0 && pUser->GetZoneID() != ZoneID))
					continue;

				pRankInfo->m_iLoyaltyDaily = 0;
				pRankInfo->m_iLoyaltyPremiumBonus = 0;
				pUser->m_iLoyaltyDaily = 0;
				pUser->m_iLoyaltyPremiumBonus = 0;
			}
		}

		m_IsPlayerRankingUpdateProcess = false; 
		return;
	}

	m_sRankResetHour++;
}

void CGameServerDlg::AddDatabaseRequest(Packet & pkt, CUser *pUser /*= nullptr*/)
{
	Packet *newPacket = new Packet(pkt.GetOpcode(), pkt.size() + 2);
	*newPacket << int16(pUser == nullptr ? -1 : pUser->GetSocketID());
	if (pkt.size())
		newPacket->append(pkt.contents(), pkt.size());
	DatabaseThread::AddRequest(newPacket);
}

void CGameServerDlg::UserInOutForMe(CUser *pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_REQ_USERIN);
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 user_count = 0;

	result << uint16(0); // placeholder for the user count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionUserIn(pMap, rx + x, rz + z, result, user_count, pSendUser->GetEventRoom());

	result.put(0, uint16(user_count));
	pSendUser->SendCompressed(&result);
}

void CGameServerDlg::RegionUserInOutForMe(CUser *pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_REGIONCHANGE);
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 user_count = 0;

	result << uint16(0); // placeholder for the user count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionUserList(pMap, rx + x, rz + z, result, user_count, pSendUser->GetEventRoom());

	result.put(0, user_count);
	pSendUser->SendCompressed(&result);
}

void CGameServerDlg::GetRegionUserIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;

	ZoneUserArray cm_RegionUserArray = pRegion->m_RegionUserArray;
	foreach (itr, cm_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| !pUser->isInGame())
			continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		pkt << uint8(0) << pUser->GetSocketID();
		pUser->GetUserInfo(pkt);
		t_count++;
	}
}

void CGameServerDlg::GetRegionUserList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;

	foreach (itr, pRegion->m_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| !pUser->isInGame())
			continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		pkt << pUser->GetSocketID();
		t_count++;
	}
}

void CGameServerDlg::MerchantUserInOutForMe(CUser *pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_MERCHANT_INOUT, uint8(1));
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 user_count = 0;

	result << uint16(0); // placeholder for user count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionMerchantUserIn(pMap, rx + x, rz + z, result, user_count, pSendUser->GetEventRoom());

	result.put(1, user_count);
	pSendUser->SendCompressed(&result);
}

void CGameServerDlg::GetRegionMerchantUserIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
{
	if (pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion *pRegion = pMap->GetRegion(region_x, region_z);
	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionUserArray.size() <= 0)
		return;

	foreach (itr, pRegion->m_RegionUserArray)
	{
		CUser *pUser = GetUserPtr(*itr);
		if (pUser == nullptr 
			|| !pUser->isInGame()
			|| !pUser->isMerchanting())
			continue;

		if (nEventRoom > 0 && nEventRoom != pUser->GetEventRoom())
			continue;

		pkt << pUser->GetSocketID()
			//<< pUser->GetMerchantState() // 0 is selling, 1 is buying
			<< (pUser->GetMerchantState() == 1 ? false : pUser->m_bPremiumMerchant); // Type of merchant [normal - gold] // bool

		for (int i = 0, listCount = (pUser->GetMerchantState() == 1 ? 4 : (pUser->m_bPremiumMerchant ? 8 : 4)); i < listCount; i++)
			pkt << pUser->m_arMerchantItems[i].nNum;

		t_count++;
	}
}

void CGameServerDlg::NpcInOutForMe(CUser* pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_REQ_NPCIN);
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 npc_count = 0;
	result << uint16(0); // placeholder for NPC count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionNpcIn(pMap, rx + x, rz + z, result, npc_count, pSendUser->GetEventRoom(), pSendUser);

	result.put(0, npc_count);
	pSendUser->SendCompressed(&result);
}

void CGameServerDlg::GetRegionNpcIn(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom, CUser* pSendUser)
{
	if (!m_bPointCheckFlag
		|| pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion * pRegion = pMap->GetRegion(region_x, region_z);

	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionNpcArray.size() <= 0)
		return;

	CKnights *pKnights = g_pMain->GetClanPtr(pSendUser->m_bKnights);
	_KNIGHTS_SIEGE_WARFARE *pSiegeWars = g_pMain->GetSiegeMasterKnightsPtr(1);

	foreach (itr, pRegion->m_RegionNpcArray)
	{
		CNpc *pNpc = GetNpcPtr(*itr);
		if (pNpc == nullptr
			|| pNpc->isDead())
			continue;

		if (nEventRoom > 0 && nEventRoom != pNpc->m_bEventRoom)
			continue;

		if (pNpc->m_sSid == 541 && pNpc->GetType() == NPC_DESTROYED_ARTIFACT && pNpc->m_bZone == ZONE_DELOS)
		{
			if ( pKnights != nullptr && pSiegeWars != nullptr)
			{
				if ((pSiegeWars->sMasterKnights == pKnights->m_sAlliance && pKnights->m_sAlliance != 0 ) || pSiegeWars->sMasterKnights == pKnights->m_sIndex)
					pNpc->m_bNation = pKnights->m_byNation ;
				else
					pNpc->m_bNation = 0;
			}
		}
			pkt << pNpc->GetID();
			pNpc->GetNpcInfo(pkt);
			t_count++;
	}
}

void CGameServerDlg::RegionNpcInfoForMe(CUser *pSendUser)
{
	if (pSendUser == nullptr)
		return;

	Packet result(WIZ_NPC_REGION);
	C3DMap* pMap = pSendUser->GetMap();
	ASSERT(pMap != nullptr);
	uint16 npc_count = 0;
	result << uint16(0); // placeholder for NPC count

	int16 rx = pSendUser->GetRegionX(), rz = pSendUser->GetRegionZ();
	foreach_region(x, z)
		GetRegionNpcList(pMap, rx + x, rz + z, result, npc_count, pSendUser->GetEventRoom());

	result.put(0, npc_count);
	pSendUser->SendCompressed(&result);
}

void CGameServerDlg::GetUnitListFromSurroundingRegions(Unit * pOwner, std::vector<uint16> * pList)
{
	if (pOwner == nullptr)
		return;

	C3DMap * pMap = pOwner->GetMap();
	ASSERT(pMap != nullptr);

	int16 rx = pOwner->GetRegionX(), rz = pOwner->GetRegionZ();

	Guard lock(pMap->m_lock);
	foreach_region(x, z)
	{
		uint16 region_x = rx + x, region_z = rz + z;
		CRegion * pRegion = pMap->GetRegion(region_x, region_z);

		if (pRegion == nullptr)
			continue;

		Guard lock2(pRegion->m_lock);

		// Add all potential NPCs to list
		foreach (itr, pRegion->m_RegionNpcArray)
			pList->push_back(*itr);

		// Add all potential users to list
		foreach (itr, pRegion->m_RegionUserArray)
			pList->push_back(*itr);
	}
}

void CGameServerDlg::GetRegionNpcList(C3DMap *pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom)
{
	if (!m_bPointCheckFlag
		|| pMap == nullptr)
		return;

	Guard lock(pMap->m_lock);
	CRegion * pRegion = pMap->GetRegion(region_x, region_z);

	if (pRegion == nullptr)
		return;

	Guard lock2(pRegion->m_lock);

	if (pRegion->m_RegionNpcArray.size() <= 0)
		return;

	foreach (itr, pRegion->m_RegionNpcArray)
	{
		CNpc *pNpc = GetNpcPtr(*itr);
		if (pNpc == nullptr || pNpc->isDead())
			continue;

		if (nEventRoom > 0 && nEventRoom != pNpc->m_bEventRoom)
			continue;

		pkt << pNpc->GetID();
		t_count++;
	}
}

void CGameServerDlg::HandleConsoleCommand(const char * msg) 
{
	string message = msg;
	if (message.empty())
		return;

	if (ProcessServerCommand(message))
	{
		printf("Command accepted.\n");
		return;
	}

	printf("Invalid command. If you're trying to send a notice, please use /notice\n");
}

bool CGameServerDlg::LoadNoticeData()
{
	ifstream file("./Notice.txt");
	string line;
	int count = 0;

	// Clear out the notices first
	memset(&m_ppNotice, 0, sizeof(m_ppNotice));

	if (!file)
	{
		TRACE("Notice.txt could not be opened.\n");
		return false;
	}

	while (!file.eof())
	{
		if (count > 19)
		{
			TRACE("Too many lines in Notice.txt\n");
			break;
		}

		getline(file, line);
		if (line.length() > 128)
		{
			TRACE("Notice.txt contains line that exceeds the limit of 128 characters.\n");
			break;
		}

		strcpy(m_ppNotice[count++], line.c_str());
	}

	file.close();
	return true;
}

void CGameServerDlg::SendAllUserInfo()
{
	Packet result(AG_USER_INFO_ALL);
	uint8 count = 0;
	result << count; // placeholder for user count
	const int tot = 20;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		TO_USER(itr->second)->GetUserInfoForAI(result);
		if (++count == tot)	{
			result.put(0, count);
			Send_AIServer(&result);
			count = 0;
			result.clear();
		}
	}

	if (count != 0 && count < (tot - 1))
	{
		result.put(0, count);
		Send_AIServer(&result);
		count = 0;
		result.clear();
	}

	foreach_stlmap (itr, m_PartyArray)
	{
		_PARTY_GROUP *pParty = itr->second;
		if (pParty == nullptr) 
			continue;

		result.Initialize(AG_PARTY_INFO_ALL);
		result << uint16(itr->first);
		for (int i = 0; i < MAX_PARTY_USERS; i++)
			result << pParty->uid[i];

		Send_AIServer(&result);
	}
}

void CGameServerDlg::DeleteAllNpcList(int flag)
{
	if (!m_bServerCheckFlag
		|| !m_bPointCheckFlag)
		return;

	TRACE("[Monster Point Delete]\n");
	TRACE("*** DeleteAllNpcList - Start *** \n");

	// Remove spawns from users to prevent them from getting bugged...
	m_arNpcArray.m_lock.lock();
	foreach_stlmap_nolock (itr, m_arNpcArray)
	{
		if (itr->second->isAlive())
			itr->second->SendInOut(INOUT_OUT, 0.0f, 0.0f, 0.0f);

		// decrease the reference count (freeing it if nothing else is using it)
		itr->second->DecRef();
	}

	// all the data should now be freed (if not, it will be by whatever's using it)
	m_arNpcArray.m_UserTypeMap.clear();
	m_arNpcArray.m_lock.unlock();

	// Now remove all spawns from all regions
	foreach_stlmap (itr, m_ZoneArray)
	{
		C3DMap *pMap = itr->second;
		if (pMap == nullptr)
			continue;

		Guard lock(pMap->m_lock);
		for (int i = 0; i < pMap->GetXRegionMax(); i++)
		{
			for (int j = 0; j < pMap->GetZRegionMax(); j++)
				pMap->m_ppRegion[i][j].m_RegionNpcArray.clear();
		}
	}
	m_bServerCheckFlag = false;

	TRACE("*** DeleteAllNpcList - End *** \n");
}

/**
* @brief	Searches for the first NPC in the specified zone
* 			with the specified picture/model ID.
*
* @param	sPid	Picture/model ID of the NPC.
* @param	byZone	Zone to search in.
*
* @return	null if it fails, else the NPC instance we found.
*/
CNpc*  CGameServerDlg::FindNpcInZone(uint16 sPid, uint8 byZone)
{
	if (!m_bPointCheckFlag)
		return nullptr;

	Guard lock(m_arNpcArray.m_lock);
	foreach_stlmap (itr, m_arNpcArray)
	{
		CNpc * pNpc = itr->second;
		if (pNpc == nullptr || pNpc->GetZoneID() != byZone
			// This isn't a typo, it's actually just a hack.
				// The picture/model ID of most spawns is the same as their prototype ID.
					// When there's more than one spawn prototype (i.e. different sSid), we keep using
						// the same picture/model ID. So we check this instead of the sSid...
							|| pNpc->m_sPid != sPid) 
							continue;

		return pNpc;
	}

	return nullptr;
}
///visualdev notice
void CGameServerDlg::BattleZoneNotice()
{
	int nWeekDay = g_localTime.tm_wday;
	int nMin = g_localTime.tm_min;
	int nSecond = g_localTime.tm_sec;
	uint32 nHour2 = g_localTime.tm_hour;

	if (!isWarOpen()&& nSecond == 0)
	{
		std::list<std::string> vargs = StrSplit(m_sBattleZoneOpenDays, ",");
		uint8 nDaySize = vargs.size();
		if (nDaySize > 0)
		{
			uint8 nDay = 0;
			for (int i = 0; i < nDaySize; i++)
			{
				nDay = atoi(vargs.front().c_str());
				if (nDay == nWeekDay)
				{
					for (int x = 0; x < WAR_TIME_COUNT; x++)
					{
						if(	(nHour2==m_nBattleZoneOpenHourStart[x]-1 )&& nMin == 0)
						{
							m_WarNoticeTime = 60;
							Announcement(IDS_PRE_BATTLE_ANNOUNCEMENT);
						}
						if(	(nHour2==m_nBattleZoneOpenHourStart[x]-1 )&& nMin == 30)
						{
							m_WarNoticeTime = 30;
							Announcement(IDS_PRE_BATTLE_ANNOUNCEMENT);
						}
						if(	(nHour2==m_nBattleZoneOpenHourStart[x]-1 )&& nMin == 55)
						{
							m_WarNoticeTime = 5;
							Announcement(IDS_PRE_BATTLE_ANNOUNCEMENT);
						}
						if(	(nHour2==m_nBattleZoneOpenHourStart[x]-1 )&& nMin == 56)
						{
							m_WarNoticeTime = 4;
							Announcement(IDS_PRE_BATTLE_ANNOUNCEMENT);
						}
						if(	(nHour2==m_nBattleZoneOpenHourStart[x]-1 )&& nMin == 57)
						{
							m_WarNoticeTime = 3;
							Announcement(IDS_PRE_BATTLE_ANNOUNCEMENT);
						}
						if(	(nHour2==m_nBattleZoneOpenHourStart[x]-1 )&& nMin == 58)
						{
							m_WarNoticeTime = 2;
							Announcement(IDS_PRE_BATTLE_ANNOUNCEMENT);
						}
						if(	(nHour2==m_nBattleZoneOpenHourStart[x]-1 )&& nMin == 59)
						{
							m_WarNoticeTime = 1;
							Announcement(IDS_PRE_BATTLE_ANNOUNCEMENT);
						}
					}
				}
				vargs.pop_front();
			}
		}
	}
}
/// end
void CGameServerDlg::BattleZoneOpenTimer()
{
	BattleZoneNotice();
	int nWeekDay = g_localTime.tm_wday;
	int nHour = g_localTime.tm_hour;
	int nMin = g_localTime.tm_min;

	CUser* pUser = g_pMain->GetUserPtr("Heros", TYPE_CHARACTER);

	if (!isWarOpen() && nMin == 0)
	{
		std::list<std::string> vargs = StrSplit(m_sBattleZoneOpenDays, ",");
		uint8 nDaySize = vargs.size();
		if (nDaySize > 0)
		{
			uint8 nDay = 0;
			for (int i = 0; i < nDaySize; i++)
			{
				nDay = atoi(vargs.front().c_str());
				if (nDay == nWeekDay)
				{
					for (int x = 0; x < WAR_TIME_COUNT; x++)
					{
						if (m_nBattleZoneOpenHourStart[x] == nHour && nMin == 0)
							BattleZoneOpen(BATTLEZONE_OPEN, m_nBattlezoneOpenWarZone[x]);
					}
				}
				vargs.pop_front();
			}
		}
	}
	else if (m_byBattleOpen == NATION_BATTLE)	
	{
		BattleZoneCurrentUsers();

		int32 WarElapsedTime = int32(UNIXTIME) - m_byBattleOpenedTime;
		m_byBattleRemainingTime = m_byBattleTime - WarElapsedTime;
		uint8 nBattleZone = g_pMain->m_byBattleZone + ZONE_BATTLE_BASE;

		if (m_bVictory == 0)
		{
			if (WarElapsedTime == (m_byBattleTime / 24)) // Select captain
			{
				m_KnightsRatingArray[KARUS_ARRAY].DeleteAllData();
				m_KnightsRatingArray[ELMORAD_ARRAY].DeleteAllData();
				LoadKnightsRankTable(true, true);
			}
			else if (WarElapsedTime == (m_byBattleTime / 8) && nBattleZone == ZONE_BATTLE4) // War half time + 15 minutes for Nereid's Island.
			{
				if (nBattleZone == ZONE_BATTLE4) // Nereid's Island
				{
					if (m_sKarusMonuments >= 7 && m_sElmoMonuments == 0)
						BattleZoneResult(KARUS);
					else if (m_sKarusMonuments == 0 && m_sElmoMonuments >= 7)
						BattleZoneResult(ELMORAD);
				}
			}
			else if (WarElapsedTime == (m_byBattleTime / 2)) // War half time.
			{
				if (nBattleZone == ZONE_BATTLE
					|| nBattleZone == ZONE_BATTLE2 
					||nBattleZone == ZONE_BATTLE3)
					BattleWinnerResult(BATTLE_WINNER_NPC);
				else if (nBattleZone == ZONE_BATTLE4) // Nereid's Island
					BattleWinnerResult(BATTLE_WINNER_MONUMENT);
				else if (nBattleZone == ZONE_BATTLE6) // Oreads
					BattleWinnerResult(BATTLE_WINNER_KILL);
			}

			m_sBattleTimeDelay++;

			if (m_sBattleTimeDelay >= (nBattleZone == ZONE_BATTLE4 ? (m_byBattleTime / 48) : (m_byBattleTime / 24)))
			{
				m_sBattleTimeDelay = 0;
				Announcement(DECLARE_BATTLE_ZONE_STATUS);
			}
		}
		else
		{
			if (WarElapsedTime <  m_byBattleTime) // Won the war.
			{
				m_sBattleTimeDelay++;

				if (m_sBattleTimeDelay >= (m_byBattleTime / 24))
				{
					m_sBattleTimeDelay = 0;
					Announcement(UNDER_ATTACK_NOTIFY);
				}
			}
		}

		if (m_bResultDelay)
		{
			m_sBattleResultDelay++;
			if (m_sBattleResultDelay == (m_byBattleTime  / (m_byBattleTime / 10))) {
				m_bResultDelay = false;
				BattleZoneResult(m_bResultDelayVictory);
			}
		}

		if (WarElapsedTime >= m_byBattleTime) // War is over.
			BattleZoneClose();
	}
	else if (m_byBattleOpen == CLAN_BATTLE)
	{
		if (m_byBattleSiegeWarStartTime == 0)
		{
			m_sBattleTimeDelay++;
		}
		else if (m_byBattleSiegeWarStartTime > 0)
		{
			if (m_byBattleSiegeWarStartTime == (3 * MINUTE))
			{
				m_SiegeWarNoticeTime = 3;
				Announcement(IDS_SIEGE_WAR_READY_TIME_NOTICE);
			}
			else if (m_byBattleSiegeWarStartTime == (2 * MINUTE))
			{
				m_SiegeWarNoticeTime = 2;
				Announcement(IDS_SIEGE_WAR_READY_TIME_NOTICE);
			}
			else if (m_byBattleSiegeWarStartTime == (1 * MINUTE))
			{
				m_SiegeWarNoticeTime = 1;
				Announcement(IDS_SIEGE_WAR_READY_TIME_NOTICE);
			}
			else if (m_byBattleSiegeWarStartTime == 1)
			{
				Announcement(IDS_SIEGE_WAR_START);
				m_byBattleSiegeWarOpen = true;
			}
			m_byBattleSiegeWarStartTime--;
		}else if(m_sBattleTimeDelay == 30 * MINUTE)
		{
			m_SiegeWarNoticeTime = 30;
			Announcement(IDS_SIEGE_WAR_TIME_NOTICE);
		}else if(m_sBattleTimeDelay == 40 * MINUTE)
		{
			m_SiegeWarNoticeTime = 20;
			Announcement(IDS_SIEGE_WAR_TIME_NOTICE);
		}else if(m_sBattleTimeDelay == 50 * MINUTE)
		{
			m_SiegeWarNoticeTime = 10;
			Announcement(IDS_SIEGE_WAR_TIME_NOTICE);
		}else if(m_sBattleTimeDelay == 55 * MINUTE)
		{
			m_SiegeWarNoticeTime = 5;
			Announcement(IDS_SIEGE_WAR_TIME_NOTICE);
		}else if(m_sBattleTimeDelay == 57 * MINUTE)
		{
			m_SiegeWarNoticeTime = 3;
			Announcement(IDS_SIEGE_WAR_TIME_NOTICE);
		}else if(m_sBattleTimeDelay == 59 * MINUTE)
		{
			m_SiegeWarNoticeTime = 1;
			Announcement(IDS_SIEGE_WAR_TIME_NOTICE);
		}

		if( m_sBattleTimeDelay == ( 1 * HOUR ))
		{
			Announcement(IDS_SIEGE_WAR_END);
			m_sBattleTimeDelay = 0;
			m_byBattleOpen = NO_BATTLE;
			BattleZoneClose();
			m_byBattleSiegeWarOpen = false;
			Announcement(IDS_SIEGE_WAR_VICTORY);
		}
	}

	if (m_byBanishFlag)
	{
		m_sBanishDelay++;

		if (m_sBanishDelay == (m_byBattleTime / 360))
			Announcement(DECLARE_BAN);
		else if (m_sBanishDelay == (m_byBattleTime / 120)) {
			m_byBanishFlag = false;
			m_sBanishDelay = 0;
			BanishLosers();
		}
	}
}

uint8	CGameServerDlg::GetTariffByZone(int zoneid)
{

	_KNIGHTS_SIEGE_WARFARE *pSiegeWar = g_pMain->GetSiegeMasterKnightsPtr(1);
	CKingSystem *pKingSystemK = g_pMain->m_KingSystemArray.GetData(1);
	CKingSystem *pKingSystemH = g_pMain->m_KingSystemArray.GetData(2);
	if (pSiegeWar != nullptr 
		|| pKingSystemK != nullptr 
		|| pKingSystemH != nullptr)
	{
		switch (zoneid)
		{
		case ZONE_KARUS:
			if (pKingSystemK != nullptr)
				return pKingSystemK->m_nTerritoryTariff;
			else 
				return 0;
			break;
		case ZONE_KARUS_ESLANT:
			if (pKingSystemK != nullptr)
				return pKingSystemK->m_nTerritoryTariff;
			else 
				return 0;
			break;
		case ZONE_ELMORAD:
			if (pKingSystemH != nullptr)
				return pKingSystemH->m_nTerritoryTariff;
			else 
				return 0;
			break;
		case ZONE_ELMORAD_ESLANT:
			if (pKingSystemH != nullptr)
				return pKingSystemH->m_nTerritoryTariff;
			else 
				return 0;
			break;
		case ZONE_MORADON:
			if (pSiegeWar != nullptr)
				return (uint8)pSiegeWar->sMoradonTariff;
			else 
				return 0;
			break;
		case ZONE_DELOS:
			if (pSiegeWar != nullptr)
				return (uint8)pSiegeWar->sDellosTariff;
			else 
				return 0;
			break;
		default:
			break;
		}
	}
	return 0;
}

void CGameServerDlg::BattleZoneResult(uint8 nation)
{
	m_bVictory = nation;
	Announcement(DECLARE_WINNER, m_bVictory);
	Announcement(DECLARE_LOSER, nation == KARUS ? ELMORAD : KARUS);

	if (g_pMain->m_byBattleZoneType == ZONE_ARDREAM)
	{
		BattleZoneClose();
		return;
	}


	if (g_pMain->m_bResultDelayVictory == ELMORAD)
	{
		g_pMain->KickOutZoneUsers(ZONE_BATTLE,ZONE_KARUS,KARUS);
		g_pMain->KickOutZoneUsers(ZONE_BATTLE2,ZONE_KARUS,KARUS);
		g_pMain->KickOutZoneUsers(ZONE_BATTLE3,ZONE_KARUS,KARUS);
		g_pMain->KickOutZoneUsers(ZONE_BATTLE4,ZONE_KARUS,KARUS);
		g_DBAgent.karusbackt();
	}
	else if(g_pMain->m_bResultDelayVictory == KARUS)
	{
		g_pMain->KickOutZoneUsers(ZONE_BATTLE,ZONE_ELMORAD,ELMORAD);
		g_pMain->KickOutZoneUsers(ZONE_BATTLE2,ZONE_ELMORAD,ELMORAD);
		g_pMain->KickOutZoneUsers(ZONE_BATTLE3,ZONE_ELMORAD,ELMORAD);
		g_pMain->KickOutZoneUsers(ZONE_BATTLE4,ZONE_ELMORAD,ELMORAD);
		g_DBAgent.moradbackt();
	}

	m_byKarusOpenFlag = nation == ELMORAD ? true : false;
	m_byElmoradOpenFlag = nation == KARUS ? true : false;
	m_byBanishFlag = true;
	m_sBanishDelay = 0;
}

void CGameServerDlg::BattleWinnerResult(BattleWinnerTypes winnertype)
{
	uint8 winner_nation = 0;
	uint8 nBattleZone = m_byBattleZone + ZONE_BATTLE_BASE;

	CUser* pUser = g_pMain->GetUserPtr("Heros", TYPE_CHARACTER);

	if (winnertype == BATTLE_WINNER_NPC)
	{
		if (m_sKilledKarusNpc > m_sKilledElmoNpc)
			winner_nation = KARUS;
		else if (m_sKilledElmoNpc > m_sKilledKarusNpc)
			winner_nation = ELMORAD;

		if (winner_nation == 0 
			&& (nBattleZone == ZONE_BATTLE 
			|| nBattleZone == ZONE_BATTLE2
			|| nBattleZone == ZONE_BATTLE3))
		{
			BattleWinnerResult(BATTLE_WINNER_KILL);
			return;
		}
	}
	else if (winnertype == BATTLE_WINNER_MONUMENT)
	{
		if (m_sKarusMonumentPoint > m_sElmoMonumentPoint)
			winner_nation = KARUS;
		else if (m_sElmoMonumentPoint > m_sKarusMonumentPoint )
			winner_nation = ELMORAD;

		if (winner_nation == 0)
		{
			BattleWinnerResult(BATTLE_WINNER_KILL);
			return;
		}
	}
	else if (winnertype == BATTLE_WINNER_KILL)	
	{
		if (m_sKarusDead > m_sElmoradDead)
			winner_nation = ELMORAD;
		else if (m_sElmoradDead > m_sKarusDead)	
			winner_nation = KARUS;

		if (winner_nation == 0
			&& (nBattleZone == ZONE_BATTLE4 
			|| nBattleZone == ZONE_BATTLE6))
		{
			BattleWinnerResult(BATTLE_WINNER_NPC);
			return;
		}
	}

	if (winner_nation == 0) // Draw
	{
		TRACE("### WARNING : Battle is Draw Zone=%d, Karus Dead=%d, El Morad Dead=%d, Killed Karus NPC=%d, Killed Human NPC=%d, Karus Monument Point=%d, Karus Monument Point=%d ###\n",m_byBattleZone,m_sKarusDead,m_sElmoradDead,m_sKilledKarusNpc,m_sKilledElmoNpc,m_sKarusMonumentPoint,m_sKarusMonumentPoint);
		BattleZoneClose();
	}
	else
		BattleZoneResult(winner_nation);
}

void CGameServerDlg::BattleZoneOpen(int nType, uint8 bZone /*= 0*/)
{
	if ((nType == BATTLEZONE_OPEN || nType == SNOW_BATTLEZONE_OPEN) && !g_pMain->isWarOpen())
	{
		LunarGoldShells();
		m_byBattleOpen = nType == BATTLEZONE_OPEN ? NATION_BATTLE : SNOW_BATTLE;	
		m_byOldBattleOpen = nType == BATTLEZONE_OPEN ? NATION_BATTLE : SNOW_BATTLE;
		m_byBattleZone = bZone;
		m_byBattleOpenedTime = int32(UNIXTIME);		
		m_byBattleRemainingTime = m_byBattleTime;

		if (bZone + ZONE_BATTLE_BASE == ZONE_BATTLE4)
		{
			SendEventRemainingTime(true, nullptr, ZONE_BATTLE4);
		}
		KickOutZoneUsers(ZONE_ARDREAM);

		if (m_byBattleZoneType == 0)
		{
			KickOutZoneUsers(ZONE_RONARK_LAND_BASE);
			KickOutZoneUsers(ZONE_RONARK_LAND);
			KickOutZoneUsers(ZONE_BIFROST);
			KickOutZoneUsers(ZONE_KROWAZ_DOMINION);
		}
	}
	else if (nType == BATTLEZONE_CLOSE && isWarOpen()){
		Announcement(BATTLEZONE_CLOSE);
		DeleteLunarGoldShells();
	}else if (nType == SNOW_BATTLEZONE_CLOSE && isWarOpen()){
		Announcement(SNOW_BATTLEZONE_CLOSE);
	}else
		return;
	Announcement(nType);	// Send an announcement out that the battlezone is open/closed.
	Packet result(AG_BATTLE_EVENT, uint8(BATTLE_EVENT_OPEN));
	result << uint8(nType);
	Send_AIServer(&result);
}

void CGameServerDlg::CastleSiegeWarZoneOpen(int nType, uint8 bZone /*= 0*/)
{
	m_byBattleOpen = nType;	
	m_byOldBattleOpen = nType;
	m_byBattleZone = bZone;
	m_byBattleOpenedTime = int32(UNIXTIME);		
	m_byBattleRemainingTime = m_byBattleTime;
	m_byBattleSiegeWarStartTime = 10 * MINUTE;
	m_SiegeWarNoticeTime = 10;
	m_byBattleSiegeWarTeleport = true;
	m_byBattleSiegeWarOpen = false;
	bool Status = 1;
	Packet result(AG_CSW_OP_CL);
	result << Status;
	Send_AIServer(&result);
	KickOutZoneUsers(ZONE_DELOS, ZONE_MORADON);
	Announcement(IDS_SIEGE_WAR_READY_TIME_NOTICE);
}

void CGameServerDlg::BattleZoneClose()
{
	if (m_byBattleOpen == CLAN_BATTLE)
	{
		Announcement(IDS_SIEGE_WAR_END);
		bool Status = 0;
		Packet result(AG_CSW_OP_CL);
		result << Status;
		Send_AIServer(&result);
	}
	else
	{
		BattleZoneOpen(BATTLEZONE_CLOSE);
	}

	Packet result(AG_BATTLE_EVENT, uint8(BATTLE_EVENT_OPEN));
	result << uint8(BATTLEZONE_CLOSE);
	Send_AIServer(&result);
	ResetBattleZone();
	m_byBanishFlag = true;
}

void CGameServerDlg::BattleZoneVictoryCheck()
{	
	if (m_bKarusFlag >= NUM_FLAG_VICTORY)
		m_bVictory = KARUS;
	else if (m_bElmoradFlag >= NUM_FLAG_VICTORY)
		m_bVictory = ELMORAD;
	else 
		return;

	Announcement(DECLARE_WINNER);

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser* pTUser = TO_USER(itr->second);
		if (!pTUser->isInGame()
			|| pTUser->GetZoneID() != pTUser->GetNation() 
			|| pTUser->GetNation() != m_bVictory)
			continue;

		pTUser->GoldGain(AWARD_GOLD);
		pTUser->ExpChange(AWARD_EXP);

		if (pTUser->GetFame() == COMMAND_CAPTAIN)
		{
			if (pTUser->isKing())
				pTUser->SendLoyaltyChange(500);
			else
				pTUser->SendLoyaltyChange(300);
		}

		// Make the winning nation use a victory emotion (yay!)
		pTUser->StateChangeServerDirect(4, 12);
	}	
}

/**
* @brief	Kicks invaders out of the invaded nation after a war
*			and resets captains.
**/
void CGameServerDlg::BanishLosers()
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser *pUser = TO_USER(itr->second); 
		if (!pUser->isInGame())
			continue;

		// Reset captains
		if (!isWarOpen())
		{
			if (pUser->GetFame() == COMMAND_CAPTAIN)
				pUser->ChangeFame(CHIEF);
		}

		if (m_byBattleOpen == NATION_BATTLE)
		{
			// Kick out losers
			if (pUser->GetMap()->isWarZone() && m_bVictory != pUser->GetNation())
				pUser->KickOutZoneUser(true);
		}
		else if (!g_pMain->isWarOpen())
		{
			// Kick out invaders
			if ((pUser->GetZoneID() <= ELMORAD && pUser->GetZoneID() != pUser->GetNation())
				|| pUser->GetMap()->isWarZone())
				pUser->KickOutZoneUser(true);
		}
	}
}

void CGameServerDlg::ResetBattleZone()
{
	m_bVictory = 0;

	m_sBanishDelay = 0;
	m_byBanishFlag = false;

	m_sBattleResultDelay = 0;
	m_bResultDelay = false;

	m_bKarusFlag = 0;
	m_bElmoradFlag = 0;

	m_byKarusOpenFlag = false;
	m_byElmoradOpenFlag = false;

	m_byBattleSave = false;

	m_byBattleZone = 0;
	m_byBattleZoneType = 0;


	m_byBattleOpen = NO_BATTLE;
	m_byOldBattleOpen = NO_BATTLE;

	m_byBattleOpenedTime = 0;
	m_byBattleRemainingTime = 0;
	m_sBattleTimeDelay = 0;

	m_byBattleSiegeWarStartTime = 0;
	m_SiegeWarNoticeTime = 0;

	//visualdev notice
	m_WarNoticeTime = 0;
	m_SnowWarNoticeTime=0;
	//end

	m_byBattleSiegeWarOpen = false;
	m_byBattleSiegeWarMomument = false;
	m_byBattleSiegeWarTeleport = false;

	m_sKarusDead = 0;
	m_sElmoradDead = 0;

	m_sKarusCount = 0;
	m_sElmoradCount = 0;

	m_sKilledKarusNpc = 0;
	m_sKilledElmoNpc = 0;

	m_sKarusMonumentPoint = 0;
	m_sElmoMonumentPoint = 0;
	m_sKarusMonuments = 3;
	m_sElmoMonuments = 3;
	///war 64
	for(int i = 0; i < 7; i++)
		m_sNereidsMonumentNation[i] = 0;

	foreach_stlmap(itr,m_arNpcArray)
	{
		if (itr->second == nullptr)
			continue;

		if(itr->second->GetZoneID() != ZONE_BATTLE4)
			continue;


		if (itr->second->m_tNpcType == NPC_BATTLE_MONUMENT)
		{
			itr->second->HpChange(-30000,nullptr,true);
			itr->second->HpChange(-30000,nullptr,true);
			itr->second->HpChange(-30000,nullptr,true);
			itr->second->HpChange(-30000,nullptr,true);
			itr->second->HpChange(-30000,nullptr,true);
			itr->second->HpChange(-30000,nullptr,true);
			g_pMain->NpcUpdate(itr->second->GetProtoID(), itr->second->m_bMonster, 4, 14005);

		}
	}
	//end
	m_NationMonumentInformationArray.DeleteAllData();
	m_bMiddleStatueNation = 0;
}

void CGameServerDlg::ForgettenTempleEventTimer()
{
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;
	uint32 nSecond = g_localTime.tm_sec;

	if (!m_bForgettenTempleIsActive)
	{
		foreach_stlmap_nolock (itr, m_MonsterChallengeArray)
		{	
			if ((nHour == itr->second->bStartTime1 - 1
				|| nHour == itr->second->bStartTime2 - 1
				|| nHour == itr->second->bStartTime3 - 1) && nMinute == 55 && nSecond == 0)
			{
				if (nHour + 1 == itr->second->bStartTime1)
					m_nForgettenTempleStartHour = itr->second->bStartTime1;
				else if (nHour + 1 == itr->second->bStartTime2)
					m_nForgettenTempleStartHour = itr->second->bStartTime2;
				else if (nHour + 1 == itr->second->bStartTime3)
					m_nForgettenTempleStartHour = itr->second->bStartTime3;

				m_nForgettenTempleLevelMin = itr->second->bLevelMin;
				m_nForgettenTempleLevelMax = itr->second->bLevelMax;

				Announcement(IDS_MONSTER_CHALLENGE_ANNOUNCEMENT);
			}
			else if ((nHour == itr->second->bStartTime1 - 1
				|| nHour == itr->second->bStartTime2 - 1
				|| nHour == itr->second->bStartTime3 - 1) && nMinute == 59 && nSecond == 40)
			{
				m_nForgettenTempleLevelMin = itr->second->bLevelMin;
				m_nForgettenTempleLevelMax = itr->second->bLevelMax;
				Announcement(IDS_MONSTER_CHALLENGE_ANNOUNCEMENT);
			}
			else if ((nHour == itr->second->bStartTime1
				|| nHour == itr->second->bStartTime2
				|| nHour == itr->second->bStartTime3) && nMinute == 0)
			{
				if (itr->second->bLevelMin == 30)	
					m_nForgettenTempleChallengeTime = 1;
				else if (itr->second->bLevelMin == 46)
					m_nForgettenTempleChallengeTime = 2;
				else if (itr->second->bLevelMin == 60)
					m_nForgettenTempleChallengeTime = 3;
			}
		}

		if (m_nForgettenTempleChallengeTime > 0)
		{
			m_bForgettenTempleIsActive = true;
			m_nForgettenTempleStartTime = int32(UNIXTIME);

			foreach_stlmap_nolock (itr, m_MonsterChallengeSummonListArray)
				if (itr->second->bLevel == m_nForgettenTempleChallengeTime)
					m_nForgettenTempleLastStage++;

			if(m_nForgettenTempleLastStage>56)
				m_nForgettenTempleLastStage=56;

			Announcement(IDS_MONSTER_CHALLENGE_OPEN);
		}
	}
	else if (m_bForgettenTempleIsActive)
	{
		if (int32(UNIXTIME) - m_nForgettenTempleStartTime == 300 /* 5 minutes */
			&& !m_bForgettenTempleSummonMonsters)
		{
			m_bForgettenTempleSummonMonsters = true;
			m_nForgettenTempleCurrentStage = 1;
			m_nForgettenTempleLastSummonTime = int32(UNIXTIME) + 30;
			Announcement(IDS_MONSTER_CHALLENGE_START);
		}
		else if (m_bForgettenTempleSummonMonsters)
		{
			foreach_stlmap_nolock (itr, m_MonsterChallengeSummonListArray)
			{
				if (m_nForgettenTempleChallengeTime != itr->second->bLevel
					|| int32(UNIXTIME) - m_nForgettenTempleLastSummonTime < 45
					|| itr->second->bStage != m_nForgettenTempleCurrentStage
					&& m_nForgettenTempleCurrentStage != 57)
					continue;

				if (m_nForgettenTempleCurrentStage > m_nForgettenTempleLastStage)
					m_bForgettenTempleSummonMonsters = false;
				else
				{
					SpawnEventNpc(itr->second->sSid,true,ZONE_FORGOTTEN_TEMPLE,(float)itr->second->sPosX,0,(float)itr->second->sPosZ,itr->second->sCount,itr->second->bRange);
					m_nForgettenTempleCurrentStage++;
					m_nForgettenTempleLastSummonTime = int32(UNIXTIME);
				}
			}
		}
		else
		{
			if (m_nForgettenTempleCurrentStage > m_nForgettenTempleLastStage && !m_nForgettenTempleBanishFlag) 
			{
				if (m_ForgettenTempleMonsterList.size() == 0)
				{
					m_nForgettenTempleBanishFlag = true;
					m_nForgettenTempleBanishTime = 200;
					Announcement(IDS_MONSTER_CHALLENGE_VICTORY);
					switch(m_nForgettenTempleChallengeTime)
					{
					case 1:
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, RED_TREASURE_CHEST);
						break;
					case 2:
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, GREEN_TREASURE_CHEST);
						break;
					case 3:
						SendItemZoneUsers(ZONE_FORGOTTEN_TEMPLE, BLUE_TREASURE_CHEST);
						break;
					}
				}
				else if (int32(UNIXTIME) - m_nForgettenTempleStartTime >= (HOUR - (10 * MINUTE)))
				{
					m_nForgettenTempleBanishFlag = true;
					m_nForgettenTempleBanishTime = 0;
				}
			}
		}

		if (m_nForgettenTempleBanishFlag)
		{
			m_nForgettenTempleBanishTime++;

			if (m_nForgettenTempleBanishTime == 30 /* 30 seconds */)
				Announcement(IDS_MONSTER_CHALLENGE_CLOSE);
			else if (m_nForgettenTempleBanishTime == 230 /* 5 minutes */)
			{
				m_nForgettenTempleBanishFlag = false;
				m_nForgettenTempleBanishTime = 0;

				m_nForgettenTempleUsers.clear();
				m_bForgettenTempleIsActive = false;
				m_nForgettenTempleStartHour = 0;
				m_nForgettenTempleLevelMin = 0;
				m_nForgettenTempleLevelMax = 0;
				m_nForgettenTempleStartTime = 0;
				m_nForgettenTempleChallengeTime = 0;
				m_bForgettenTempleSummonMonsters = false;
				m_nForgettenTempleCurrentStage = 0;
				m_nForgettenTempleLastStage = 0;

				foreach (itr, m_ForgettenTempleMonsterList)
					KillNpc(itr->first);

				m_ForgettenTempleMonsterList.clear();
				m_nForgettenTempleLastSummonTime = 0;

				KickOutZoneUsers(ZONE_FORGOTTEN_TEMPLE,ZONE_MORADON);
			}
		}
	}
}
void CGameServerDlg::BowlEventTimer()
{
	if (CZBowlAktifMi == false)
		return;

	uint8 importantPrize = 0;
	uint8 totalPrize = 0;
	if ((CzBowlItemCount > 0) && (CzBowlItem > 99999999) && (CzBowlItemTimeSec > 0))
		totalPrize++;

	if ((CzBowlKC > 0) && (CzBowlKCTimeSec > 0))
		totalPrize++;

	if ((CzBowlNP > 0)
		&& (CzBowlNpTimeSec > 0))
		totalPrize++;


	if (CzBowlNpTimeSec > CzBowlKCTimeSec && CzBowlNpTimeSec > CzBowlItemTimeSec)
		importantPrize = 3;
	else if(CzBowlKCTimeSec > CzBowlNpTimeSec && CzBowlKCTimeSec > CzBowlItemTimeSec)
		importantPrize = 2;
	else if(CzBowlItemTimeSec > CzBowlKCTimeSec	&& CzBowlItemTimeSec > CzBowlNpTimeSec)
		importantPrize = 1;
	else
		importantPrize = 0;

	if (importantPrize == 0	&& CzBowlReloadTimeSec == 0)
	{
		printf("CZ Bowl event hatali ayar, bowl event kapatiliyor...\n");
		CZBowlAktifMi = false;
	}else if(importantPrize == 0 && CzBowlReloadTimeSec > 0)
	{
		printf("CZ Bowl event hatali ayar[2], bowl event kapatiliyor...\n");
		CZBowlAktifMi = false;
	}
	// importantPrize 3
	// totalPrize 2
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == nullptr)
			continue;

		if (!pUser->isInGame())
			continue;

		if (pUser->GetZoneID() != ZONE_RONARK_LAND)
			continue;

		if (pUser->isDead())
			continue;

		if (pUser->inBowl && pUser->isInBowl(pUser->GetSPosX(),pUser->GetSPosZ()))
		{

			if (((UNIXTIME - pUser->BowlRewardTime) > CzBowlItemTimeSec)
				&& (CzBowlItemCount > 0)
				&& (CzBowlItem > 99999999)
				&& (CzBowlItemTimeSec > 0)
				&& (pUser->lastPrize == 0 || pUser->lastPrize == 2 || pUser->lastPrize == 3))
			{
				pUser->GiveItem(CzBowlItem,CzBowlItemCount);
				pUser->lastPrize = 1;
			}

			if (((UNIXTIME - pUser->BowlRewardTime) > CzBowlKCTimeSec)
				&& (CzBowlKC > 0)
				&& (CzBowlKCTimeSec > 0)
				&& (pUser->lastPrize == 0 || pUser->lastPrize == 1 || pUser->lastPrize == 3))
			{
				g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(),CzBowlKC);
				SendHelpDescription(pUser,string_format("%d Knight Cash earned.",CzBowlKC));
				pUser->lastPrize = 2;
			}

			if (((UNIXTIME - pUser->BowlRewardTime) > CzBowlNpTimeSec)
				&& (CzBowlNP > 0)
				&& (CzBowlNpTimeSec > 0)
				&& (pUser->lastPrize == 0 || pUser->lastPrize == 1 || pUser->lastPrize == 2))
			{
				pUser->SendLoyaltyChange(CzBowlNP,false,false,false);
				pUser->lastPrize = 3;
			}

			if (pUser->lastPrize == importantPrize)
			{
				pUser->lastPrize = 0;
				pUser->BowlRewardTime = UNIXTIME;
			}


		}


	}
}
void CGameServerDlg::TempleEventTimer()
{
	uint32 nHour = g_localTime.tm_hour;
	uint32 nMinute = g_localTime.tm_min;
	uint32 nSeconds = g_localTime.tm_sec;
	if (m_nTempleEventRemainSeconds > 0)
		m_nTempleEventRemainSeconds--;

	if (m_nTempleEventRemainSeconds%60==0 
		&& pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR 
		&& !pTempleEvent.isActive)
	{
		Packet resultmer;
		std::string bufferpro = string_format("[Event Message] Border Defance War Human: %d, Karus: %d, Remaining time: %d sec",g_pMain->pTempleEvent.ElMoradUserCount,g_pMain->pTempleEvent.KarusUserCount,g_pMain->m_nTempleEventRemainSeconds); 
		ChatPacket::Construct(&resultmer, 7, &bufferpro);
		g_pMain->Send_All(&resultmer);

	}

	if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR){

		for (int a = 0; a < pTempleEvent.LastEventRoom; a++){
			if (UNIXTIME > pTempleEvent.OurRoomTeleportTime[a+1] && pTempleEvent.waiting4TP[a+1] && !pTempleEvent.OurRoomIsTeleported[a+1] && pTempleEvent.OurRoomTeleportTime[a+1] > (UNIXTIME-10) && !pTempleEvent.isAttackable[a+1])
				TempleEventKickOutUser(a+1);
		}

	}


	if (pTempleEvent.ActiveEvent == -1)
	{
		for (int i = 0; i < BORDER_DEFENSE_WAR_EVENT_COUNT; i++)
		{
			if (nHour == m_nBorderDefenseWarTime[i] && nMinute == 0) 
			{
				pTempleEvent.ActiveEvent = TEMPLE_EVENT_BORDER_DEFENCE_WAR;
				pTempleEvent.ZoneID = ZONE_BORDER_DEFENSE_WAR;

				pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1 = m_nBorderDefenseWarPrizeWonItemNo1[i];
				pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2 = m_nBorderDefenseWarPrizeWonItemNo2[i];
				pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3 = m_nBorderDefenseWarPrizeWonItemNo3[i];
				pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_K = m_nBorderDefenseWarPrizeWonItemNo4_K[i];
				pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_H = m_nBorderDefenseWarPrizeWonItemNo4_H[i];
				pTempleEvent.m_nBorderDefenseWarPrizeWonLoyalty = m_nBorderDefenseWarPrizeWonLoyalty[i];
				pTempleEvent.m_nBorderDefenseWarPrizeWonExp = m_nBorderDefenseWarPrizeWonExp[i];
				pTempleEvent.m_nBorderDefenseWarPrizeLoserLoyalty = m_nBorderDefenseWarPrizeLoserLoyalty[i];
				pTempleEvent.m_nBorderDefenseWarPrizeLoserItem = m_nBorderDefenseWarPrizeLoserItem[i];
				pTempleEvent.m_nBorderDefenseWarPrizeLoserExp = m_nBorderDefenseWarPrizeLoserExp[i];
				pTempleEvent.m_nBorderDefenseWarMAXLEVEL = m_nBorderDefenseWarMAXLEVEL[i];
				pTempleEvent.m_nBorderDefenseWarMINLEVEL = m_nBorderDefenseWarMINLEVEL[i];

				m_nTempleEventRemainSeconds = 600; // 10 minutes
				TempleEventStart();
				break;
			} 
		}
	}
	else if (pTempleEvent.ActiveEvent != -1)
	{
		if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR)
		{
			for(int i = 0; i < BORDER_DEFENSE_WAR_EVENT_COUNT; i++)
			{
				if (nHour == m_nBorderDefenseWarTime[i] && nMinute == 10 && !pTempleEvent.isActive)
				{
					m_nTempleEventRemainSeconds = 901; 
					pTempleEvent.LastEventRoom = 1;
					pTempleEvent.isActive = true;
					TempleEventStart();
					TempleEventTeleportUsers();
					TempleEventSummon();
					for (int sa = 0; sa < pTempleEvent.LastEventRoom; sa++)
						pTempleEvent.isAttackable[sa+1] = true;
					break;
				}
				else if (nHour == m_nBorderDefenseWarTime[i] && pTempleEvent.isActive &&  m_nTempleEventRemainSeconds < 1791 && !pTempleEvent.isAttackable[pTempleEvent.LastEventRoom])   
				{
					for (int cab = 0; cab < pTempleEvent.LastEventRoom; cab++){
						if(!pTempleEvent.waiting4TP[cab+1])
							pTempleEvent.isAttackable[cab+1] = true;
					}
					break;
				}
				else if (nHour == m_nBorderDefenseWarTime[i] && nMinute == 40 && pTempleEvent.isAttackable)   
				{
					for (int b = 0; b < pTempleEvent.LastEventRoom; b++){
						TerminationFinish(b+1);
						pTempleEvent.isAttackable[b+1] = false;
						pTempleEvent.OurRoomTeleportTime[b+1] = UNIXTIME + 18;
						pTempleEvent.OurRoomIsTeleported[b+1] = false;
						pTempleEvent.waiting4TP[b+1] = true;
					}
					break;
				}
				else if (nHour == m_nBorderDefenseWarTime[i] && nMinute == 40 && nSeconds == 20 && pTempleEvent.isActive) 
				{
					pTempleEvent.isActive = false;
					TempleEventFinish(0);
					break;
				}
			}
		}
	}
}
void CGameServerDlg::TempleEventStart()
{
	pTempleEvent.StartTime = (uint32)UNIXTIME;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;	
	pTempleEvent.AllUserCount = 0;
	pTempleEvent.m_sElmoBdw = 0;
	pTempleEvent.m_sKarusBdw = 0;

	if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR 
		&& !pTempleEvent.isActive){
			Packet resultmer;
			std::string bufferpro = string_format("[Event Message] (%d~%d) levels can register for Border Defance War with typing '+bdw' to chat.",pTempleEvent.m_nBorderDefenseWarMINLEVEL,pTempleEvent.m_nBorderDefenseWarMAXLEVEL); 
			ChatPacket::Construct(&resultmer, 7, &bufferpro);
			Send_All(&resultmer);
			g_pMain->SendAnnouncement(bufferpro.c_str());
	}
	else if(pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR 
		&& pTempleEvent.isActive)
	{
		Packet resultmer2;
		std::string bufferpro2 = string_format("[Event Message] Border Defance War has started. Type '+bdw' to chat for information."); 
		ChatPacket::Construct(&resultmer2, 7, &bufferpro2);
		Send_All(&resultmer2);
		g_pMain->SendAnnouncement(bufferpro2.c_str());

	}

}

void CGameServerDlg::TempleEventTeleportUsers()
{
	foreach_stlmap (itr, m_TempleEventUserArray)
	{
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser == nullptr 
			|| !pUser->isInGame())
			continue;

		if (pTempleEvent.ZoneID == ZONE_BORDER_DEFENSE_WAR)
		{
			pUser->ZoneChange(pTempleEvent.ZoneID,0.0f,0.0f);
		}
	}
}

void CGameServerDlg::TerminationFinish(uint16 nEventRoom)
{
	if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR) 
	{
		Packet bresult(WIZ_EVENT);
		bresult << uint8(10)
			<< uint8(2)  << uint8(0)  
			<< uint8(0)  << uint8(20) << uint32(0);

		Packet cresult(WIZ_EVENT);
		cresult << uint8(10)
			<< uint8(2)  << uint8(0)  
			<< uint8(1)  << uint8(20) << uint32(0);	

		Packet dresult(WIZ_EVENT);
		dresult << uint8(10)
			<< uint8(1)  << uint8(0)  
			<< uint8(2)  << uint8(20) << uint32(0);	

		foreach_stlmap(itr, m_TempleEventUserArray) 
		{
			CUser * pUser = GetUserPtr(itr->second->m_socketID);
			if (pUser == nullptr ||	!pUser->isInGame())
				continue;

			if (nEventRoom > 0 && pUser->GetEventRoom() != nEventRoom)
				continue;

			if (g_pMain->pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] == pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] && g_pMain->pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] == pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()])
				g_pMain->Send_Zone(&bresult,  ZONE_BORDER_DEFENSE_WAR, pUser, Nation::ALL, pUser->GetEventRoom());

			if (pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] > pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] && pUser->GetNation() == KARUS) 
				g_pMain->Send_Zone(&cresult,  ZONE_BORDER_DEFENSE_WAR, pUser, Nation::ALL, pUser->GetEventRoom());

			if (pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] > pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] && pUser->GetNation() == ELMORAD)		
				g_pMain->Send_Zone(&dresult,  ZONE_BORDER_DEFENSE_WAR, pUser, Nation::ALL, pUser->GetEventRoom());

		}
	}
}
void CGameServerDlg::TerminitionTime(uint8 sNewZone,uint16 Time)
{
	if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_JURAD_MOUNTAIN)
	{
		Packet Inventory(WIZ_EVENT);
		Inventory << uint8(1); // Inventory Kontrol.
		Packet aresult(WIZ_SELECT_MSG);
		aresult << uint16(1)   << uint8(7) 
			<< uint32(0)   << uint32(0)
			<< uint8(7)    << uint16(2) 
			<< uint8(4)    << uint8(0x07)
			<< Time << uint16(3);

		Packet bresult(WIZ_BIFROST);
		bresult << uint8(0x05) << Time;

		Packet cresult(WIZ_EVENT);
		cresult << uint8(0x03) << uint32(35)	  << uint32(35) << uint32(0x05008504) << uint8(0);

		foreach_stlmap(itr, m_TempleEventUserArray) 
		{
			CUser * pUser = GetUserPtr(itr->second->m_socketID);
			if (pUser == nullptr ||	!pUser->isInGame())
				continue;

			cresult << pUser->GetName();
		}
		cresult << uint32(0);

		foreach_stlmap_nolock(itr, m_TempleEventUserArray) 
		{
			CUser * pUser = GetUserPtr(itr->second->m_socketID);
			if (pUser == nullptr ||	!pUser->isInGame())
				continue;

			pUser->Send(&Inventory);
			pUser->Send(&aresult);
			pUser->Send(&bresult);
			pUser->SendToRegion(&cresult, nullptr, pUser->GetEventRoom());
		}
	}
	else if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR)
	{
		Packet Inventory(WIZ_EVENT);
		Inventory << uint8(1); // Inventory Kontrol.

		Packet aresult(WIZ_SELECT_MSG);
		aresult << uint16(0)   << uint8(7) 
			<< uint32(0)   << uint32(0)
			<< uint8(8)    << uint16(0) 
			<< uint8(0)    << uint8(0x07)
			<< Time << uint16(0);

		Packet bresult(WIZ_BIFROST);
		bresult << uint8(0x05) << Time;

		Packet cresult(WIZ_EVENT);
		cresult << uint8(0x03) << uint32(0)	  << uint32(0) << uint32(0x05008504) << uint8(0);

		foreach_stlmap(itr, m_TempleEventUserArray) 
		{
			CUser * pUser = GetUserPtr(itr->second->m_socketID);

			if (pUser == nullptr ||	!pUser->isInGame())
				continue;

			cresult << pUser->GetName();
		}
		cresult << uint32(0);

		foreach_stlmap_nolock(itr, m_TempleEventUserArray) 
		{
			CUser * pUser = GetUserPtr(itr->second->m_socketID);

			if (pUser == nullptr ||	!pUser->isInGame())
				continue;

			pUser->Send(&Inventory);
			pUser->Send(&aresult);
			pUser->Send(&bresult);
			pUser->SendToRegion(&cresult,nullptr,pUser->GetEventRoom());
		}
	}
	else if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_CHAOS)
	{
		Packet aresult(WIZ_SELECT_MSG);
		aresult << uint16(0)   << uint8(7)    
			<< uint32(0)   << uint32(0) 
			<< uint8(9)	   << uint16(0) 
			<< uint8(0)    << uint8(0x18)
			<< Time << uint16(0);

		Packet bresult(WIZ_BIFROST);
		bresult << uint8(0x05) << Time;

		foreach_stlmap (itr, m_TempleEventUserArray) 
		{
			CUser * pUser = GetUserPtr(itr->second->m_socketID);

			if (pUser == nullptr ||	!pUser->isInGame())
				continue;

			pUser->Send(&aresult);
			pUser->Send(&bresult);
		}
	}
}

uint8 CGameServerDlg::TempleEventGetRoomUsers(uint16 nEventRoom, uint8 nNation) 
{
	uint8 nEventRoomUserCount = 0;

	foreach_stlmap (itr, m_TempleEventUserArray)
	{
		if (itr->second->m_bEventRoom != nEventRoom)
			continue;

		if (itr->second->m_bIsFinished == true)
			continue;

		CUser *pUser = GetUserPtr(itr->first);

		if (!pUser && pUser == nullptr)
			continue;

		if (pUser->isInGame() && pUser->GetEventRoom() == nEventRoom){
			if(nNation == 0){
				nEventRoomUserCount++;
				continue;
			}
			if(nNation != pUser->GetNation())
				continue;

			if (nNation == pUser->GetNation())
				nEventRoomUserCount++;

		}
	}

	return nEventRoomUserCount;
}

void CGameServerDlg::TempleEventSummonChaosCubes()
{
	for (int i = 0; i < g_pMain->pTempleEvent.LastEventRoom; i++)
	{
		foreach_stlmap_nolock (itr, m_MonsterRespawnListInformationArray)
		{
			if (itr->second->ZoneID == ZONE_CHAOS_DUNGEON)
				SpawnEventNpc(itr->second->sSid,false,itr->second->ZoneID,itr->second->X,itr->second->Y,itr->second->Z,itr->second->sCount,itr->second->bRadius,0,0,-1,i+1/* EventRoom */);
		}
	}
}

void CGameServerDlg::TempleEventSummon()
{
	if (pTempleEvent.ActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR)
	{
		for (int i = 0; i < g_pMain->pTempleEvent.LastEventRoom; i++)
		{
			foreach_stlmap_nolock (itr, m_MonsterRespawnListInformationArray)
			{
				if (itr->second->ZoneID == ZONE_BORDER_DEFENSE_WAR)
				{
					/*if (itr->second->sSid != 4061)*////visualdev
					SpawnEventNpc(itr->second->sSid,false,itr->second->ZoneID,itr->second->X,itr->second->Y,itr->second->Z,itr->second->sCount,itr->second->bRadius,60*MINUTE,0,-1,i+1);
					/*else*///visualdev
					SpawnEventNpc(itr->second->sSid,true,itr->second->ZoneID,itr->second->X,itr->second->Y,itr->second->Z,itr->second->sCount,itr->second->bRadius,60*MINUTE,0,-1,i+1);
				}
			}
		}
	}
}

void CGameServerDlg::TempleEventFinish(uint16 nEventRoom)
{
	if(nEventRoom > MAX_TEMPLE_EVENT_ROOM)
		return;

	if (nEventRoom > 0){
		pTempleEvent.OurRoomTeleportTime[nEventRoom] = UNIXTIME + 20;
		pTempleEvent.OurRoomIsTeleported[nEventRoom] = false;
		pTempleEvent.isAttackable[nEventRoom] = false;
		TerminationFinish(nEventRoom);
		pTempleEvent.waiting4TP[nEventRoom] = true;
	}


	foreach_stlmap_nolock(itr, m_TempleEventUserArray)
	{
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser == nullptr || !pUser->isInGame())
			continue;

		if (itr->second->m_bIsFinished == true)
			continue;

		if (pUser->GetEventRoom() != nEventRoom && nEventRoom != 0)
			continue;

		if (pUser->GetZoneID() != ZONE_BORDER_DEFENSE_WAR)
			continue;



		int64 nChangeExp;
		int32 nChangeLoyalty;
		if (pUser->GetZoneID() == ZONE_BORDER_DEFENSE_WAR)
		{
			if (pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] == pTempleEvent.KarusDeathCount[pUser->GetEventRoom()])
			{ // Draw
				nChangeExp = pTempleEvent.m_nBorderDefenseWarPrizeLoserExp;
				nChangeLoyalty = pTempleEvent.m_nBorderDefenseWarPrizeLoserLoyalty;

				if(pTempleEvent.m_nBorderDefenseWarPrizeLoserItem > 0)
					pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeLoserItem,1);

				if(nChangeLoyalty > 0)
					pUser->SendLoyaltyChange(nChangeLoyalty);

				if(nChangeExp > 0)
					pUser->ExpChange(nChangeExp);

			}
			else if(pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] > pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] && pUser->GetNation() == KARUS) 
			{ // Karus Win

				nChangeExp = pTempleEvent.m_nBorderDefenseWarPrizeWonExp;
				nChangeLoyalty = pTempleEvent.m_nBorderDefenseWarPrizeWonLoyalty;

				if(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1 > 0)
					pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1,1);

				if(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2 > 0)
					pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2,1);

				if(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3 > 0)
					pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3,1);

				if(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_K > 0)
					pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_K,1);

				if(nChangeLoyalty > 0)
					pUser->SendLoyaltyChange(nChangeLoyalty);

				if(nChangeExp > 0)
					pUser->ExpChange(nChangeExp);


			}
			else if (pTempleEvent.KarusDeathCount[pUser->GetEventRoom()] > pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] && pUser->GetNation() == ELMORAD) 
			{ // Elmorad Win

				nChangeExp = pTempleEvent.m_nBorderDefenseWarPrizeWonExp;
				nChangeLoyalty = pTempleEvent.m_nBorderDefenseWarPrizeWonLoyalty;

				if(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1 > 0)
					pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1,1);

				if(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2 > 0)
					pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2,1);

				if(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3 > 0)
					pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3,1);

				if(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_H > 0)
					pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_H,1);

				if(nChangeLoyalty > 0)
					pUser->SendLoyaltyChange(nChangeLoyalty);

				if(nChangeExp > 0)
					pUser->ExpChange(nChangeExp);

			}
			else
			{ // I am loser

				nChangeExp = pTempleEvent.m_nBorderDefenseWarPrizeLoserExp;
				nChangeLoyalty = pTempleEvent.m_nBorderDefenseWarPrizeLoserLoyalty;

				if(pTempleEvent.m_nBorderDefenseWarPrizeLoserItem > 0)
					pUser->GiveItem(pTempleEvent.m_nBorderDefenseWarPrizeLoserItem,1);

				if(nChangeLoyalty > 0)
					pUser->SendLoyaltyChange(nChangeLoyalty);

				if(nChangeExp > 0)
					pUser->ExpChange(nChangeExp);

			}

		}


		Packet resultmer;
		std::string bufferpro; 

		if (pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] > pTempleEvent.KarusDeathCount[pUser->GetEventRoom()])
			bufferpro = string_format("[Event Message] Border Defance War finished. Karus nation has won. You will teleport in 20 seconds.",g_pMain->pTempleEvent.ElMoradUserCount,g_pMain->pTempleEvent.KarusUserCount,g_pMain->m_nTempleEventRemainSeconds);
		else if (pTempleEvent.ElmoDeathCount[pUser->GetEventRoom()] < pTempleEvent.KarusDeathCount[pUser->GetEventRoom()])
			bufferpro = string_format("[Event Message] Border Defance War finished. Human nation has won. You will teleport in 20 seconds.",g_pMain->pTempleEvent.ElMoradUserCount,g_pMain->pTempleEvent.KarusUserCount,g_pMain->m_nTempleEventRemainSeconds);
		else
			bufferpro = string_format("[Event Message] Border Defance War finished. Score is draw. You will teleport in 20 seconds.",g_pMain->pTempleEvent.ElMoradUserCount,g_pMain->pTempleEvent.KarusUserCount,g_pMain->m_nTempleEventRemainSeconds);

		ChatPacket::Construct(&resultmer, 7, &bufferpro);

		pUser->Send(&resultmer);
	}

	foreach_stlmap_nolock (itr, m_TempleEventUserArray)
	{
		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser && pUser->isInGame())
		{
			TemploVotarUser(pUser);
			pUser->m_bEventRoom = 0;
		}
			
	}


	if (nEventRoom == 0)
		TempleEventReset();
}

void CGameServerDlg::TempleEventGetActiveEventTime(CUser *pUser)
{
	if (pUser == nullptr)
		return;

	if (!pUser->isEventUser())
		return;

	if(pTempleEvent.ActiveEvent != TEMPLE_EVENT_BORDER_DEFENCE_WAR)
		return;

	Packet resultmer;
	std::string bufferpro = string_format("[Event Message] Border Defance War Human: %d, Karus: %d, Remaining time: %d sec",g_pMain->pTempleEvent.ElMoradUserCount,g_pMain->pTempleEvent.KarusUserCount,g_pMain->m_nTempleEventRemainSeconds); 
	ChatPacket::Construct(&resultmer, 7, &bufferpro);
	pUser->Send(&resultmer);

}

void CGameServerDlg::TemploVotarUser(CUser *pUser)
{
	if (pUser == nullptr
		|| !pUser->isInGame())
		return;

	uint8 nZoneID = 0;

	if (pUser->GetZoneID() == ZONE_BORDER_DEFENSE_WAR)
		nZoneID = pUser->GetNation();

	if (nZoneID == 0)
		return;

	_START_POSITION * pStartPosition = GetStartPosition(nZoneID);

	if (pStartPosition == nullptr)
	{
		TRACE("### TemploVotarUser - StartPosition not found : Zone ID=%d",nZoneID);
		return;
	}

	pUser->ZoneChange(nZoneID, 0.0f,0.0f);

}


void CGameServerDlg::TempleEventKickOutUser(uint16 nEventRoom)
{
	uint8 nZoneID = 0;
	_START_POSITION * pStartPosition;
	foreach_stlmap_nolock(itr, m_TempleEventUserArray)
	{

		CUser * pUser = GetUserPtr(itr->second->m_socketID);

		if (pUser == nullptr || !pUser->isInGame())
			continue;

		if (itr->second->m_bIsFinished)
			continue;

		if (pUser->GetEventRoom() != nEventRoom )
			continue;



		if (pUser->GetZoneID() == ZONE_BORDER_DEFENSE_WAR)
			nZoneID = pUser->GetNation();

		if (nZoneID == 0)
			return;

		pStartPosition = GetStartPosition(nZoneID);

		if (pStartPosition == nullptr)
		{
			TRACE("### TempleEventKickOutUser - StartPosition not found : Zone ID=%d",nZoneID);
			return;
		}

		_TEMPLE_EVENT_USER * pEventUser = itr->second;

		if (pEventUser != nullptr)
			pEventUser->m_bIsFinished = true;

		pUser->ZoneChange(nZoneID, 0.0f,0.0f);
		pUser->SetUnitEventRoom(0);
		pUser->SetUserEventRoom(0);

	}

	pTempleEvent.OurRoomIsTeleported[nEventRoom]=true;
	pTempleEvent.waiting4TP[nEventRoom] = false;
}

void CGameServerDlg::TempleEventReset()
{
	pTempleEvent.ActiveEvent = -1;
	pTempleEvent.ZoneID = 0;
	pTempleEvent.LastEventRoom = 1;
	pTempleEvent.StartTime = 0;
	pTempleEvent.AllUserCount = 0;
	pTempleEvent.KarusUserCount = 0;
	pTempleEvent.ElMoradUserCount = 0;
	pTempleEvent.KarusKillCount = 0;
	pTempleEvent.ElMoradKillCount = 0;
	pTempleEvent.m_sKarusBdw = 0;
	pTempleEvent.m_sElmoBdw = 0;
	for (int b = 0; b < MAX_TEMPLE_EVENT_ROOM; b++){
		pTempleEvent.isAttackable[b+1] = false;
		pTempleEvent.waiting4TP[b+1] = false;
	}
	memset(pTempleEvent.OurRoomTeleportTime , 0 , sizeof(pTempleEvent.OurRoomTeleportTime));
	for (int ab = 0; ab < MAX_TEMPLE_EVENT_ROOM; ab++)
		pTempleEvent.OurRoomIsTeleported[ab+1] = false;
	pTempleEvent.isActive = false;	
	memset(pTempleEvent.KarusDeathCount , 0 , sizeof(pTempleEvent.KarusDeathCount));
	memset(pTempleEvent.ElmoDeathCount , 0 , sizeof(pTempleEvent.ElmoDeathCount));
	m_TempleEventUserArray.DeleteAllData();

	pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo1 = 0;
	pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo2 = 0;
	pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo3 = 0;
	pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_K = 0;
	pTempleEvent.m_nBorderDefenseWarPrizeWonItemNo4_H = 0;
	pTempleEvent.m_nBorderDefenseWarPrizeWonLoyalty = 0;
	pTempleEvent.m_nBorderDefenseWarPrizeWonExp = 0;
	pTempleEvent.m_nBorderDefenseWarPrizeLoserLoyalty = 0;
	pTempleEvent.m_nBorderDefenseWarPrizeLoserItem = 0;
	pTempleEvent.m_nBorderDefenseWarPrizeLoserExp = 0;
	pTempleEvent.m_nBorderDefenseWarMAXLEVEL = 0;
	pTempleEvent.m_nBorderDefenseWarMINLEVEL = 0;
}
void CGameServerDlg::Announcement(uint16 type, int nation, int chat_type, CUser* pExceptUser, CNpc *pExpectNpc)
{
	string chatstr; 
	uint8 ZoneID = 0;

	switch (type)
	{
	case BATTLEZONE_OPEN:
		GetServerResource(IDP_BATTLEZONE_OPEN, &chatstr);
		break;
	case DECLARE_WINNER:
		if (m_bVictory == KARUS)
			GetServerResource(IDP_KARUS_VICTORY, &chatstr, m_sElmoradDead, m_sKarusDead);
		else if (m_bVictory == ELMORAD)
			GetServerResource(IDP_ELMORAD_VICTORY, &chatstr, m_sKarusDead, m_sElmoradDead);
		else 
			return;
		break;
	case DECLARE_LOSER:
		if (m_bVictory == KARUS)
			GetServerResource(IDS_ELMORAD_LOSER, &chatstr, m_sKarusDead, m_sElmoradDead);
		else if (m_bVictory == ELMORAD)
			GetServerResource(IDS_KARUS_LOSER, &chatstr, m_sElmoradDead, m_sKarusDead);
		else 
			return;
		break;
	case DECLARE_BAN:
		if (m_bVictory == KARUS || m_bVictory == ELMORAD)
			GetServerResource(IDS_BANISH_LOSER, &chatstr);
		else
			GetServerResource(IDS_BANISH_USER, &chatstr);
		break;
	case DECLARE_BATTLE_ZONE_STATUS:
		if (m_byBattleZone + ZONE_BATTLE_BASE == ZONE_BATTLE4)
		{
			GetServerResource(IDS_BATTLE_MONUMENT_STATUS, &chatstr,  m_sKarusMonumentPoint, m_sElmoMonumentPoint, m_sKarusDead, m_sElmoradDead);
			SendNotice<PUBLIC_CHAT>(chatstr.c_str(), ZONE_BATTLE4, Nation::ALL, true);
		}
		else
			GetServerResource(IDS_BATTLEZONE_STATUS, &chatstr,  m_sKarusDead, m_sElmoradDead);
		break;
	case DECLARE_BATTLE_MONUMENT_STATUS:
		if (pExceptUser)
		{
			GetServerResource(IDS_BATTLE_MONUMENT_WON_MESSAGE, &chatstr, GetBattleAndNationMonumentName(chat_type).c_str());
			g_pMain->SendNotice<PUBLIC_CHAT>(chatstr.c_str(),pExceptUser->GetZoneID(),pExceptUser->GetNation());
			GetServerResource(IDS_BATTLE_MONUMENT_LOST_MESSAGE, &chatstr, GetBattleAndNationMonumentName(chat_type).c_str());
			g_pMain->SendNotice<PUBLIC_CHAT>(chatstr.c_str(),pExceptUser->GetZoneID(),pExceptUser->GetNation() == KARUS ? ELMORAD : KARUS);
			return;
		}
		break;
	case DECLARE_NATION_MONUMENT_STATUS:
		if (pExceptUser)
		{
			uint16 nTrapNumber = pExceptUser->GetZoneID() == ZONE_KARUS ?  chat_type - LUFERSON_MONUMENT_SID : chat_type - ELMORAD_MONUMENT_SID;

			GetServerResource(IDS_INFILTRATION_CONQUER, &chatstr, GetBattleAndNationMonumentName(nTrapNumber, pExceptUser->GetZoneID()).c_str());
			g_pMain->SendAnnouncement(chatstr.c_str(), pExceptUser->GetNation());
			GetServerResource(IDS_INFILTRATION_RECAPTURE, &chatstr, GetBattleAndNationMonumentName(nTrapNumber, pExceptUser->GetZoneID()).c_str());
			g_pMain->SendAnnouncement(chatstr.c_str(), pExceptUser->GetNation() == KARUS ? ELMORAD : KARUS);
			return;
		}
		break;
	case DECLARE_NATION_REWARD_STATUS:
		if (pExpectNpc)
		{
			uint16 nTrapNumber = pExpectNpc->GetZoneID() == ZONE_KARUS ?  chat_type - LUFERSON_MONUMENT_SID : chat_type - ELMORAD_MONUMENT_SID;

			GetServerResource(pExpectNpc->GetNation() == KARUS ? IDS_INFILTRATION_REWARD_KARUS : IDS_INFILTRATION_REWARD_ELMORAD, &chatstr, GetBattleAndNationMonumentName(nTrapNumber, pExpectNpc->GetZoneID()).c_str());
			g_pMain->SendAnnouncement(chatstr.c_str(), Nation::ALL);
			return;
		}
		break;
	case SNOW_BATTLEZONE_OPEN:
		GetServerResource(IDS_SNOWBATTLE_OPEN, &chatstr);
		break;
	case UNDER_ATTACK_NOTIFY:
		if (m_bVictory == KARUS)
			GetServerResource(IDS_UNDER_ATTACK_ELMORAD, &chatstr, m_sKarusDead, m_sElmoradDead);
		else if (m_bVictory == ELMORAD)
			GetServerResource(IDS_UNDER_ATTACK_KARUS, &chatstr, m_sElmoradDead, m_sKarusDead);
		else 
			return;
		break;
	case BATTLEZONE_CLOSE:
		GetServerResource(IDS_BATTLE_CLOSE, &chatstr);
		break;
	case KARUS_CAPTAIN_NOTIFY:
		GetServerResource(IDS_KARUS_CAPTAIN, &chatstr, m_strKarusCaptain.c_str());
		break;
	case ELMORAD_CAPTAIN_NOTIFY:
		GetServerResource(IDS_ELMO_CAPTAIN, &chatstr, m_strElmoradCaptain.c_str());
		break;
	case KARUS_CAPTAIN_DEPRIVE_NOTIFY:
		if (pExceptUser)
		{
			CKnights *pKnights = g_pMain->GetClanPtr(pExceptUser->GetClanID());
			GetServerResource(IDS_KARUS_CAPTAIN_DEPRIVE, &chatstr, pKnights == nullptr ? "***" : pKnights->GetName().c_str(), pExceptUser->GetName().c_str());
		}
		break;
	case ELMORAD_CAPTAIN_DEPRIVE_NOTIFY:
		if (pExceptUser)
		{
			CKnights *pKnights = g_pMain->GetClanPtr(pExceptUser->GetClanID());
			GetServerResource(IDS_ELMO_CAPTAIN_DEPRIVE, &chatstr, pKnights == nullptr ? "***" : pKnights->GetName().c_str(),  pExceptUser->GetName().c_str());
		}
		break;
	case IDS_MONSTER_CHALLENGE_ANNOUNCEMENT:
		GetServerResource(IDS_MONSTER_CHALLENGE_ANNOUNCEMENT, &chatstr, m_nForgettenTempleLevelMin, m_nForgettenTempleLevelMax, m_nForgettenTempleStartHour);
		break;
	case IDS_MONSTER_CHALLENGE_OPEN:
		GetServerResource(IDS_MONSTER_CHALLENGE_OPEN, &chatstr);
		break;
	case IDS_MONSTER_CHALLENGE_START:
		ZoneID = ZONE_FORGOTTEN_TEMPLE;
		GetServerResource(IDS_MONSTER_CHALLENGE_START, &chatstr);
		break;
	case IDS_MONSTER_CHALLENGE_VICTORY:
		ZoneID = ZONE_FORGOTTEN_TEMPLE;
		GetServerResource(IDS_MONSTER_CHALLENGE_VICTORY, &chatstr);
		break;
	case IDS_MONSTER_CHALLENGE_CLOSE:
		GetServerResource(IDS_MONSTER_CHALLENGE_CLOSE, &chatstr);
		break;
	case IDS_SIEGE_WAR_TIME_NOTICE:
		GetServerResource(IDS_SIEGE_WAR_TIME_NOTICE, &chatstr, m_SiegeWarNoticeTime);
		break;
	case IDS_SIEGE_WAR_READY_TIME_NOTICE:
		GetServerResource(IDS_SIEGE_WAR_READY_TIME_NOTICE, &chatstr, m_SiegeWarNoticeTime);
		break;
	case IDS_SIEGE_WAR_START:
		GetServerResource(IDS_SIEGE_WAR_START, &chatstr);
		break;		
	case IDS_SIEGE_WAR_END:
		GetServerResource(IDS_SIEGE_WAR_END, &chatstr);
		break;		
	case IDS_NPC_GUIDON_DESTORY:
		GetServerResource(IDS_NPC_GUIDON_DESTORY, &chatstr, m_SiegeWarWinKnightsNotice.c_str());
		break;		
	case IDS_SIEGE_WAR_VICTORY:
		GetServerResource(IDS_SIEGE_WAR_VICTORY, &chatstr, m_SiegeWarWinKnightsNotice.c_str());
		break;
	case IDS_PRE_BATTLE_ANNOUNCEMENT:
		GetServerResource(IDS_PRE_BATTLE_ANNOUNCEMENT,&chatstr,m_WarNoticeTime);
		break;
	case IDS_PRE_SNOWBATTLE_ANNOUNCEMENT:
		GetServerResource(IDS_PRE_SNOWBATTLE_ANNOUNCEMENT,&chatstr,m_SnowWarNoticeTime);
		break;
	}

	Packet result;
	string finalstr;
	GetServerResource(IDP_ANNOUNCEMENT, &finalstr, chatstr.c_str());
	ChatPacket::Construct(&result, (uint8) chat_type, &finalstr);
	Send_All(&result, nullptr, nation, ZoneID);

	ChatPacket::Construct(&result, 7, &finalstr);
	Send_All(&result, nullptr, nation, ZoneID);
}

/**
* @brief	Loads the specified user's NP ranks
* 			from the rankings tables.
*
* @param	pUser	The user.
*/
void CGameServerDlg::GetUserRank(CUser *pUser)
{
	// Acquire the lock for thread safety
	Guard lock(m_userRankingsLock);

	// Get character's name & convert it to upper case for case insensitivity
	string strUserID = pUser->GetName();
	STRTOUPPER(strUserID);

	// Grab the personal rank from the map, if applicable.
	UserNameRankMap::iterator itr = m_UserPersonalRankMap.find(strUserID);
	pUser->m_bPersonalRank = itr != m_UserPersonalRankMap.end() ? int8(itr->second->nRank) : -1;

	// Grab the knights rank from the map, if applicable.
	itr = m_UserKnightsRankMap.find(strUserID);
	pUser->m_bKnightsRank = itr != m_UserKnightsRankMap.end() ? int8(itr->second->nRank) : -1;
}

uint16 CGameServerDlg::GetKnightsAllMembers(uint16 sClanID, Packet & result, uint16 & pktSize, bool bClanLeader)
{
	CKnights* pKnights = GetClanPtr(sClanID);
	if (pKnights == nullptr)
		return 0;

	uint16 count = 0;
	foreach_array (i, pKnights->m_arKnightsUser)
	{
		_KNIGHTS_USER *p = &pKnights->m_arKnightsUser[i];
		if (!p->byUsed)
			continue;

		CUser *pUser = p->pSession;
		if (pUser != nullptr)
			result << pUser->GetName() << pUser->GetFame() << pUser->GetLevel() << pUser->m_sClass << uint8(1);
		else // normally just clan leaders see this, but we can be generous now.
			result << pKnights->m_arKnightsUser[i].strUserName << uint8(0) << uint8(0) << uint16(0) << uint8(0);

		count++;
	}

	return count;
}

/**
* @brief	Calculates the clan 
from the specified
* 			loyalty points (NP).
*
* @param	nPoints	Loyalty points (NP). 
* 					The points will be converted to clan points 
* 					by this method.
*
* @return	The clan grade.
*/
int CGameServerDlg::GetKnightsGrade(uint32 nPoints)
{
	uint32 nClanPoints = nPoints /*/ MAX_CLAN_USERS*/;

	if (nClanPoints >= g_pMain->m_Loyalty_Clan_G1)	
		return 1;
	else if (nClanPoints >= g_pMain->m_Loyalty_Clan_G2)
		return 2;
	else if (nClanPoints >= g_pMain->m_Loyalty_Clan_G3)
		return 3;
	else if (nClanPoints >= g_pMain->m_Loyalty_Clan_G4)
		return 4;

	return 5;
}

/**
* @brief	Disconnects all players in the server.
*
* @return	The number of users who were in-game.
*/
int CGameServerDlg::KickOutAllUsers()
{
	int count = 0;

	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser *pUser = TO_USER(itr->second);
		if (pUser->isInGame())
			count++;
		else
			continue;

		pUser->ReqSaveCharacter();//visualdev agregado
		pUser->UserDataSaveToAgent();//visualdev agregado
		g_DBAgent.UpdateUser(pUser->GetName(), UPDATE_LOGOUT, pUser);
		g_DBAgent.UpdateWarehouseData(pUser->GetAccountName(), UPDATE_LOGOUT, pUser);
		g_DBAgent.UpdateSavedMagic(pUser);

		pUser->Disconnect();
		//Sleep(100); //visualdev
	}
	return count;
}

/**
* @brief	Generates a new item serial.
*/
uint64 CGameServerDlg::GenerateItemSerial()
{
	static std::recursive_mutex _mutex;

	MYINT64 serial;
	MYSHORT	increase;
	serial.i = 0;

	time_t t = UNIXTIME;
	struct tm * ptm;
	ptm = gmtime(&t);

	Guard lock(_mutex);
	increase.w = g_increase_serial++;

	serial.b[7] = (uint8)(m_nServerNo);
	serial.b[6] = (uint8)(ptm->tm_year % 100);
	serial.b[5] = (uint8)(ptm->tm_mon);
	serial.b[4] = (uint8)(ptm->tm_mday);
	serial.b[3] = (uint8)(ptm->tm_hour);
	serial.b[2] = (uint8)(ptm->tm_min);
	serial.b[1] = increase.b[1];
	serial.b[0] = increase.b[0];

	return serial.i;
}

/**
* @brief	Kick out all users from the specified zone
* 			to their home zone.
*
* @param	zone	The zone to kick users out from.
*/
void CGameServerDlg::KickOutZoneUsers(uint8 ZoneID, uint8 TargetZoneID, uint8 bNation)
{
	// TODO: Make this localised to zones.
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	C3DMap	*pKarusMap		= GetZoneByID(KARUS),
		*pElMoradMap	= GetZoneByID(ELMORAD);	

	ASSERT (pKarusMap != nullptr && pElMoradMap != nullptr);

	if (ZoneID == ZONE_BIFROST)
		g_DBAgent.BifrostBosalt();

	foreach (itr, sessMap)
	{
		// Only kick users from requested zone.
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame()
			|| pUser->GetZoneID() != ZoneID) 
			continue;

		if (TargetZoneID > 0 && bNation == pUser->GetNation())
		{
			pUser->ZoneChange(TargetZoneID, 0.0f,0.0f);
			continue;
		}
		else if (TargetZoneID > 0 && bNation == ALL)
		{
			pUser->ZoneChange(TargetZoneID, 0.0f,0.0f);
			continue;
		}

		C3DMap * pMap = (pUser->GetNation() == KARUS ? pKarusMap : pElMoradMap);
		if(TargetZoneID == 0)
			pUser->ZoneChange(pMap->m_nZoneNumber, pMap->m_fInitX, pMap->m_fInitZ);
	}
}

void CGameServerDlg::SendItemZoneUsers(uint8 ZoneID, uint32 nItemID, uint16 sCount /*= 1*/)
{
	// TODO: Make this localised to zones.
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame()
			|| pUser->GetZoneID() != ZoneID) 
			continue;

		pUser->GiveItem(nItemID, sCount);
	}
}

void CGameServerDlg::SendItemEventRoom(uint16 nEventRoom,uint32 nItemID, uint16 sCount /*= 1*/)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame()
			|| pUser->GetEventRoom() != nEventRoom) 
			continue;

		pUser->GiveItem(nItemID, sCount);
	}
}

void CGameServerDlg::Send_CommandChat(Packet *pkt, int nation, CUser* pExceptUser)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser->isInGame() && (nation == 0 || nation == pUser->GetNation()))
			pUser->Send(pkt);
	}
}

void CGameServerDlg::GetCaptainUserPtr()
{
	foreach_stlmap (itr, m_KnightsArray)
	{
		CKnights *pKnights = itr->second;
		if (pKnights->m_byRanking != 1)
			continue;

		// do something cool here
	}
}

/**
* @brief	Updates the number of users currently in the war zone
* 			and sends the user counts to all servers in this group.
*/
void CGameServerDlg::BattleZoneCurrentUsers()
{
	C3DMap* pMap = GetZoneByID(ZONE_BATTLE_BASE + m_byBattleZone);
	if (pMap == nullptr || m_nServerNo != pMap->m_nServerNo)
		return;

	uint16 nKarusMan = 0, nElmoradMan = 0;
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (!pUser->isInGame() || pUser->GetZoneID() != pMap->GetID())
			continue;

		if (pUser->GetNation() == KARUS)
			nKarusMan++;
		else
			nElmoradMan++;
	}

	m_sKarusCount = nKarusMan;
	m_sElmoradCount = nElmoradMan;
}

/**
* @brief	Sends the flying santa/angel packet to all users in the server.
*/
void CGameServerDlg::SendFlyingSantaOrAngel()
{
	Packet result(WIZ_SANTA, uint8(m_bSantaOrAngel));
	Send_All(&result);
}

CGameServerDlg::~CGameServerDlg() 
{
	printf("Waiting for timer threads to exit...");
	foreach (itr, g_timerThreads)
	{
		(*itr)->waitForExit();
		delete (*itr);
	}
	printf(" exited.\n");

	// Cleanup our script pool & consequently ensure all scripts 
	// finish execution before proceeding.
	// This prevents us from freeing data that's in use.
	printf("Shutting down Lua engine...");
	m_luaEngine.Shutdown();
	printf(" done.\n");

	printf("Shutting down database system...");
	DatabaseThread::Shutdown();
	printf(" done.\n");

	printf("Shutting down socket system...");
	m_aiSocketMgr.Shutdown();
	m_socketMgr.Shutdown();
	printf(" done.\n");

	CUser::CleanupChatCommands();
	CGameServerDlg::CleanupServerCommands();

	CleanupUserRankings();
	m_LevelUpArray.clear();

	if (m_fpDeathUser != nullptr)
		fclose(m_fpDeathUser);

	if (m_fpMerchant != nullptr)
		fclose(m_fpMerchant);

	if (m_fpTrade != nullptr)
		fclose(m_fpTrade);

	if (m_fpDeathNpc != nullptr)
		fclose(m_fpDeathNpc);

	if (m_fpChat != nullptr)
		fclose(m_fpChat);

	if (m_fpCheat != nullptr)
		fclose(m_fpCheat);
}

std::string CGameServerDlg::GetBattleAndNationMonumentName(int16 TrapNumber, uint8 ZoneID)
{
	std::string sMonumentName = ZoneID == 0 ? "Nereid Monument" : "DECLARE_NATION_MONUMENT_STATUS";

	if (ZoneID == 0)
	{
		if (TrapNumber == 1)
			sMonumentName = "El Morad main territory";
		else if (TrapNumber == 2)
			sMonumentName = "El Morad provision line";
		else if (TrapNumber == 3)
			sMonumentName = "Lake of Life";
		else if (TrapNumber == 4)
			sMonumentName = "Foss Castle";
		else if (TrapNumber == 5)
			sMonumentName = "Karus main territory";
		else if (TrapNumber == 6)
			sMonumentName = "Karus provision line";
		else if (TrapNumber == 7)
			sMonumentName = "Swamp of Shadows";
	}
	else
	{
		if (ZoneID == ZONE_KARUS && TrapNumber == 1)
			TrapNumber = 2;
		else if (ZoneID == ZONE_KARUS && TrapNumber == 2)
			TrapNumber = 1;

		if (TrapNumber == 0)
			sMonumentName = string_format("%s Monument", ZoneID == ZONE_KARUS ? "Luferson" : "El Morad");
		else if (TrapNumber == 1)
			sMonumentName = string_format("%s Monument", ZoneID == ZONE_KARUS ? "Bellua" : "Asga Village");
		else if (TrapNumber == 2)
			sMonumentName = string_format("%s Monument", ZoneID == ZONE_KARUS ? "Linate" : "Raiba Village");
		else if (TrapNumber == 3)
			sMonumentName = string_format("%s Monument", ZoneID == ZONE_KARUS ? "Laon Camp" : "Dodo Camp");
	}

	return sMonumentName;
}

void CGameServerDlg::CheckNationMonumentRewards()
{
	std::vector<uint16> deleted;

	foreach_stlmap (itr, m_NationMonumentInformationArray)
	{
		if (int32(UNIXTIME) - itr->second->RepawnedTime < 20)
			continue;

		CNpc *pNpc = GetNpcPtr(itr->second->sNid);

		if (pNpc == nullptr)
		{
			deleted.push_back(itr->second->sSid);
			continue;
		}

		uint16 nTrapNumber = pNpc->GetZoneID() == ZONE_KARUS ?  itr->second->sSid - LUFERSON_MONUMENT_SID : itr->second->sSid - ELMORAD_MONUMENT_SID;

		std::vector<Unit *> distributed_member;
		std::vector<uint16> unitList;
		g_pMain->GetUnitListFromSurroundingRegions(pNpc, &unitList);

		foreach (itrx, unitList)
		{		
			Unit * pTarget = g_pMain->GetUnitPtr(*itrx);

			if(pTarget == nullptr || pTarget->isNPC() /*|| pTarget->GetNation() == pNpc->GetZoneID()*/)
				continue; 

			if (pTarget->GetNation() == pNpc->GetNation() && pTarget->isInRangeSlow(pNpc,RANGE_50M))
				distributed_member.push_back(pTarget);
		}

		foreach (itry, distributed_member)
		{
			Unit * pTarget = *itry;

			if(pTarget == nullptr || pTarget->isNPC() /*|| pTarget->GetNation() == pNpc->GetZoneID()*/)
				continue;

			g_pMain->m_lastBlessTime = UNIXTIME;
			TO_USER(pTarget)->SendLoyaltyChange(nTrapNumber == 0 ? 200 : 50);
			Announcement(DECLARE_NATION_REWARD_STATUS, Nation::ALL, itr->second->sSid, nullptr, pNpc);
			ShowNpcEffect(itr->second->sNid,20100,pNpc->GetZoneID());
		}
	}

	foreach (itr, deleted)
		g_pMain->m_NationMonumentInformationArray.DeleteData(*itr);
}

void CGameServerDlg::ShowNpcEffect(uint16 sNpcID, uint32 nEffectID, uint8 ZoneID)
{
	Packet result(WIZ_OBJECT_EVENT, uint8(OBJECT_NPC));
	result << uint8(3) << sNpcID << nEffectID;
	g_pMain->Send_Zone(&result, ZoneID);
}

void CGameServerDlg::WriteDeathUserLogFile(string & logMessage)
{
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpDeathUser);
	fflush(m_fpDeathUser);
}

void CGameServerDlg::WriteMerchantUserLogFile(string & logMessage)
{
	string logMessageEnd;
	DateTime time;
	logMessageEnd = string_format(_T("[ %d:%d:%d ] %s\n"),time.GetHour(),time.GetMinute(),time.GetSecond(),logMessage.c_str());

	fwrite(logMessageEnd.c_str(), logMessageEnd.length(), 1, m_fpMerchant);
	fflush(m_fpMerchant);
}

void CGameServerDlg::WriteTradeUserLogFile(string & logMessage)
{
	string logMessageEnd;
	DateTime time;
	logMessageEnd = string_format(_T("[ %d:%d:%d ] %s\n"),time.GetHour(),time.GetMinute(),time.GetSecond(),logMessage.c_str());

	fwrite(logMessageEnd.c_str(), logMessageEnd.length(), 1, m_fpTrade);
	fflush(m_fpTrade);
}

void CGameServerDlg::WriteDeathNpcLogFile(string & logMessage)
{
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpDeathNpc);
	fflush(m_fpDeathNpc);
}

void CGameServerDlg::WriteChatLogFile(string & logMessage)
{
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpChat);
	fflush(m_fpChat);
}

void CGameServerDlg::WriteCheatLogFile(string & logMessage)
{
	fwrite(logMessage.c_str(), logMessage.length(), 1, m_fpCheat);
	fflush(m_fpCheat);
}

bool CGameServerDlg::IsDuplicateItem(uint32 nItemID, uint64 nItemSerial)
{
	return false;

	if (nItemID <= 0 || nItemSerial <= 0)
		return false;

	_USER_ITEM * pUserItem = g_pMain->m_UserItemArray.GetData(nItemID);

	if (pUserItem == nullptr)
		return false;

	uint32 nDuplicateCount = 0;

	foreach(itr, pUserItem->nItemSerial)
	{
		if (*itr == nItemSerial)
			nDuplicateCount++;

		if (nDuplicateCount > 1)
			return true;
	}

	return false;
}

void CGameServerDlg::AddUserItem(uint32 nItemID, uint64 nItemSerial)
{
	if (nItemID <= 0 || nItemSerial <= 0)
		return;

	_USER_ITEM * pUserItem = g_pMain->m_UserItemArray.GetData(nItemID);

	if (pUserItem == nullptr)
	{
		pUserItem = new _USER_ITEM;
		pUserItem->nItemID = nItemID;
		pUserItem->nItemSerial.push_back(nItemSerial);

		if (!g_pMain->m_UserItemArray.PutData(pUserItem->nItemID,pUserItem))
			delete pUserItem;
	}
	else
		pUserItem->nItemSerial.push_back(nItemSerial);
}

void CGameServerDlg::ReloadKnightAndUserRanks()
{
	g_DBAgent.UpdateRanks();

	// Update clan grades/rankings
	m_KnightsArray.DeleteAllData();
	m_KnightsAllianceArray.DeleteAllData();//error 1
	m_KnightsRatingArray[KARUS_ARRAY].DeleteAllData();
	m_KnightsRatingArray[ELMORAD_ARRAY].DeleteAllData();
	LoadAllKnights(true);
	LoadAllKnightsUserData(true);
	LoadKnightsAllianceTable(true);
	LoadKnightsRankTable(false, true);

	// Update user rankings
	LoadUserRankings();
}

void CGameServerDlg::SetPlayerRankingRewards(uint16 ZoneID)
{
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser == nullptr 
			|| !pUser->isInGame()
			|| pUser->isGM()
			|| pUser->GetZoneID() != ZoneID)
			continue;

		if (pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) > 0
			&& pUser->GetPlayerRank(RANK_TYPE_PK_ZONE) <= 10)
		{
			if (m_nPlayerRankingLoyaltyReward > 0)
				pUser->SendLoyaltyChange(m_nPlayerRankingLoyaltyReward, false, true, false);
			if (m_nPlayerRankingKnightCashReward > 0)
				g_DBAgent.UpdateAccountKnightCash(pUser->GetAccountName(), m_nPlayerRankingKnightCashReward);
		}
	}
}

bool CGameServerDlg::CastleSiegeWarAttack(CUser *pUser, CUser *pTargetUser)
{
	enum
	{CHALLENGE_CLAN_ERROR = 13};

	Packet result(WIZ_CHALLENGE, uint8(CHALLENGE_CLAN_ERROR));

	pUser->m_sChallengeUser = -1;
	pUser->m_bChallengeRequested = 0;

	if (pUser == nullptr)
	{
		pUser->Send(&result);
		return false;
	}


	pUser->m_sChallengeUser = -1;
	pUser->m_bRequestingChallenge = 0;

	CKnights *pClan1 = g_pMain->GetClanPtr(pUser->GetClanID());
	CKnights *pClan2 = g_pMain->GetClanPtr(pTargetUser->GetClanID());

	if (pClan1 == nullptr || pClan2 == nullptr)
	{
		pTargetUser->Send(&result);
		pUser->Send(&result);
		return false;
	}

	if (pUser->GetClanID() == pTargetUser->GetClanID())
	{
		pTargetUser->Send(&result);
		pUser->Send(&result);
		return false;
	}

	foreach_array (i, pClan1->m_arKnightsUser)
	{
		_KNIGHTS_USER *p = &pClan1->m_arKnightsUser[i];
		if (!p->byUsed || p->pSession == nullptr)
			continue;

		CUser *pClanMember = p->pSession;
		if (pClanMember->GetZoneID() == pUser->GetZoneID()
			/*&& !pClanMember->isStoreOpen()
			&& !pClanMember->isMerchanting()
			&& !pClanMember->isTrading()
			&& !pClanMember->isInParty()
			&& !pClanMember->m_bRequestingChallenge
			&& pClanMember->m_sChallengeUser < 0 */
			&& !CKnightsManager::CheckAlliance(pClan1 , pClan2))
			return true;
	}

	foreach_array (i, pClan2->m_arKnightsUser)
	{
		_KNIGHTS_USER *p = &pClan2->m_arKnightsUser[i];
		if (!p->byUsed || p->pSession == nullptr)
			continue;

		CUser *pClanMember = p->pSession;
		if (pClanMember->GetZoneID() == pTargetUser->GetZoneID()
			/*&& !pClanMember->isStoreOpen()
			&& !pClanMember->isMerchanting()
			&& !pClanMember->isTrading()
			&& !pClanMember->isInParty()
			&& !pClanMember->m_bRequestingChallenge
			&& pClanMember->m_sChallengeUser < 0 */
			&& !CKnightsManager::CheckAlliance(pClan1 , pClan2))
			return true;
	}
	return false;
}

void CGameServerDlg::UpdateSiege(int16 m_sCastleIndex, int16 m_sMasterKnights, int16 m_bySiegeType, int16 m_byWarDay, int16 m_byWarTime, int16 m_byWarMinute)
{
	g_DBAgent.UpdateSiege(m_sCastleIndex, m_sMasterKnights, m_bySiegeType, m_byWarDay, m_byWarTime, m_byWarMinute);
}

void CGameServerDlg::UpdateSiegeTax(uint8 Zone, int16 ZoneTarrif)
{
	g_DBAgent.UpdateSiegeTax(Zone, ZoneTarrif);
}
void CGameServerDlg::LunarGoldShells(CUser *pUser){
	if (pUser != nullptr){
		Packet result(WIZ_MAP_EVENT);
		result << uint8(9) << uint8(1);
		if (pUser == nullptr 
			|| !pUser->isInGame())
			return;

		pUser->Send(&result);

		return;
	}

	Packet result(WIZ_MAP_EVENT);
	result << uint8(9) << uint8(1); //wiz_map_event , 9 gold shell , 1 giydir 
	Send_All(&result); 
}


void CGameServerDlg::DeleteLunarGoldShells()
{
	Packet result(WIZ_MAP_EVENT);
	result << uint8(9) << uint8(0); //wiz_map_event , 9 gold shell , 0 sildir    
	Send_All(&result);
}

//war 64
void CGameServerDlg::NereidsMonumentEvent(uint8 TrapNumber, uint8 bNation, CUser * pUser)
{
	Packet result(WIZ_MAP_EVENT);
	result << uint8(0) << uint8(7);


	if (pUser == nullptr)
	{
		uint8 MonumentID;
		switch(TrapNumber)
		{
		case 1:
			MonumentID = 6;
			break;
		case 2:
			MonumentID = 1;
			break;
		case 3:
			MonumentID = 2;
			break;
		case 4:
			MonumentID = 3;
			break;
		case 5:
			MonumentID = 0;
			break;
		case 6:
			MonumentID = 5;
			break;
		case 7:
			MonumentID = 4;
			break;
		}
		m_sNereidsMonumentNation[MonumentID] = bNation;
	}

	for (int i = 0; i < 7; i++)
		result << m_sNereidsMonumentNation[i];

	if(pUser != nullptr)
		pUser->Send(&result); 
	else
		Send_Zone(&result,ZONE_BATTLE4); 
}
