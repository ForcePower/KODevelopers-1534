#pragma once

#ifdef GAMESERVER

#include "resource.h"
#include "LuaEngine.h"

#include "Define.h"
#include "ChatHandler.h"

class C3DMap;
class CUser;

#include "LoadServerData.h"

#include "User.h"
#include "AISocket.h"
#include "../shared/ClientSocketMgr.h"

typedef std::map<std::string, CUser *>	NameMap;
typedef	std::map<uint16, uint16>					ForgettenTempleMonsterList;

class CGameServerDlg
{
public:	
	CGameServerDlg();
	bool Startup();

	void GetTimeFromIni();

	bool LoadItemTable();
	bool LoadSetItemTable();
	bool LoadItemExchangeTable();
	bool LoadItemUpgradeTable();
	bool LoadItemOpTable();
	bool LoadExpiration();
	bool LoadServerResourceTable();
	bool LoadQuestHelperTable();
	bool LoadQuestMonsterTable();
	bool LoadMagicTable();
	bool LoadMagicType1();
	bool LoadMagicType2();
	bool LoadMagicType3();
	bool LoadMagicType4();
	bool LoadMagicType5();
	bool LoadMagicType6();
	bool LoadMagicType7();
	bool LoadMagicType8();
	bool LoadMagicType9();
	bool LoadRentalList();
	bool LoadCoefficientTable();
	bool LoadLevelUpTable();
	bool LoadAllKnights(bool bIsslient = false);
	bool LoadAllKnightsUserData(bool bIsslient = false);
	bool LoadKnightsAllianceTable(bool bIsslient = false);
	bool LoadKnightsSiegeWarsTable();
	bool LoadUserRankings();
	void CleanupUserRankings();
	bool LoadKnightsCapeTable();
	bool LoadKnightsRankTable(bool bWarTime = false, bool bIsslient = false);
	bool LoadStartPositionTable();
	bool LoadStartPositionRandomTable();
	bool LoadBattleTable();
	bool LoadKingSystem();
	bool LoadMonsterSummonListTable();
	bool LoadMonsterSummonListZoneTable();
	bool LoadMonsterRespawnListTable();
	bool LoadMonsterRespawnListInformationTable();
	bool LoadPremiumItemTable();
	bool LoadPremiumItemExpTable();
	bool LoadUserDailyOpTable();
	bool LoadEventTriggerTable();
	bool LoadMonsterChallengeTable();
	bool LoadMonsterChallengeSummonListTable();
	bool LoadUserItemTable();
	bool LoadObjectPosTable();

	bool MapFileLoad();
	bool LoadNoticeData();

	void AIServerConnect();

	static uint32 THREADCALL Timer_CheckGameEvents(void * lpParam);
	static uint32 THREADCALL Timer_BifrostTime(void * lpParam); 
	static uint32 THREADCALL Timer_UpdateGameTime(void * lpParam);
	static uint32 THREADCALL Timer_UpdateSessions(void * lpParam);
	static uint32 THREADCALL Timer_UpdateConcurrent(void * lpParam);

	void ReqUpdateConcurrent();
	uint32 ReqUpdateDatas();
	void SendFlyingSantaOrAngel();
	void BattleZoneCurrentUsers();
	void GetCaptainUserPtr();
	void Send_CommandChat(Packet *pkt, int nation, CUser* pExceptUser = nullptr);
	void KickOutZoneUsers(uint8 ZoneID, uint8 TargetZoneID = 0, uint8 bNation = 0);
	void SendItemZoneUsers(uint8 ZoneID, uint32 nItemID, uint16 sCount = 1);
	void SendItemEventRoom(uint16 nEventRoom,uint32 nItemID, uint16 sCount = 1);
	uint64 GenerateItemSerial();
	int KickOutAllUsers();
	void CheckAliveUser();
	int GetKnightsGrade(uint32 nPoints);
	void WritePacketLog();
	uint16 GetKnightsAllMembers(uint16 sClanID, Packet & result, uint16 & pktSize, bool bClanLeader);
	void GetUserRank(CUser *pUser);
	void Announcement(uint16 type, int nation=0, int chat_type=8, CUser* pExceptUser = nullptr, CNpc *pExpectNpc = nullptr);
	void ResetBattleZone();
	void BanishLosers();
	void BattleZoneVictoryCheck();
	void BattleZoneOpenTimer();
	//visualdev notice
	void BattleZoneNotice();
	void BattleZoneSnowNotice();
	///end
	void BattleZoneOpen(int nType, uint8 bZone = 0);
	void CastleSiegeWarZoneOpen(int nType, uint8 bZone = 0);
	void BattleZoneClose();
	uint8 GetTariffByZone(int zoneid);
	void BattleZoneResult(uint8 nation);
	void BattleWinnerResult(BattleWinnerTypes winnertype);
	void AliveUserCheck();
	void TerminationFinish(uint16 nEventRoom);
	//snow war visualdev
	void SnowBattleZoneOpenTimer();
	//end
	void Send_PartyMember(int party, Packet *result);
	void Send_KnightsMember(int index, Packet *pkt);
	void Send_KnightsAlliance(uint16 sAllianceID, Packet *pkt);
	void SetGameTime();
	void ResetPlayerRankings(uint8 ZoneID = 0);
	void UpdateWeather();
	void UpdateGameTime();
	//snow war visualdev
	void SnowBanishLosers();
	void SnowBattleZoneClose();
	void SnowBattleZoneCurrentUsers();
	//end
	void ResetLoyaltyMonthly();
	void SendAllUserInfo();
	void DeleteAllNpcList(int flag = 0);
	CNpc*  FindNpcInZone(uint16 sPid, uint8 byZone);
	void ForgettenTempleEventTimer();
	//war 64
	void NereidsMonumentEvent(uint8 TrapNumber = 0, uint8 bNation = 0, CUser * pUser = nullptr);
	uint8	m_sNereidsMonumentNation[7];
	//end
	uint8 GetMonsterChallengeTime();
	//visualdev bless
	time_t	m_lastBlessTime;
	//end
	void TempleEventTimer();
	void BowlEventTimer();
	void TempleEventStart();
	void TempleEventTeleportUsers();
	uint8 TempleEventGetRoomUsers(uint16 nEventRoom = 0, uint8 nNation = 0);
	void TempleEventSummon();
	void TempleEventSummonChaosCubes();
	void TempleEventFinish(uint16 nEventRoom);
	void TempleEventGetActiveEventTime(CUser *pUser);
	void TempleEventKickOutUser(uint16 nEventRoom);
	void TemploVotarUser(CUser *pUser);
	void TempleEventReset();
	void AddEventUser(CUser * pUser);
	void RemoveEventUser(CUser * pUser);
	void UpdateEventUser(CUser * pUser, uint16 nEventRoom = 0);
	void SetEventUser(CUser *pUser);
	std::string GetBattleAndNationMonumentName(int16 TrapNumber = -1, uint8 ZoneID = 0);
	void CheckNationMonumentRewards();

	void ReloadKnightAndUserRanks();
	void SetPlayerRankingRewards(uint16 ZoneID);

	bool CastleSiegeWarAttack(CUser *pUser, CUser *pTargetUser);
	void UpdateSiege(int16 m_sCastleIndex, int16 m_sMasterKnights, int16 m_bySiegeType, int16 m_byWarDay, int16 m_byWarTime, int16 m_byWarMinute);
	void UpdateSiegeTax(uint8 Zone, int16 ZoneTarrif);

	bool IsDuplicateItem(uint32 nItemID, uint64 nItemSerial);
	void AddUserItem(uint32 nItemID, uint64 nItemSerial);
	void LunarGoldShells(CUser *pUser = nullptr);
	void DeleteLunarGoldShells();
	void AddDatabaseRequest(Packet & pkt, CUser *pUser = nullptr);

	void TerminitionTime(uint8 sNewZone,uint16 Time);

	// Get info for NPCs in regions around user (WIZ_REQ_NPCIN)
	void NpcInOutForMe(CUser* pSendUser);

	// Get info for NPCs in region
	void GetRegionNpcIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom, CUser* pSendUser);

	// Get list of NPC IDs in region
	void GetRegionNpcList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom = 0);

	// Get list of NPCs for regions around user (WIZ_NPC_REGION)
	void RegionNpcInfoForMe(CUser* pSendUser);	

	// Get raw list of all units in regions surrounding pOwner.
	void GetUnitListFromSurroundingRegions(Unit * pOwner, std::vector<uint16> * pList);

	// Get info for users in regions around user (WIZ_REQ_USERIN)
	void UserInOutForMe(CUser* pSendUser);

	// Get info for users in region
	void GetRegionUserIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom = 0);

	// Get list of user IDs in region
	void GetRegionUserList(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom = 0);

	// Get list of users for regions around user (WIZ_REGIONCHANGE)
	void RegionUserInOutForMe(CUser* pSendUser);

	// Get info for merchants in regions around user
	void MerchantUserInOutForMe(CUser* pSendUser);

	// Get war status
	INLINE bool isWarOpen() { return m_byBattleOpen != NO_BATTLE;} 

	// Get list of merchants in region
	void GetRegionMerchantUserIn(C3DMap* pMap, uint16 region_x, uint16 region_z, Packet & pkt, uint16 & t_count, uint16 nEventRoom = 0);

	void SendHelpDescription(CUser *pUser, std::string sHelpMessage);

	void SendColorfulNotice(CUser *pUser, std::string sHelpMessage);

	INLINE bool isPermanentMessageSet() { return m_bPermanentChatMode; }
	void SetPermanentMessage(const char * format, ...);

	void HandleConsoleCommand(const char * msg);

	template <ChatType chatType>
	INLINE void SendChat(const char * msg, uint8 byNation = Nation::ALL, bool bFormatNotice = false)
	{
		Packet result;
		std::string buffer;

		if (bFormatNotice)
			GetServerResource(IDP_ANNOUNCEMENT, &buffer, msg);
		else
			buffer = msg;

		ChatPacket::Construct(&result, (uint8) chatType, &buffer);
		Send_All(&result, nullptr, byNation);
	}

	template <ChatType chatType>
	INLINE void SendChatUpgrade(const char * msg, uint8 byNation = Nation::ALL, bool bFormatNotice = false)
	{
		Packet result;
		std::string buffer;


		if (bFormatNotice)
			GetServerResource(IDP_SUCCED, &buffer, msg);
		else
			buffer = msg;


		ChatPacket::Construct(&result, (uint8) chatType, &buffer);
		Send_All(&result, nullptr, byNation);
	}

	template <ChatType chatType>
	INLINE void SendChatToZone(const char * msg, uint8 ZoneID, uint8 byNation = Nation::ALL, bool bFormatNotice = false)
	{
		Packet result;
		std::string buffer;

		if (bFormatNotice)
			GetServerResource(IDP_ANNOUNCEMENT, &buffer, msg);
		else
			buffer = msg;

		ChatPacket::Construct(&result, (uint8) chatType, &buffer);
		Send_Zone(&result, ZoneID, nullptr, byNation);
	}

	template <ChatType chatType>
	INLINE void SendFormattedChat(const char * msg, uint8 byNation = Nation::ALL, bool bFormatNotice = false, va_list args = nullptr)
	{
		char buffer[512];
		vsnprintf(buffer, sizeof(buffer), msg, args);
		SendChat<chatType>(buffer, byNation, bFormatNotice);
		va_end(args);
	}

	template <ChatType chatType>
	void SendFormattedChat(const char * msg, uint8 byNation = Nation::ALL, bool bFormatNotice = false, ...)
	{
		va_list ap;
		va_start(ap, byNation);
		SendFormattedChat<chatType>(msg, byNation, bFormatNotice, ap);
		va_end(ap);
	}

	/* The following are simply wrappers for more readable SendChat() calls */

	INLINE void SendNotice(const char *msg, uint8 byNation = Nation::ALL) 
	{
		SendChat<PUBLIC_CHAT>(msg, byNation, true);
	}

	INLINE void SendNoticeall(const char *msg, uint8 byNation = Nation::ALL) 
	{
		SendChat<WAR_SYSTEM_CHAT>(msg, byNation, true);
	}

	INLINE void SendNoticePm(const char *msg, uint8 byNation = Nation::ALL) 
	{
		SendChat<PRIVATE_CHAT>(msg, byNation, true);
	}

	INLINE void SendNoticeClan(const char *msg, uint8 byNation = Nation::ALL) 
	{
		SendChat<KNIGHTS_CHAT>(msg, byNation, true);
	}

	INLINE void SendNoticeAlly(const char *msg, uint8 byNation = Nation::ALL) 
	{
		SendChat<ALLIANCE_CHAT>(msg, byNation, true);
	}

	INLINE void SendNoticeCommand(const char *msg, uint8 byNation = Nation::ALL) 
	{
		SendChat<COMMAND_CHAT>(msg, byNation, true);
	}

	INLINE void SendNoticePart(const char *msg, uint8 byNation = Nation::ALL) 
	{
		SendChat<PARTY_CHAT>(msg, byNation, true);
	}


	template <ChatType chatType>
	INLINE void SendNotice(const char *msg, uint8 ZoneID, uint8 byNation = Nation::ALL, bool bFormatNotice = false) 
	{
		SendChatToZone<chatType>(msg, ZoneID, byNation, bFormatNotice);
	}

	void SendFormattedNotice(const char *msg, uint8 byNation = Nation::ALL, ...)
	{
		va_list ap;
		va_start(ap, byNation);
		SendFormattedChat<PUBLIC_CHAT>(msg, byNation, true, ap);
		va_end(ap);
	}

	INLINE void SendAnnouncement(const char *msg, uint8 byNation = Nation::ALL)
	{
		SendChat<WAR_SYSTEM_CHAT>(msg, byNation, true);
	}

	template <ChatType chatType>
	INLINE void SendUpgradeRebirth(const char *msg, uint8 byNation = Nation::ALL)
	{
		SendChatUpgrade<chatType>(msg, byNation, true);
	}

	void SendFormattedAnnouncement(const char *msg, uint8 byNation = Nation::ALL, ...)
	{
		va_list ap;
		va_start(ap, byNation);
		SendFormattedChat<WAR_SYSTEM_CHAT>(msg, byNation, true, ap);
		va_end(ap);
	}

	void SendFormattedResource(uint32 nResourceID, uint8 byNation = Nation::ALL, bool bIsNotice = true, ...);

	void Send_Region(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, uint16 nEventRoom = 0);
	void Send_UnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, uint16 nEventRoom = 0);
	void Send_OldRegions(Packet *pkt, int old_x, int old_z, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, uint16 nEventRoom = 0);
	void Send_NewRegions(Packet *pkt, int new_x, int new_z, C3DMap *pMap, int x, int z, CUser* pExceptUser = nullptr, uint16 nEventRoom = 0);

	void Send_NearRegion(Packet *pkt, C3DMap *pMap, int region_x, int region_z, float curx, float curz, CUser* pExceptUser=nullptr, uint16 nEventRoom = 0);
	void Send_FilterUnitRegion(Packet *pkt, C3DMap *pMap, int x, int z, float ref_x, float ref_z, CUser* pExceptUser=nullptr, uint16 nEventRoom = 0);

	void Send_Zone_Matched_Class(Packet *pkt, uint8 bZoneID, CUser* pExceptUser, uint8 nation, uint8 seekingPartyOptions, uint16 nEventRoom = 0);
	void Send_Zone(Packet *pkt, uint8 bZoneID, CUser* pExceptUser = nullptr, uint8 nation = 0, uint16 nEventRoom = 0, float fRange = 0.0f);

	void Send_All(Packet *pkt, CUser* pExceptUser = nullptr, uint8 nation = 0, uint8 ZoneID = 0, bool isSendEventUsers = false, uint16 nEventRoom = 0);
	void Send_AIServer(Packet *pkt);

	void GetServerResource(int nResourceID, std::string * result, ...);
	_START_POSITION *GetStartPosition(int nZoneID);

	uint32 KCmiktari;
	bool KCaktifmi, onbesinde;
	int64 GetExpByLevel(int nLevel);
	C3DMap * GetZoneByID(int zoneID);

	CUser * GetUserPtr(std::string findName, NameType type);
	CUser * GetUserPtr(uint16 sUserId);
	CNpc  * GetNpcPtr(uint16 sNpcId);

	Unit * GetUnitPtr(uint16 id);

	// Spawns an event NPC/monster
	void SpawnEventNpc(uint16 sSid, bool bIsMonster, uint8 byZone, float fX, float fY, float fZ, uint16 sCount = 1, uint16 sRadius = 0, uint16 sDuration = 0, uint8 nation = 0, int16 socketID = -1, uint16 nEventRoom = 0);

	// Kill a Npc/Monster
	void KillNpc(uint16 sNid);

	// Change NPC/Monster properties for Respawn
	void NpcUpdate(uint16 sSid, bool bIsMonster, uint8 byGroup = 0, uint16 sPid = 0);	

	// Adds the account name & session to a hashmap (on login)
	void AddAccountName(CUser *pSession);

	// Adds the character name & session to a hashmap (when in-game)
	void AddCharacterName(CUser *pSession);

	// Removes an existing character name/session from the hashmap, replaces the character's name 
	// and reinserts the session with the new name into the hashmap.
	void ReplaceCharacterName(CUser *pSession, std::string & strNewUserID);

	// Removes the account name & character names from the hashmaps (on logout)
	void RemoveSessionNames(CUser *pSession);
	// Send to Zone NPC Effect
	void ShowNpcEffect(uint16 sNpcID, uint32 nEffectID, uint8 ZoneID);

	_PARTY_GROUP * GetPartyPtr(uint16 sPartyID);
	CKnights * GetClanPtr(uint16 sClanID);
	_KNIGHTS_ALLIANCE * GetAlliancePtr(uint16 sAllianceID);
	_ITEM_TABLE * GetItemPtr(uint32 nItemID);
	_KNIGHTS_SIEGE_WARFARE * GetSiegeMasterKnightsPtr(uint16 sMasterKnights);

	_PARTY_GROUP * CreateParty(CUser *pLeader);
	void DeleteParty(uint16 sIndex);

	_EVENT_STATUS pTempleEvent;

	~CGameServerDlg();

	char	m_ppNotice[20][128];
	std::string	m_AIServerIP;
	uint32		m_AIServerPort;

	NpcArray							m_arNpcArray;
	ZoneArray							m_ZoneArray;
	ItemtableArray						m_ItemtableArray;
	SetItemArray						m_SetItemArray;
	MagictableArray						m_MagictableArray;
	Magictype1Array						m_Magictype1Array;
	Magictype2Array						m_Magictype2Array;
	Magictype3Array						m_Magictype3Array;
	Magictype4Array						m_Magictype4Array;
	Magictype5Array						m_Magictype5Array;
	Magictype6Array						m_Magictype6Array;
	Magictype7Array						m_Magictype7Array;
	Magictype8Array						m_Magictype8Array;
	Magictype9Array						m_Magictype9Array;
	CoefficientArray					m_CoefficientArray;
	LevelUpArray						m_LevelUpArray;
	PartyArray							m_PartyArray;
	KnightsArray						m_KnightsArray;
	KnightsRatingArray					m_KnightsRatingArray[2]; // one for both nations
	KnightsAllianceArray				m_KnightsAllianceArray;
	KnightsSiegeWarfareArray			m_KnightsSiegeWarfareArray;
	KnightsCapeArray					m_KnightsCapeArray;
	UserNameRankMap						m_UserPersonalRankMap;
	UserNameRankMap						m_UserKnightsRankMap;
	UserRankMap							m_playerRankings[2]; // one for both nations
	std::recursive_mutex				m_userRankingsLock;
	StartPositionArray					m_StartPositionArray;
	ServerResourceArray					m_ServerResourceArray;
	QuestHelperArray					m_QuestHelperArray;
	QuestNpcList						m_QuestNpcList;
	QuestMonsterArray					m_QuestMonsterArray;
	RentalItemArray						m_RentalItemArray;
	ItemExchangeArray					m_ItemExchangeArray;
	ItemUpgradeArray					m_ItemUpgradeArray;
	ItemOpArray							m_ItemOpArray;
	ExpirationArray						m_ExpirationArray;
	KingSystemArray						m_KingSystemArray;
	EventTriggerArray					m_EventTriggerArray;
	MonsterChallengeArray				m_MonsterChallengeArray;
	MonsterChallengeSummonListArray		m_MonsterChallengeSummonListArray;
	MonsterSummonListArray				m_MonsterSummonList;
	MonsterSummonListZoneArray			m_MonsterSummonListZoneArray;
	MonsterRespawnListArray				m_MonsterRespawnListArray;
	MonsterRespawnListInformationArray	m_MonsterRespawnListInformationArray;
	PremiumItemArray					m_PremiumItemArray;
	PremiumItemExpArray					m_PremiumItemExpArray;
	UserRankingArray					m_UserRankingArray[2];
	UserDailyOpMap						m_UserDailyOpMap;
	TempleEventUserArray				m_TempleEventUserArray;
	NationMonumentInformationArray		m_NationMonumentInformationArray;
	StartPositionRandomArray			m_StartPositionRandomArray;
	UserItemArray						m_UserItemArray;
	ObjectEventArray					m_ObjectEventArray;

	Atomic<uint16>				m_sPartyIndex;
	short	m_sZoneCount;							// AI Server �����ӽ� ����

	bool	m_bFirstServerFlag;		// ������ ó�������� �� ���Ӽ����� ���� ���쿡�� 1, ���� ���� ���� 0
	bool	m_bServerCheckFlag;
	bool	m_bPointCheckFlag;		// AI������ �������� NPC������ �������� (true:������ ����, false:������ ���� ����)
	short   m_sErrorSocketCount;  // �̻����� ���ÿ�

	uint16 m_sYear, m_sMonth, m_sDate, m_sHour, m_sMin;
	uint8 m_byWeather;
	uint16 m_sWeatherAmount;
	int m_nCastleCapture;
	uint8 m_ReloadKnightAndUserRanksMinute;

	uint8   m_byBattleOpen, m_byOldBattleOpen;
	uint8	m_byBattleZone, m_byBattleZoneType, m_bVictory, m_byOldVictory, m_bResultDelayVictory, m_bKarusFlag, m_bElmoradFlag, m_bMiddleStatueNation;
	int32	m_byBattleOpenedTime;
	int32	m_byBattleTime;
	int32	m_byBattleRemainingTime;
	int32	m_sBattleTimeDelay;
	int32	m_sBattleResultDelay;
	int32	m_byBattleSiegeWarStartTime;
	uint8	m_SiegeWarNoticeTime;
	//visualdev war notice
	uint8	m_WarNoticeTime;
	uint8	m_SnowWarNoticeTime;
	// end
	std::string	m_SiegeWarWinKnightsNotice;
	bool	m_byBattleSiegeWarOpen;
	bool	m_byBattleSiegeWarMomument;
	bool	m_byBattleSiegeWarTeleport;
	uint8	m_sKilledKarusNpc, m_sKilledElmoNpc;
	uint8	m_sKarusMonuments, m_sElmoMonuments;
	uint16	m_sKarusMonumentPoint, m_sElmoMonumentPoint;											//snow war visualdev
	bool    m_byKarusOpenFlag, m_byElmoradOpenFlag, m_byBanishFlag, m_byBattleSave, m_bResultDelay,m_bySnowBanishFlag;
	short   m_sDiscount;
	short	m_sKarusDead, m_sElmoradDead, m_sBanishDelay, m_sKarusCount, m_sElmoradCount;
	std::string m_sBattleZoneOpenDays;
	// snow war visualdev
	std::string	m_sBattleZoneSnowOpenDays;
	//end
	uint8	m_nBattleZoneOpenHourStart[WAR_TIME_COUNT], m_nBattlezoneOpenWarZone[WAR_ZONE_COUNT];

	// snow war visualdev
	uint8	m_nBattleZoneOpenHourSnowStart[SNOW_TIME_COUNT], m_nBattlezoneOpenSnowWarZone[SNOW_ZONE_COUNT];
	//end
	std::string m_strKarusCaptain, m_strElmoradCaptain;

	uint8   m_nBorderDefenseWarTime[BORDER_DEFENSE_WAR_EVENT_COUNT], m_nChaosTime[CHAOS_EVENT_COUNT];
	uint32	m_nBorderDefenseWarPrizeWonItemNo1[BORDER_DEFENSE_WAR_EVENT_COUNT];
	uint32	m_nBorderDefenseWarPrizeWonItemNo2[BORDER_DEFENSE_WAR_EVENT_COUNT];
	uint32	m_nBorderDefenseWarPrizeWonItemNo3[BORDER_DEFENSE_WAR_EVENT_COUNT];
	uint32	m_nBorderDefenseWarPrizeWonItemNo4_K[BORDER_DEFENSE_WAR_EVENT_COUNT];
	uint32	m_nBorderDefenseWarPrizeWonItemNo4_H[BORDER_DEFENSE_WAR_EVENT_COUNT];
	uint32	m_nBorderDefenseWarPrizeWonLoyalty[BORDER_DEFENSE_WAR_EVENT_COUNT];
	uint32	m_nBorderDefenseWarPrizeWonExp[BORDER_DEFENSE_WAR_EVENT_COUNT];
	uint32	m_nBorderDefenseWarPrizeLoserLoyalty[BORDER_DEFENSE_WAR_EVENT_COUNT];
	uint32	m_nBorderDefenseWarPrizeLoserItem[BORDER_DEFENSE_WAR_EVENT_COUNT];
	uint32	m_nBorderDefenseWarPrizeLoserExp[BORDER_DEFENSE_WAR_EVENT_COUNT];
	uint32	m_nBorderDefenseWarMAXLEVEL[BORDER_DEFENSE_WAR_EVENT_COUNT];
	uint32	m_nBorderDefenseWarMINLEVEL[BORDER_DEFENSE_WAR_EVENT_COUNT];


	uint8	m_nPVPMonumentNation[MAX_ZONE_ID];

	uint32	m_GameServerPort;
	int32	m_nGameMasterRHitDamage;
	uint8	m_nPlayerRankingResetTime;
	std::string	m_sPlayerRankingsRewardZones;
	uint32	m_nPlayerRankingKnightCashReward;
	uint32  m_nPlayerRankingLoyaltyReward;

	uint16	m_nTempleEventRemainSeconds;

	uint8	m_bMaxRegenePoint;

	bool	m_bPermanentChatMode;
	std::string	m_strPermanentChat;

	uint8	m_bSantaOrAngel;
	uint8	m_sRankResetHour;

	// National Points Settings
	int m_Loyalty_Ardream_Source;
	int m_Loyalty_Ardream_Target;
	int m_Loyalty_Ronark_Land_Base_Source;
	int m_Loyalty_Ronark_Land_Base_Target;
	int m_Loyalty_Ronark_Land_Source;
	int m_Loyalty_Ronark_Land_Target;
	int m_Loyalty_Other_Zone_Source;
	int m_Loyalty_Other_Zone_Target;

	uint32 m_Loyalty_Clan_G4;
	uint32 m_Loyalty_Clan_G3;
	uint32 m_Loyalty_Clan_G2;
	uint32 m_Loyalty_Clan_G1;

	// Bifrost
	uint8   m_nBifrostTime[BIFROST_EVENT_COUNT];

	uint8 m_BifrostVictory;
	uint16 m_sBifrostRemainingTime;
	uint16 m_sBifrostTime;
	bool m_sBifrostWarStart;
	uint8 m_sBifrostVictoryAll;
	uint8 m_sBifrostVictoryNoticeAll;

	uint16 m_xBifrostRemainingTime;
	uint16 m_xJoinOtherNationBifrostTime;
	uint16 m_xBifrostTime;
	uint16 m_xBifrostMonumentAttackTime;
	bool m_bAttackBifrostMonument;

	void SendEventRemainingTime(bool bSendAll = false, CUser *pUser = nullptr, uint8 ZoneID = 0);
	uint16 CalcEventRemainingTime();

	bool m_IsMagicTableInUpdateProcess;
	bool m_IsPlayerRankingUpdateProcess;


	// Forgetten Temple
	std::vector<uint16>				m_nForgettenTempleUsers;
	bool							m_bForgettenTempleIsActive;
	int8							m_nForgettenTempleStartHour;
	int8							m_nForgettenTempleLevelMin;
	int8							m_nForgettenTempleLevelMax;
	int32							m_nForgettenTempleStartTime;
	uint8							m_nForgettenTempleChallengeTime;
	bool							m_bForgettenTempleSummonMonsters;
	uint8							m_nForgettenTempleCurrentStage;
	uint8							m_nForgettenTempleLastStage;
	ForgettenTempleMonsterList		m_ForgettenTempleMonsterList;
	uint32							m_nForgettenTempleLastSummonTime;
	bool							m_nForgettenTempleBanishFlag;
	uint8							m_nForgettenTempleBanishTime;

	// zone server info
	int					m_nServerNo, m_nServerGroupNo;
	int					m_nServerGroup;	// server�� ��ȣ(0:�������� ����, 1:����1��, 2:����2��)
	ServerArray			m_ServerArray;
	ServerArray			m_ServerGroupArray;

	NameMap		m_accountNameMap,
		m_characterNameMap;

	std::recursive_mutex	m_accountNameLock,
		m_characterNameLock,
		m_questNpcLock;

	// Controlled weather events set by Kings
	uint8 m_byKingWeatherEvent;
	uint8 m_byKingWeatherEvent_Day;
	uint8 m_byKingWeatherEvent_Hour;
	uint8 m_byKingWeatherEvent_Minute, m_bIsPKServer;
	std::string m_bServerName;
	// XP/coin/NP events
	uint8 m_byNPEventAmount, m_byExpEventAmount, m_byCoinEventAmount,m_byPKZoneExpEventAmount,Reskillmoney,Restatmoney;
	uint8 m_notice1,m_notice2,m_notice3;

	uint8 MAXLVLINI;
	bool CZBowlAktifMi;
	uint32 CzBowlItem;
	uint16 CzBowlItemCount;
	uint32 CzBowlItemTimeSec,CzBowlReloadTimeSec;
	uint32 CzBowlKC,CzBowlKCTimeSec,CzBowlNP,CzBowlNpTimeSec;

	// EXP 2, EXP 10 , EXP 50, EXP 100, EXP 500, EXP 1000, EXP TOTAL
	int m_byExpx2, m_byExpx10, m_byExpx50, m_byExpx100, m_byExpx500, m_byExpx1000, m_byExpTotal;
	// Gold 2, Gold 10 , Gold 50, Gold 100, Gold 500, Gold 1000, Gold TOTAL
	int m_byGoldx2, m_byGoldx10, m_byGoldx50, m_byGoldx100, m_byGoldx500, m_byGoldx1000, m_byGoldTotal;

	INLINE CLuaEngine * GetLuaEngine() { return &m_luaEngine; }

	KOSocketMgr<CUser> m_socketMgr;
	ClientSocketMgr<CAISocket> m_aiSocketMgr;

	FILE *m_fpDeathUser;
	FILE *m_fpDeathNpc;
	FILE *m_fpKnightCash;
	FILE *m_fpChat;
	FILE *m_fpCheat;
	FILE *m_fpMerchant;
	FILE *m_fpTrade;

	void WriteDeathUserLogFile(std::string & logMessage);
	void WriteMerchantUserLogFile(std::string & logMessage);
	void WriteTradeUserLogFile(std::string & logMessage);
	void WriteDeathNpcLogFile(std::string & logMessage);
	void WriteChatLogFile(std::string & logMessage);
	void WriteCheatLogFile(std::string & logMessage);

private:
	CLuaEngine	m_luaEngine;

	std::string m_strGameDSN, m_strAccountDSN;
	std::string m_strGameUID, m_strAccountUID;
	std::string m_strGamePWD, m_strAccountPWD;
	bool m_bMarsEnabled;

	bool ProcessServerCommand(std::string & command);

public:
	char * Command1, * Command11, * Command12, * Command13, * Command10, * Command14, * Command15, * Command16, * Command17, * Command18, * Command19;
	char * Command2, * Command21, * Command22, * Command23, * Command20, * Command24, * Command25, * Command26, * Command27, * Command28, * Command29;
	char * Command3, * Command31, * Command32, * Command33, * Command30, * Command34, * Command35, * Command36, * Command37, * Command38, * Command39;
	char * Command40, *Command41,*Command42,*Command43;
	char * Command4, * Command5,* Command6,* Command7,* Command8,* Command9;
	std::string Cmd1;
	void InitServerCommands();
	void CleanupServerCommands();

	static ServerCommandTable s_commandTable;

	COMMAND_HANDLER(HandleNoticeCommand);
	COMMAND_HANDLER(HandleKillUserCommand);
	COMMAND_HANDLER(HandleWar1OpenCommand);

	COMMAND_HANDLER(HandleWar2OpenCommand);
	COMMAND_HANDLER(HandleWar3OpenCommand);
	COMMAND_HANDLER(HandleWar4OpenCommand);
	COMMAND_HANDLER(HandleWar5OpenCommand);
	COMMAND_HANDLER(HandleWar6OpenCommand);
	COMMAND_HANDLER(HandleSnowWarOpenCommand);
	COMMAND_HANDLER(HandleSiegeWarOpenCommand);
	COMMAND_HANDLER(HandleWarCloseCommand);
	COMMAND_HANDLER(HandleShutdownCommand);
	COMMAND_HANDLER(HandleDiscountCommand);
	COMMAND_HANDLER(HandleGlobalDiscountCommand);
	COMMAND_HANDLER(HandleDiscountOffCommand);
	COMMAND_HANDLER(HandleCaptainCommand);
	COMMAND_HANDLER(HandleSantaCommand);
	COMMAND_HANDLER(HandleSantaOffCommand);
	COMMAND_HANDLER(HandleAngelCommand);
	COMMAND_HANDLER(HandlePermanentChatCommand);
	COMMAND_HANDLER(HandlePermanentChatOffCommand);
	COMMAND_HANDLER(HandleReloadNoticeCommand);
	COMMAND_HANDLER(HandleReloadTablesCommand);
	COMMAND_HANDLER(HandleReloadMagicsCommand);
	COMMAND_HANDLER(HandleReloadQuestCommand);
	COMMAND_HANDLER(HandleReloadRanksCommand);
	COMMAND_HANDLER(HandleCountCommand);
	COMMAND_HANDLER(HandlePermitConnectCommand);	
	COMMAND_HANDLER(HandleNoPermitConnectCommand);
	COMMAND_HANDLER(HandleBannedAccountCommand);
	COMMAND_HANDLER(HandleUnBannedAccountCommand);
	COMMAND_HANDLER(HandleAccessGMCommand);
	COMMAND_HANDLER(HandleRemoveGMCommand);
	COMMAND_HANDLER(HandleWarResultCommand);
	COMMAND_HANDLER(HandleExpChangeCommand);
	COMMAND_HANDLER(HandleTeleportAllCommand);
	COMMAND_HANDLER(HandleKnightsSummonCommand);
	COMMAND_HANDLER(HandleLoyaltyChangeCommand);
	COMMAND_HANDLER(HandleGoldChangeCommand);
	COMMAND_HANDLER(HandleLoyaltyAddCommand);
	COMMAND_HANDLER(HandleExpAddCommand);
	COMMAND_HANDLER(HandleMoneyAddCommand);
	COMMAND_HANDLER(HandleNoticeallCommand);
	COMMAND_HANDLER(HandlePmCommand);
	COMMAND_HANDLER(HandleClanCommand);
	COMMAND_HANDLER(HandleAllyCommand);
	COMMAND_HANDLER(HandleCapCommand);
	COMMAND_HANDLER(HandlePartCommand);
	COMMAND_HANDLER(HandleIpBannedCommand);
	COMMAND_HANDLER(HandleItemKnightsCommand);
	COMMAND_HANDLER(HandleItemOnlineCommand);

};

extern CGameServerDlg * g_pMain;

#endif
