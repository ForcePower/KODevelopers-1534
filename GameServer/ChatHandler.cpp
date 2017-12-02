#include "stdafx.h"
#include "DBAgent.h"
#include "../shared/DateTime.h"

using std::string;

ServerCommandTable CGameServerDlg::s_commandTable;
ChatCommandTable CUser::s_commandTable;
void CGameServerDlg::InitServerCommands()
{
	static Command<CGameServerDlg> commandTable[] = 
	{
		// Command				Handler											Help message
		{ "notice",				&CGameServerDlg::HandleNoticeCommand,			"Sends a server-wide chat notice." },
		{ "kill",				&CGameServerDlg::HandleKillUserCommand,			"Disconnects the specified player" },
		{ "open1",				&CGameServerDlg::HandleWar1OpenCommand,			"Opens war zone 1" },
		{ "open2",				&CGameServerDlg::HandleWar2OpenCommand,			"Opens war zone 2" },
		{ "open3",				&CGameServerDlg::HandleWar3OpenCommand,			"Opens war zone 3" },
		{ "open4",				&CGameServerDlg::HandleWar4OpenCommand,			"Opens war zone 4" },
		{ "open5",				&CGameServerDlg::HandleWar5OpenCommand,			"Opens war zone 5" },
		{ "open6",				&CGameServerDlg::HandleWar6OpenCommand,			"Opens war zone 6" },
		{ "snowopen",			&CGameServerDlg::HandleSnowWarOpenCommand,		"Opens the snow war zone" },
		{ "siegewarfare",		&CGameServerDlg::HandleSiegeWarOpenCommand,		"Opens the Castle Siege War zone" },
		{ "close",				&CGameServerDlg::HandleWarCloseCommand,			"Closes the active war zone" },
		{ "down",				&CGameServerDlg::HandleShutdownCommand,			"Shuts down the server" },
		{ "discount",			&CGameServerDlg::HandleDiscountCommand,			"Enables server discounts for the winning nation of the last war" },
		{ "alldiscount",		&CGameServerDlg::HandleGlobalDiscountCommand,	"Enables server discounts for everyone" },
		{ "offdiscount",		&CGameServerDlg::HandleDiscountOffCommand,		"Disables server discounts" },
		{ "captain",			&CGameServerDlg::HandleCaptainCommand,			"Sets the captains/commanders for the war" },
		{ "santa",				&CGameServerDlg::HandleSantaCommand,			"Enables a flying Santa Claus." },
		{ "offsanta",			&CGameServerDlg::HandleSantaOffCommand,			"Disables a flying Santa Claus/angel." },
		{ "angel",				&CGameServerDlg::HandleAngelCommand,			"Enables a flying angel." },
		{ "offangel",			&CGameServerDlg::HandleSantaOffCommand,			"Disables a flying Santa Claus/angel." },
		{ "permanent",			&CGameServerDlg::HandlePermanentChatCommand,	"Sets the permanent chat bar to the specified text." },
		{ "offpermanent",		&CGameServerDlg::HandlePermanentChatOffCommand,	"Resets the permanent chat bar text." },
		{ "reload_notice",		&CGameServerDlg::HandleReloadNoticeCommand,		"Reloads the in-game notice list." },
		{ "reload_tables",		&CGameServerDlg::HandleReloadTablesCommand,		"Reloads the in-game tables." },
		{ "reload_magics",		&CGameServerDlg::HandleReloadMagicsCommand,		"Reloads the in-game magic tables." },
		{ "reload_quests",		&CGameServerDlg::HandleReloadQuestCommand,		"Reloads the in-game quest tables." },
		{ "reload_ranks",		&CGameServerDlg::HandleReloadRanksCommand,		"Reloads the in-game rank tables." },
		{ "count",				&CGameServerDlg::HandleCountCommand,			"Get online user count." },
		{ "permitconnect",		&CGameServerDlg::HandlePermitConnectCommand,	"Player unban" },
		{ "warresult",			&CGameServerDlg::HandleWarResultCommand,		"Set result for War" },
		{ "exp_change",			&CGameServerDlg::HandleExpChangeCommand,		"Change a player an exp" },
		{ "tp_all",				&CGameServerDlg::HandleTeleportAllCommand,		"Players send to home zone." },
		{ "np_change",		    &CGameServerDlg::HandleLoyaltyChangeCommand,    "Change a player an loyalty" },
		{ "gold_change",		&CGameServerDlg::HandleGoldChangeCommand,       "Change a player an gold" },
		{ "np_add",			    &CGameServerDlg::HandleLoyaltyAddCommand,		"Sets the server-wide NP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "exp_add",			&CGameServerDlg::HandleExpAddCommand,			"Sets the server-wide XP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "money_add",			&CGameServerDlg::HandleMoneyAddCommand,			"Sets the server-wide coin event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ "noticeall",			&CGameServerDlg::HandleNoticeallCommand,		"Sends a server-wide chat notice." },
		{ "noticepm",			&CGameServerDlg::HandlePmCommand,			    "Sends a server-wide chat notice." },
		{ "noticeclan",			&CGameServerDlg::HandleClanCommand,			    "Sends a server-wide chat notice." },
		{ "noticeally",			&CGameServerDlg::HandleAllyCommand,			    "Sends a server-wide chat notice." },
		{ "noticecaptain",		&CGameServerDlg::HandleCapCommand,			    "Sends a server-wide chat notice." },
		{ "noticeparty",		&CGameServerDlg::HandlePartCommand,			    "Sends a server-wide chat notice." },

	};



	init_command_table(CGameServerDlg, commandTable, s_commandTable);
}

void CGameServerDlg::CleanupServerCommands() { free_command_table(s_commandTable); }

void CUser::InitChatCommands()
{
	static Command<CUser> commandTable[] = 
	{
		// Command				Handler											Help message
		{ g_pMain->Command1,	&CUser::HandleTestCommand,						"Test command" },
		{ g_pMain->Command2,	&CUser::HandleGiveItemCommand,					"Gives a player an item. Arguments: character name | item ID | [optional stack size]" },
		{ g_pMain->Command3,	&CUser::HandleZoneChangeCommand,				"Teleports you to the specified zone. Arguments: zone ID | eventroom" },
		{ g_pMain->Command4,	&CUser::HandleZoneCountCommand,					"Teleports you to the specified zone. Arguments: zone ID" },
		{ g_pMain->Command5,	&CUser::HandleMonsterSummonCommand,				"Spawns the specified monster (does not respawn). Arguments: monster's database ID" },
		{ g_pMain->Command6,	&CUser::HandleNPCSummonCommand,					"Spawns the specified NPC (does not respawn). Arguments: NPC's database ID" },
		{ g_pMain->Command7,	&CUser::HandleMonKillCommand,					"Kill a NPC or Monster, Arguments: select an Npc and monster than use this command" },
		{ g_pMain->Command8,	&CUser::HandleWar1OpenCommand,					"Opens war zone 1" },
		{ g_pMain->Command9,	&CUser::HandleWar2OpenCommand,					"Opens war zone 2" },
		{ g_pMain->Command10,	&CUser::HandleWar3OpenCommand,					"Opens war zone 3" },
		{ g_pMain->Command11,	&CUser::HandleWar4OpenCommand,					"Opens war zone 4" },
		{ g_pMain->Command12,	&CUser::HandleWar5OpenCommand,					"Opens war zone 5" },
		{ g_pMain->Command13,	&CUser::HandleWar6OpenCommand,					"Opens war zone 6" },
		{ g_pMain->Command14,	&CUser::HandleCaptainCommand,					"Sets the captains/commanders for the war" },
		{ g_pMain->Command15,	&CUser::HandleSnowWarOpenCommand,				"Opens the snow war zone" },
		{ g_pMain->Command16,	&CUser::HandleSiegeWarOpenCommand,				"Opens the Castle Siege War zone" },
		{ g_pMain->Command17,	&CUser::HandleWarCloseCommand,					"Closes the active war zone" },
		{ g_pMain->Command18,	&CUser::HandleLoyaltyChangeCommand,				"Change a player an loyalty" },
		{ g_pMain->Command19,	&CUser::HandleExpChangeCommand,					"Change a player an exp" },
		{ g_pMain->Command20,	&CUser::HandleGoldChangeCommand,				"Change a player an gold" },
		{ g_pMain->Command21,	&CUser::HandleLoyaltyAddCommand,				"Sets the server-wide NP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ g_pMain->Command22,	&CUser::HandleExpAddCommand,					"Sets the server-wide XP event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ g_pMain->Command23,	&CUser::HandleMoneyAddCommand,					"Sets the server-wide coin event. If bonusPercent is set to 0, the event is ended. Arguments: bonusPercent" },
		{ g_pMain->Command24,	&CUser::HandlePermitConnectCommand,				"Player unban" },
		{ g_pMain->Command25,	&CUser::HandleTeleportAllCommand,				"Players send to home zone." },
		{ g_pMain->Command26,	&CUser::HandleKnightsSummonCommand,				"Teleport the clan users. Arguments: clan name" },
		{ g_pMain->Command27,	&CUser::HandleWarResultCommand,					"Set result for War"},
		{ g_pMain->Command28,	&CUser::HandleResetPlayerRankingCommand,		"Reset player ranking. Arguments : Zone ID"},
		{ g_pMain->Command29,	&CUser::HandlePmCommand,			    "Sends a server-wide chat notice." },
		{ g_pMain->Command30,	&CUser::HandleClanCommand,			    "Sends a server-wide chat notice." },
		{ g_pMain->Command31,	&CUser::HandleAllyCommand,			    "Sends a server-wide chat notice." },
		{ g_pMain->Command32,	&CUser::HandleCapCommand,			    "Sends a server-wide chat notice." },
		{ g_pMain->Command33,	&CUser::HandlePartCommand,			    "Sends a server-wide chat notice." },
		{ g_pMain->Command34,	&CUser::HandleBDWRegisterCommand,			    "Sends a server-wide chat notice." },
		{ g_pMain->Command35,	&CUser::HandleNPtoKCCommand,			    "NP ile KC alabileceginiz sistem kullanim: +nptokc npmiktari" },
		{ g_pMain->Command36,	&CUser::HandleNoPermitConnectCommand,			"Player banned" },
		{ g_pMain->Command37,	&CUser::HandleBannedAccountCommand,				"account banned" },
		{ g_pMain->Command38,	&CUser::HandleUnBannedAccountCommand,			"client unbanned" },
		{ g_pMain->Command39,	&CUser::HandleAccessGMCommand,					"Player access GM" },
		{ g_pMain->Command40,	&CUser::HandleRemoveGMCommand,					"Player remove GM" },
		{ g_pMain->Command41,	&CUser::HandleIpBannedCommand,					"Player Ip banned" },
		{ g_pMain->Command42,	&CUser::HandleItemKnightsCommand,				"give item the clan users." },
		{ g_pMain->Command43,	&CUser::HandleItemOnlineCommand,				"give item the users online." },
	};

	init_command_table(CUser, commandTable, s_commandTable);
}

void CUser::CleanupChatCommands() { free_command_table(s_commandTable); }

void CUser::Chat(Packet & pkt)
{
	Packet result;
	uint16 sessID;
	uint8 type = pkt.read<uint8>(), bOutType = type, seekingPartyOptions, bNation;
	string chatstr, finalstr, strSender, * strMessage, chattype;
	CUser *pUser;
	CKnights * pKnights;
	DateTime time;

	if (GetLevel() < 20) 
	{
		Packet resultmer;
		std::string bufferpro = string_format("[Chat Message] must be at least 20 level"); 
		ChatPacket::Construct(&resultmer, 7, &bufferpro);
		Send(&resultmer);
		return;
	}

	bool isAnnouncement = false;

	if (isMuted() || (GetZoneID() == ZONE_PRISON && !isGM()))
		return;

	pkt >> chatstr;
	if (chatstr.empty() || chatstr.size() > 128)
		return;

	// Process GM commands
	if ( ProcessChatCommand(chatstr))
	{
		if (isGM()){
			chattype = "GAME MASTER";}
		else{chattype="USER";}
		g_pMain->WriteChatLogFile(string_format("[ %s - %d:%d:%d ] %s : %s ( Zone=%d, X=%d, Z=%d )\n",chattype.c_str(),time.GetHour(),time.GetMinute(),time.GetSecond(),GetName().c_str(),chatstr.c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
		return;
	}
	if (NpExchangeAsk)
	{
		NpExchangeAsk = 0;
		NpExchangeValue = 0;
		g_pMain->SendHelpDescription(this, string_format("Your np exchange request has cancelled. [%d] NP",NpExchangeValue));	 
	}

	if (type == SEEKING_PARTY_CHAT)
		pkt >> seekingPartyOptions;

	// Handle GM notice & announcement commands
	if (type == PUBLIC_CHAT || type == ANNOUNCEMENT_CHAT)
	{
		// Trying to use a GM command without authorisation? Bad player!
		if (!isGM())
			return;

		if (type == ANNOUNCEMENT_CHAT)
			type = WAR_SYSTEM_CHAT;

		bOutType = type;

		// This is horrible, but we'll live with it for now.
		// Pull the notice string (#### NOTICE : %s ####) from the database.
		// Format the chat string around it, so our chat data is within the notice
		g_pMain->GetServerResource(IDP_ANNOUNCEMENT, &finalstr, chatstr.c_str());
		isAnnouncement = true;
	}


	if (isAnnouncement)
	{
		// GM notice/announcements show no name, so don't bother setting it.
		strMessage = &finalstr; // use the formatted message from the user
		bNation = KARUS; // arbitrary nation
		sessID = -1;
	}
	else
	{
		strMessage = &chatstr; // use the raw message from the user
		strSender = GetName(); // everything else uses a name, so set it
		bNation = GetNation();
		sessID = GetSocketID();
	}

	//// GMs should use GM chat to help them stand out amongst players.
	//if (type == GENERAL_CHAT && isGM())
	//	bOutType = GM_CHAT;

	ChatPacket::Construct(&result, bOutType, strMessage, &strSender, bNation, sessID);

	switch (type) 
	{
	case GENERAL_CHAT:
		g_pMain->Send_NearRegion(&result, GetMap(), GetRegionX(), GetRegionZ(), GetX(), GetZ(), nullptr, GetEventRoom());
		chattype = "GENERAL_CHAT";
		break;

	case PRIVATE_CHAT:
		{
			pUser = g_pMain->GetUserPtr(m_sPrivateChatUser);
			if (pUser == nullptr || !pUser->isInGame()) 
				return;

			chattype = "PRIVATE_CHAT";
		}
	case COMMAND_PM_CHAT:
		{
			if (type == COMMAND_PM_CHAT && GetFame() != COMMAND_CAPTAIN)
				return;

			pUser = g_pMain->GetUserPtr(m_sPrivateChatUser);
			if (pUser != nullptr) 
				pUser->Send(&result);
		}
		break;

	case PARTY_CHAT:
		if (isInParty())
		{
			g_pMain->Send_PartyMember(GetPartyID(), &result);
			chattype = "PARTY_CHAT";
		}
		break;

	case SHOUT_CHAT:
		if (m_sMp < (m_iMaxMp / 5))
			break;

		//// Characters under level 35 require 3,000 coins to shout.
		//if (!isGM()
		//	&& GetLevel() < 35
		//	&& !GoldLose(SHOUT_COIN_REQUIREMENT))
		//	break;

		MSpChange(-(m_iMaxMp / 5));
		SendToRegion(&result, nullptr, GetEventRoom());
		chattype = "SHOUT_CHAT";
		break;

	case KNIGHTS_CHAT:
		if (isInClan())
		{
			pKnights = g_pMain->GetClanPtr(GetClanID());
			g_pMain->Send_KnightsMember(GetClanID(), &result);
			chattype = "KNIGHTS_CHAT";
		}
		break;
	case CLAN_NOTICE:
		if (isInClan() 
			&& isClanLeader())
		{
			pKnights = g_pMain->GetClanPtr(GetClanID());
			if (pKnights == nullptr)
				return;

			pKnights->UpdateClanNotice(chatstr);
		}
		break;
	case PUBLIC_CHAT:
	case ANNOUNCEMENT_CHAT:
		if (isGM())
			g_pMain->Send_All(&result);
		break;
	case COMMAND_CHAT:
		if (GetFame() == COMMAND_CAPTAIN)
		{
			g_pMain->Send_CommandChat(&result, m_bNation, this);
			chattype = "COMMAND_CHAT";
		}
		break;
	case MERCHANT_CHAT:
		if (isMerchanting())
			SendToRegion(&result);
		break;
	case ALLIANCE_CHAT:
		if (isInClan())
		{
			pKnights = g_pMain->GetClanPtr(GetClanID());
			if (pKnights != nullptr && pKnights->isInAlliance())
				g_pMain->Send_KnightsAlliance(pKnights->GetAllianceID(), &result);
			chattype = "ALLIANCE_CHAT";
		}
		break;
	case WAR_SYSTEM_CHAT:
		if (isGM())
			g_pMain->Send_All(&result);
		break;
	case SEEKING_PARTY_CHAT:
		if (m_bNeedParty == 2)
		{
			Send(&result);
			g_pMain->Send_Zone_Matched_Class(&result, GetZoneID(), this, GetNation(), seekingPartyOptions, GetEventRoom());
		}
		break;
	}

	if (!chattype.empty())
	{
		if (pUser && type == 2)
			g_pMain->WriteChatLogFile(string_format("[ %s - %d:%d:%d ] %s > %s : %s ( Zone=%d, X=%d, Z=%d )\n",chattype.c_str(),time.GetHour(),time.GetMinute(),time.GetSecond(),strSender.c_str(),pUser->GetName().c_str(),chatstr.c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
		else if (pKnights && (type == 6 || type == 15))
			g_pMain->WriteChatLogFile(string_format("[ %s - %d:%d:%d ] %s > %s : %s ( Zone=%d, X=%d, Z=%d )\n",chattype.c_str(),time.GetHour(),time.GetMinute(),time.GetSecond(),strSender.c_str(),pKnights->GetName().c_str(),chatstr.c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
		else
			g_pMain->WriteChatLogFile(string_format("[ %s - %d:%d:%d ] %s : %s ( Zone=%d, X=%d, Z=%d )\n",chattype.c_str(),time.GetHour(),time.GetMinute(),time.GetSecond(),strSender.c_str(),chatstr.c_str(),GetZoneID(),uint16(GetX()),uint16(GetZ())));
	}
}

void CUser::ChatTargetSelect(Packet & pkt)
{
	uint8 type = pkt.read<uint8>();

	// TODO: Replace this with an enum
	// Attempt to find target player in-game
	if (type == 1)
	{

		Packet result(WIZ_CHAT_TARGET, type);
		std::string strUserID;
		pkt >> strUserID;
		if (strUserID.empty() || strUserID.size() > MAX_ID_SIZE)
			return;

		CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
		if (pUser == nullptr || pUser == this)
			result << int16(0); 
		else if (pUser->isBlockingPrivateChat())
			result << int16(-1) << pUser->GetName();
		else
		{
			m_sPrivateChatUser = pUser->GetSocketID();
			result << int16(1) << pUser->GetName();
		}
		Send(&result);
	}
	else if (type == 3)
	{
		//Questions
		uint8 sSubType;
		uint8 sMessageLen;
		std::string sMessage;
		pkt >> sSubType >> sMessageLen >> sMessage;
	}
	// Allow/block PMs
	else
	{
		m_bBlockPrivateChat = pkt.read<bool>(); 
	}
}

/**
* @brief	Sends a notice to all users in the current zone
* 			upon death.
*
* @param	pKiller	The killer.
*/
void CUser::SendDeathNotice(Unit * pKiller, DeathNoticeType noticeType) 
{
	if (pKiller == nullptr)
		return;

	string buffer;
	if(noticeType == 0 || noticeType == 2)
		g_pMain->GetServerResource(IDS_DEATH_ANNOUNCEMENT, &buffer, pKiller->GetName().c_str(), GetName().c_str(), uint16(GetX()), uint16(GetZ()));
	else
		// g_pMain->GetServerResource(IDS_GUARD_TOWER_DEATH_ANNOUNCEMENT, &buffer, pKiller->GetName().c_str(), GetName().c_str());
		g_pMain->GetServerResource(IDS_GUARD_TOWER_DEATH_ANNOUNCEMENT, &buffer, GetName().c_str() , pKiller->GetName().c_str());

	string * strMessage = &buffer;
	Packet result;

	ChatPacket::Construct(&result, PUBLIC_CHAT, strMessage);
	g_pMain->Send_Zone(&result, GetZoneID(), nullptr, 0, GetEventRoom());
	//Packet result(WIZ_CHAT, uint8(DEATH_NOTICE));

	//result.SByte();
	//result	<< GetNation()
	//	<< uint8(noticeType)
	//	<< pKiller->GetID() // session ID?
	//	<< pKiller->GetName()
	//	<< GetID() // session ID?
	//	<< GetName()
	//	<< uint16(GetX()) << uint16(GetZ());

	//SendToZone(&result,this,pKiller->GetEventRoom(),(isInArena() ? RANGE_20M : 0.0f));
}

bool CUser::ProcessChatCommand(std::string & message)
{
	// Commands require at least 2 characters
	if (message.size() <= 1
		// If the prefix isn't correct
			|| message[0] != CHAT_COMMAND_PREFIX
			// or if we're saying, say, ++++ (faster than looking for the command in the map)
			|| message[1] == CHAT_COMMAND_PREFIX)
			// we're not a command.
			return false;

	// Split up the command by spaces
	CommandArgs vargs = StrSplit(message, " ");
	std::string command = vargs.front(); // grab the first word (the command)
	vargs.pop_front(); // remove the command from the argument list

	// Make the command lowercase, for 'case-insensitive' checking.
	STRTOLOWER(command);

	// Command doesn't exist
	ChatCommandTable::iterator itr = s_commandTable.find(command.c_str() + 1); // skip the prefix character
	if (itr == s_commandTable.end())
		return false;

	// Run the command
	return (this->*(itr->second->Handler))(vargs, message.c_str() + command.size() + 1, itr->second->Help);
}

long getFileSize(FILE *file)
{
	long lCurPos, lEndPos;
	lCurPos = ftell(file);
	fseek(file, 0, 2);
	lEndPos = ftell(file);
	fseek(file, lCurPos, 0);
	return lEndPos;
}

COMMAND_HANDLER(CUser::HandleTestCommand)
{
	if (!isGM())
		return false;

	uint8 WizCode = 0;
	int Type = 1;

	if (vargs.size() < 1)
	{
		g_pMain->SendHelpDescription(this, "Örnek : +sendfile WIZ_CODE [1|2] {Par1:WIZCODE , Par2: 1=SendAll - 2=Send - Default=Send}");
		return true;
	}

	if (vargs.size() == 1)
		WizCode = atoi(vargs.front().c_str());

	if (vargs.size() == 2)
	{
		WizCode = atoi(vargs.front().c_str());
		vargs.pop_front();
		Type = atoi(vargs.front().c_str());
	}

	const char *filePath = "sendf.bin"; 
	BYTE *fileBuf;   
	FILE *file = NULL;  

	if ((file = fopen(filePath, "rb")) == NULL)
		return true;
	long fileSize = getFileSize(file);
	fileBuf = new BYTE[fileSize];
	fread(fileBuf, fileSize, 1, file);

	Packet pckt(WizCode);
	for (int i = 0; i < fileSize; i++)
		pckt << fileBuf[i];

	if (Type == 1)
		this->Send(&pckt);
	else
		g_pMain->Send_All(&pckt);

	delete[]fileBuf;
	fclose(file);  

	return true;
}



COMMAND_HANDLER(CUser::HandleGiveItemCommand)
{
	if (!isGM())
		return false;

	// Char name | item ID | [stack size]
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +give_item CharacterName ItemID StackSize");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	uint32 nItemID = atoi(vargs.front().c_str());
	vargs.pop_front();
	_ITEM_TABLE *pItem = g_pMain->GetItemPtr(nItemID);
	if (pItem == nullptr)
	{
		// send error message saying the item does not exist
		g_pMain->SendHelpDescription(this, "Error : Item does not exist");
		return true;
	}

	uint16 sCount = 1;
	if (!vargs.empty())
	{
		sCount = atoi(vargs.front().c_str());
		vargs.pop_front();
		//codigo para avisar 	
		std::string sMonSummonNotice;
		sMonSummonNotice = string_format("Game Master '%s' has added the item '%s' in the server", GetName().c_str(),pItem->m_sName.c_str());
		g_pMain->SendAnnouncement(sMonSummonNotice.c_str());
	}

	uint32 Time = 0;
	if (!vargs.empty())
		Time = atoi(vargs.front().c_str());

	if (!pUser->GiveItem(nItemID, sCount, true, Time))
	{
		g_pMain->SendHelpDescription(this, "Error : Item could not be added");
	}
	return true;
}

COMMAND_HANDLER(CUser::HandlePmCommand)
{
	if (!isGM())
		return false;

	if (vargs.empty())
		return true;

	g_pMain->SendNoticePm(args);
	return true;
}

COMMAND_HANDLER(CUser::HandleClanCommand)
{
	if (!isGM())
		return false;

	if (vargs.empty())
		return true;

	g_pMain->SendNoticeClan(args);
	return true;
}

COMMAND_HANDLER(CUser::HandleAllyCommand)
{
	if (!isGM())
		return false;

	if (vargs.empty())
		return true;

	g_pMain->SendNoticeAlly(args);
	return true;
}

COMMAND_HANDLER(CUser::HandleCapCommand)
{
	if (!isGM())
		return false;

	if (vargs.empty())
		return true;

	g_pMain->SendNoticeCommand(args);
	return true;
}

COMMAND_HANDLER(CUser::HandlePartCommand)
{

	if (!isGM())
		return false;

	if (vargs.empty())
		return true;

	g_pMain->SendNoticePart(args);
	return true;
}

COMMAND_HANDLER(CUser::HandleZoneCountCommand)
{
	if (!isGM())
		return false;

	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +zonecount ZoneNumber");
		return true;
	}

	// Behave as in official (we'll fix this later)
	int nZoneID = atoi(vargs.front().c_str());


	uint16 tcount = 0;
	uint16 kacount = 0;
	uint16 elcount = 0;
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		if (TO_USER(itr->second)->isInGame()){
			if (TO_USER(itr->second)->GetZoneID() == nZoneID){
				tcount++;

				if(TO_USER(itr->second)->GetNation() == 1)
					kacount++;

				if(TO_USER(itr->second)->GetNation() == 2)
					elcount++;

			}
		}

	}

	Packet result(WIZ_CHAT);
	result << uint8(21) << uint16(nZoneID) << uint16(tcount) << uint16(kacount) << uint16(elcount);

	Send(&result);
	return true;
}


COMMAND_HANDLER(CUser::HandleZoneChangeCommand)
{
	if (!isGM())
		return false;

	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +zonechange ZoneNumber | EventRoom(If has)");
		return true;
	}

	// Behave as in official (we'll fix this later)
	int nZoneID = atoi(vargs.front().c_str());
	vargs.pop_front();
	uint16 m_bEventRoom = 0;
	if (!vargs.empty())
		m_bEventRoom = atoi(vargs.front().c_str());

	_START_POSITION * pStartPosition = g_pMain->GetStartPosition(nZoneID);
	if (pStartPosition == nullptr) 
		return false;

	SetUserEventRoom(m_bEventRoom);
	SetUnitEventRoom(m_bEventRoom);

	ZoneChange(nZoneID, 
		(float)(GetNation() == KARUS ? pStartPosition->sKarusX : pStartPosition->sElmoradX + myrand(0, pStartPosition->bRangeX)), 
		(float)(GetNation() == KARUS ? pStartPosition->sKarusZ : pStartPosition->sElmoradZ + myrand(0, pStartPosition->bRangeZ)));

	return true;
}

COMMAND_HANDLER(CUser::HandleMonsterSummonCommand)
{
	if (!isGM())
		return false;

	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +monsummon MonsterSID");
		return true;
	}
	int sSid = 0;
	uint16 sCount = 1;
	uint16 sRadius = 5;


	if (vargs.size() == 1)
	{
		sSid = atoi(vargs.front().c_str());
	}

	if (vargs.size() == 2)
	{
		sSid = atoi(vargs.front().c_str());
		vargs.pop_front();
		sCount = atoi(vargs.front().c_str());
	}
	
	if(vargs.size() == 3)
	{
		sSid = atoi(vargs.front().c_str());
		vargs.pop_front();
		sCount = atoi(vargs.front().c_str());
		vargs.pop_front();
		sRadius = atoi(vargs.front().c_str());
	}

	g_pMain->SpawnEventNpc(sSid, true, GetZoneID(), GetX(), GetY(), GetZ(),sCount,sRadius);

	std::string sZoneName;
	switch (GetZoneID())
	{
	case ZONE_KARUS: sZoneName = "Karus"; break;
	case ZONE_ELMORAD: sZoneName = "El morad"; break;
	case ZONE_KARUS_ESLANT: sZoneName = "Karus Eslant"; break;
	case ZONE_ELMORAD_ESLANT: sZoneName = "El morad Eslant"; break;
	case ZONE_MORADON: sZoneName = "Moradon"; break;
	case ZONE_DELOS: sZoneName = "Delos"; break;
	case ZONE_BIFROST: sZoneName = "Bifrost"; break;
	case ZONE_DESPERATION_ABYSS: sZoneName = "Desperation Abyss"; break;
	case ZONE_HELL_ABYSS: sZoneName = "Hell Abyss"; break;
	case ZONE_DRAGON_CAVE: sZoneName = "Dragon Cave"; break;
	case ZONE_ARENA: sZoneName = "Arena"; break;
	case ZONE_ORC_ARENA: sZoneName = "Orc Arena"; break;
	case ZONE_GOBLIN_ARENA: sZoneName = "Goblin Arena"; break;
	case ZONE_CAITHAROS_ARENA: sZoneName = "Caitharos Arena"; break;
	case ZONE_FORGOTTEN_TEMPLE: sZoneName = "Forgotten Temple"; break;
	case ZONE_RONARK_LAND: sZoneName = "Ronark Land"; break;
	default:
		break;
	}

	std::string sMonSummonNotice;
	sMonSummonNotice = string_format("%s has summoned %d monster in %s.", GetName().c_str(), sCount,sZoneName.c_str());
	g_pMain->SendAnnouncement(sMonSummonNotice.c_str());
	g_pMain->SendNotice(sMonSummonNotice.c_str());
	return true;
}

COMMAND_HANDLER(CUser::HandleNPCSummonCommand)
{
	if (!isGM())
		return false;

	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +npcsummon NPCSID");
		return true;
	}

	int sSid = atoi(vargs.front().c_str());
	g_pMain->SpawnEventNpc(sSid, false, GetZoneID(), GetX(), GetY(), GetZ());

	return true;
}

COMMAND_HANDLER(CUser::HandleMonKillCommand)
{
	if (!isGM())
		return false;

	if (GetTargetID() == 0 && GetTargetID() < NPC_BAND)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : Select a NPC or Monster than use +monkills");
		return false;
	}

	CNpc *pNpc = g_pMain->GetNpcPtr(GetTargetID());

	if (pNpc != nullptr)
		g_pMain->KillNpc(GetTargetID());

	return true;
}

bool CGameServerDlg::ProcessServerCommand(std::string & message)
{
	// Commands require at least 2 characters
	if (message.size() <= 1
		// If the prefix isn't correct
			|| message[0] != SERVER_COMMAND_PREFIX)
			// we're not a command.
			return false;

	// Split up the command by spaces
	CommandArgs vargs = StrSplit(message, " ");
	std::string command = vargs.front(); // grab the first word (the command)
	vargs.pop_front(); // remove the command from the argument list

	// Make the command lowercase, for 'case-insensitive' checking.
	STRTOLOWER(command);

	// Command doesn't exist
	ServerCommandTable::iterator itr = s_commandTable.find(command.c_str() + 1); // skip the prefix character
	if (itr == s_commandTable.end())
		return false;

	// Run the command
	return (this->*(itr->second->Handler))(vargs, message.c_str() + command.size() + 1, itr->second->Help);
}

COMMAND_HANDLER(CGameServerDlg::HandleNoticeCommand)
{
	if (vargs.empty())
		return true;

	SendNotice(args);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleNoticeallCommand)
{
	if (vargs.empty())
		return true;

	SendNoticeall(args);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandlePmCommand)
{
	if (vargs.empty())
		return true;

	SendNoticePm(args);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleClanCommand)
{
	if (vargs.empty())
		return true;

	SendNoticeClan(args);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleAllyCommand)
{
	if (vargs.empty())
		return true;

	SendNoticeAlly(args);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleCapCommand)
{
	if (vargs.empty())
		return true;

	SendNoticeCommand(args);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandlePartCommand)
{
	if (vargs.empty())
		return true;

	SendNoticePart(args);
	return true;
}




COMMAND_HANDLER(CGameServerDlg::HandleKillUserCommand)
{
	if (vargs.empty())
	{
		// send description
		printf("Using Sample : +kill CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	CUser *pUser = GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		printf("Error : User is not online\n");
		return true;
	}

	// Disconnect the player
	pUser->Disconnect();

	// send a message saying the player was disconnected
	return true;
}

COMMAND_HANDLER(CUser::HandleWar1OpenCommand) { return !isGM() ? false : g_pMain->HandleWar1OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar1OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 1);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar2OpenCommand) { return !isGM() ? false : g_pMain->HandleWar2OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar2OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 2);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar3OpenCommand) { return !isGM() ? false : g_pMain->HandleWar3OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar3OpenCommand)
{
	g_pMain->m_byBattleZoneType = ZONE_ARDREAM;
	BattleZoneOpen(BATTLEZONE_OPEN, 3);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar4OpenCommand) { return !isGM() ? false : g_pMain->HandleWar4OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar4OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 4);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar5OpenCommand) { return !isGM() ? false : g_pMain->HandleWar5OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar5OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 5);
	return true;
}

COMMAND_HANDLER(CUser::HandleWar6OpenCommand) { return !isGM() ? false : g_pMain->HandleWar6OpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWar6OpenCommand)
{
	BattleZoneOpen(BATTLEZONE_OPEN, 6);
	return true;
}

COMMAND_HANDLER(CUser::HandleSnowWarOpenCommand) { return !isGM() ? false : g_pMain->HandleSnowWarOpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleSnowWarOpenCommand)
{
	BattleZoneOpen(SNOW_BATTLEZONE_OPEN);
	return true;
}

COMMAND_HANDLER(CUser::HandleSiegeWarOpenCommand) { return !isGM() ? false : g_pMain->HandleSiegeWarOpenCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleSiegeWarOpenCommand)
{
	CastleSiegeWarZoneOpen(CLAN_BATTLE);
	return true;
}

COMMAND_HANDLER(CUser::HandleWarCloseCommand) { return !isGM() ? false : g_pMain->HandleWarCloseCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWarCloseCommand)
{
	BattleZoneClose();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleLoyaltyChangeCommand)
{

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{

		return true;
	}

	uint32 nLoyalty = atoi(vargs.front().c_str());

	if (nLoyalty != 0)
		pUser->SendLoyaltyChange(nLoyalty, false);

	return true;
}

COMMAND_HANDLER(CUser::HandleLoyaltyChangeCommand)
{
	if (!isGM())
		return false;

	// Char name | loyalty
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +np_change CharacterName Loyalty(+/-)");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	uint32 nLoyalty = atoi(vargs.front().c_str());

	if (nLoyalty != 0)
		pUser->SendLoyaltyChange(nLoyalty, false);

	return true;
}

COMMAND_HANDLER(CUser::HandleExpChangeCommand)
{
	if (!isGM())
		return false;

	// Char name | exp
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +exp_change CharacterName Exp(+/-)");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	int64 nExp = atoi(vargs.front().c_str());

	if (nExp != 0)
		pUser->ExpChange(nExp);

	return true;
}


COMMAND_HANDLER(CGameServerDlg::HandleGoldChangeCommand)
{

	// Char name | coins
	if (vargs.size() < 2)
	{
		// send description

		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{

		return true;
	}

	uint32 nGold = atoi(vargs.front().c_str());

	if (nGold != 0)
	{
		if (nGold > 0)
			pUser->GoldGain(nGold);
		else
			pUser->GoldLose(nGold);
	}

	return true;
}

COMMAND_HANDLER(CUser::HandleGoldChangeCommand)
{
	if (!isGM())
		return false;

	// Char name | coins
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +gold_change CharacterName Gold(+/-)");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : User is not online");
		return true;
	}

	uint32 nGold = atoi(vargs.front().c_str());

	if (nGold != 0)
	{
		if (nGold > 0)
			pUser->GoldGain(nGold);
		else
			pUser->GoldLose(nGold);
	}

	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleLoyaltyAddCommand)
{


	// Expects the bonus np percent, e.g. '+np_add' for a +15 np boost.
	if (vargs.empty())
	{
		// send description

		return true;
	}

	g_pMain->m_byNPEventAmount = (uint8) atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byNPEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_NP_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byNPEventAmount);
	return true;
}

COMMAND_HANDLER(CUser::HandleLoyaltyAddCommand)
{
	if (!isGM())
		return false;

	// Expects the bonus np percent, e.g. '+np_add' for a +15 np boost.
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +np_add Percent");
		return true;
	}

	g_pMain->m_byNPEventAmount = (uint8) atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	std::string Eventend;
	if (g_pMain->m_byNPEventAmount == 0)
	{
		Eventend = string_format("The increased National Point event has ended.");
		g_pMain->SendNoticeCommand(Eventend.c_str());
		g_pMain->SendChat<KNIGHTS_CHAT>(Eventend.c_str());
		return true;
	}
	else 
	{		
		g_pMain->SendFormattedResource(IDS_NP_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byNPEventAmount);
		return true;
	}
}

// Starts/stops the server XP event & sets its server-wide bonus.
COMMAND_HANDLER(CUser::HandleExpAddCommand)
{
	if (!isGM())
		return false;

	// Expects the bonus XP percent, e.g. '+exp_add' for a +15 XP boost.
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +exp_add Percent");
		return true;
	}

	g_pMain->m_byExpEventAmount = (uint8) atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byExpEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_EXP_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byExpEventAmount);
	return true;
}


COMMAND_HANDLER(CGameServerDlg::HandleExpAddCommand)
{


	// Expects the bonus XP percent, e.g. '+exp_add' for a +15 XP boost.
	if (vargs.empty())
	{
		// send description

		return true;
	}

	g_pMain->m_byExpEventAmount = (uint8) atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byExpEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_EXP_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byExpEventAmount);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleExpChangeCommand)
{

	// Char name | exp
	if (vargs.size() < 2)
	{
		// send description

		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	CUser *pUser = g_pMain->GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{

		return true;
	}

	int64 nExp = atoi(vargs.front().c_str());

	if (nExp != 0)
		pUser->ExpChange(nExp);

	return true;
}


COMMAND_HANDLER(CGameServerDlg::HandleMoneyAddCommand)
{


	// Expects the bonus coin percent, e.g. '+money_add' for a +15 dropped coin boost.
	if (vargs.empty())
	{
		// send description

		return true;
	}

	g_pMain->m_byCoinEventAmount = (uint8) atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byCoinEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_MONEY_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byCoinEventAmount);
	return true;
}


// Starts/stops the server coin event & sets its server-wide bonus.
COMMAND_HANDLER(CUser::HandleMoneyAddCommand)
{
	if (!isGM())
		return false;

	// Expects the bonus coin percent, e.g. '+money_add' for a +15 dropped coin boost.
	if (vargs.empty())
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +money_add Percent");
		return true;
	}

	g_pMain->m_byCoinEventAmount = (uint8) atoi(vargs.front().c_str());

	// Don't send the announcement if we're turning the event off.
	if (g_pMain->m_byCoinEventAmount == 0)
		return true;

	g_pMain->SendFormattedResource(IDS_MONEY_REPAY_EVENT, Nation::ALL, false, g_pMain->m_byCoinEventAmount);
	return true;
}
//visualdev GM
COMMAND_HANDLER(CUser::HandlePermitConnectCommand) { return !isGM() ? false : g_pMain->HandlePermitConnectCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandlePermitConnectCommand)
{
	// Char name
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +unban_char CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	g_DBAgent.UpdateUserAuthority(strUserID,AUTHORITY_PLAYER);

	std::string sNoticeMessage = string_format("%s is currently regular player.", strUserID.c_str());

	if (!sNoticeMessage.empty())
		g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);

	return true;
}

COMMAND_HANDLER(CUser::HandleNoPermitConnectCommand) { return !isGM() ? false : g_pMain->HandleNoPermitConnectCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleNoPermitConnectCommand)
{

	// Char name
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +ban_char CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	CUser *pUser = GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		std::string strUserID = vargs.front();
		vargs.pop_front();
		g_DBAgent.UpdateUserAuthority(strUserID,AUTHORITY_BANNED);
		std::string sNoticeMessage = string_format("%s is currently blocked for illegal activity.", strUserID.c_str());
		if (!sNoticeMessage.empty())
			g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);

	}
	else 
	{
		std::string strUserID = vargs.front();
		vargs.pop_front();
		pUser->m_bAuthority =AUTHORITY_BANNED;
		pUser->Disconnect();	
		std::string sNoticeMessage = string_format("%s is currently blocked for illegal activity.", strUserID.c_str());
		if (!sNoticeMessage.empty())
			g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);

	}
	return true;
}

COMMAND_HANDLER(CUser::HandleBannedAccountCommand) { return !isGM() ? false : g_pMain->HandleBannedAccountCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleBannedAccountCommand)
{

	// Char name
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +ban_account CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	CUser *pUser = GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		std::string strUserID = vargs.front();
		vargs.pop_front();
		g_DBAgent.AccountBanned(strUserID);
		std::string sNoticeMessage = string_format("%s your account is currently locked by illegal activity.", strUserID.c_str());
		if (!sNoticeMessage.empty())
			g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);

	}
	else 
	{
		std::string strUserID = vargs.front();
		vargs.pop_front();
		pUser->Disconnect();	
		g_DBAgent.AccountBanned(strUserID);
		std::string sNoticeMessage = string_format("%s your account is currently locked by illegal activity.", strUserID.c_str());
		if (!sNoticeMessage.empty())
			g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);

	}
	return true;
}
COMMAND_HANDLER(CUser::HandleUnBannedAccountCommand) { return !isGM() ? false : g_pMain->HandleUnBannedAccountCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleUnBannedAccountCommand)
{

	// Char name
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +unban_client CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	vargs.pop_front();

	g_DBAgent.AccountUnbanned(strUserID);

	std::string sNoticeMessage = string_format("%s your account is activated again.", strUserID.c_str());

	if (!sNoticeMessage.empty())
		g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);
	return true;
}

COMMAND_HANDLER(CUser::HandleAccessGMCommand) { return !isGM() ? false : g_pMain->HandleAccessGMCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleAccessGMCommand)
{


	// Char name
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +access_gm CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	CUser *pUser = GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		std::string strUserID = vargs.front();
		vargs.pop_front();
		g_DBAgent.UpdateUserAuthority(strUserID,AUTHORITY_GAME_MASTER);
		std::string sNoticeMessage = string_format("You're Trial GM Welcome to the staff ! [%s] ", strUserID.c_str());
		if (!sNoticeMessage.empty())
			g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);
	}
	else 
	{
		std::string strUserID = vargs.front();
		vargs.pop_front();
		pUser->m_bAuthority =AUTHORITY_GAME_MASTER;
		pUser->Disconnect();
		std::string sNoticeMessage = string_format("You're Trial GM Welcome to the staff ! [%s].", strUserID.c_str());
		if (!sNoticeMessage.empty())
			g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);
	}
	return true;
}

COMMAND_HANDLER(CUser::HandleRemoveGMCommand) { return !isGM() ? false : g_pMain->HandleRemoveGMCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleRemoveGMCommand)
{
	// Char name
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +remove_gm CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	CUser *pUser = GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		std::string strUserID = vargs.front();
		vargs.pop_front();
		g_DBAgent.UpdateUserAuthority(strUserID,AUTHORITY_PLAYER);
		std::string sNoticeMessage = string_format("The GM  [%s] has been removed staff.", strUserID.c_str());
		if (!sNoticeMessage.empty())
			g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);
	}
	else 
	{
		std::string strUserID = vargs.front();
		vargs.pop_front();
		pUser->m_bAuthority =AUTHORITY_PLAYER;
		pUser->Disconnect();
		std::string sNoticeMessage = string_format("The GM  [%s] has been removed staff.", strUserID.c_str());
		if (!sNoticeMessage.empty())
			g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);
	}
	return true;
}

COMMAND_HANDLER(CUser::HandleIpBannedCommand) { return !isGM() ? false : g_pMain->HandleIpBannedCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleIpBannedCommand)
{


	// Char name
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +ipban_char CharacterName\n");
		return true;
	}

	std::string strUserID = vargs.front();
	CUser *pUser = GetUserPtr(strUserID, TYPE_CHARACTER);
	if (pUser == nullptr)
	{
		std::string strUserID = vargs.front();
		vargs.pop_front();
		g_DBAgent.AccountIpBanned(strUserID);
		std::string sNoticeMessage = string_format("%s You're banned IP permanently by using illegal software.", strUserID.c_str());
		if (!sNoticeMessage.empty())
			g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);

	}
	else 
	{
		std::string strUserID = vargs.front();
		vargs.pop_front();
		pUser->Disconnect();	
		g_DBAgent.AccountIpBanned(strUserID);
		std::string sNoticeMessage = string_format("%s You're banned IP permanently by using illegal software.", strUserID.c_str());
		if (!sNoticeMessage.empty())
			g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);

	}
	return true;
}

// end

COMMAND_HANDLER(CGameServerDlg::HandleTeleportAllCommand)
{

	int nZoneID = 0;
	int nTargetZoneID = 0;

	if (vargs.size() == 1)
		nZoneID = atoi(vargs.front().c_str());

	if (vargs.size() == 2)
	{
		nZoneID = atoi(vargs.front().c_str());
		vargs.pop_front();
		nTargetZoneID = atoi(vargs.front().c_str());
	}

	if (nZoneID > 0 || nTargetZoneID > 0)
		g_pMain->KickOutZoneUsers(nZoneID,nTargetZoneID);

	return true;
}

COMMAND_HANDLER(CUser::HandleTeleportAllCommand)
{
	if (!isGM())
		return false;

	// Zone number
	if (vargs.size() < 1)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +tp_all ZoneNumber | +tp_all ZoneNumber TargetZoneNumber");
		return true;
	}

	int nZoneID = 0;
	int nTargetZoneID = 0;

	if (vargs.size() == 1)
		nZoneID = atoi(vargs.front().c_str());

	if (vargs.size() == 2)
	{
		nZoneID = atoi(vargs.front().c_str());
		vargs.pop_front();
		nTargetZoneID = atoi(vargs.front().c_str());
	}

	if (nZoneID > 0 || nTargetZoneID > 0)
		g_pMain->KickOutZoneUsers(nZoneID,nTargetZoneID);

	return true;
}
//sek
//visualdev item online
COMMAND_HANDLER(CUser::HandleItemOnlineCommand)
{
	// Char name
	if (vargs.size() < 2)
	{
		// send description
		g_pMain->SendHelpDescription(this, "Using Sample : +item_online ItemID");
		return true;
	}

	uint32 nItemID = atoi(vargs.front().c_str());
	vargs.pop_front();

	_ITEM_TABLE *pItem = g_pMain->GetItemPtr(nItemID);
	if (pItem == nullptr)
	{
		g_pMain->SendHelpDescription(this, "Error : Item does not exist");
		return true;
	}

	uint16 sCount = 1;
	if (!vargs.empty())
	{
		sCount = atoi(vargs.front().c_str());
		vargs.pop_front();
	}

	if (pItem != nullptr)
	{

		g_DBAgent.ItemOnline(nItemID,sCount);
		printf("\n %d",nItemID);
		std::string sNoticeMessage = string_format("Hello Knights, You been received a gift by UnixKO staff  press L to see.");
		if (!sNoticeMessage.empty())
			g_pMain->SendNotice(sNoticeMessage.c_str(),Nation::ALL);
		g_pMain->SendAnnouncement(sNoticeMessage.c_str(),Nation::ALL);
	}
	return true;

}
//visualdev clan item
COMMAND_HANDLER(CUser::HandleItemKnightsCommand)
{
	if (!isGM())
		return false;

	// Clan name
	if (vargs.size() < 3)
	{
		// Send description
		g_pMain->SendHelpDescription(this, "Using Sample : +clan_item ItemID StackSize  ClanName");
		return true;
	}

	uint32 nItemID = atoi(vargs.front().c_str());
	vargs.pop_front();
	_ITEM_TABLE *pItem = g_pMain->GetItemPtr(nItemID);
	if (pItem == nullptr)
	{
		printf("%d",pItem);
		g_pMain->SendHelpDescription(this, "Error : Item does not exist");
		return true;
	}
	printf("\n%d",pItem);
	///

	uint16 sCount = 1;
	if (!vargs.empty())
	{
		sCount = atoi(vargs.front().c_str());
		vargs.pop_front();
	}


	CKnights * pKnights;
	foreach_stlmap (itr,g_pMain->m_KnightsArray)
	{
		if(itr->second->GetName() == vargs.front().c_str())
		{
			pKnights = g_pMain->GetClanPtr(itr->first);
			break;
		}
	}

	if(pKnights == nullptr){
		g_pMain->SendHelpDescription(this, "Error : clan does not exist");
		return true;}

	//

	foreach_array(i,pKnights->m_arKnightsUser)
	{
		_KNIGHTS_USER *p = &pKnights->m_arKnightsUser[i];
		if (!p->byUsed || p->pSession == nullptr)
			continue;

		CUser* pUser = p->pSession;
		if(!pUser->isInGame() || pUser->GetName() == GetName())
			continue;

		pUser->GiveItem(nItemID, sCount);
		//codigo para avisar 
		Packet result3;
		std::string buffer4 = string_format("User %s of the %s has received '%s'",pUser->GetName().c_str(),pKnights->GetName().c_str(),pItem->m_sName.c_str()); 
		ChatPacket::Construct(&result3, 6, &buffer4);
		Send(&result3);
	}

	return true;
}
///
COMMAND_HANDLER(CUser::HandleKnightsSummonCommand)
{
	if (!isGM())
		return false;

	// Clan name
	if(vargs.empty())
	{
		// Send description
		g_pMain->SendHelpDescription(this, "Using Sample : +summonknights ClanName");
		return true;
	}

	CKnights * pKnights;
	foreach_stlmap (itr,g_pMain->m_KnightsArray)
	{
		if(itr->second->GetName() == vargs.front().c_str())
		{
			pKnights = g_pMain->GetClanPtr(itr->first);
			break;
		}
	}

	if(pKnights == nullptr){
		g_pMain->SendHelpDescription(this, "Error : clan does not exist");
		return true;}

	foreach_array(i,pKnights->m_arKnightsUser)
	{
		_KNIGHTS_USER *p = &pKnights->m_arKnightsUser[i];
		if (!p->byUsed || p->pSession == nullptr)
			continue;

		CUser* pUser = p->pSession;
		if(!pUser->isInGame() || pUser->GetName() == GetName())
			continue;

		pUser->ZoneChange(GetZoneID(), m_curx, m_curz);
	}

	return true;
}

COMMAND_HANDLER(CUser::HandleResetPlayerRankingCommand)
{
	if (!isGM())
		return false;

	// Zone ID
	if(vargs.empty())
	{
		// Send description
		g_pMain->SendHelpDescription(this, "Using Sample : +resetranking ZoneID");
		return true;
	}

	uint8 nZoneID;
	nZoneID = atoi(vargs.front().c_str());

	if (nZoneID > 0)
		g_pMain->ResetPlayerRankings(nZoneID);

	return true;
}

COMMAND_HANDLER(CUser::HandleBDWRegisterCommand)
{

	uint16 nActiveEvent = (uint16)g_pMain->pTempleEvent.ActiveEvent;
	if(nActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR && !g_pMain->pTempleEvent.isActive){

		if(g_pMain->pTempleEvent.m_nBorderDefenseWarMAXLEVEL < GetLevel() // if(g_pMain->pTempleEvent.m_nBorderDefenseWarMAXLEVEL < GetLevel()
			|| g_pMain->pTempleEvent.m_nBorderDefenseWarMINLEVEL > GetLevel())
		{
			g_pMain->SendColorfulNotice(this, string_format("You haven't registered to Border defense war, it's only available for (%d~%d) levels.",g_pMain->pTempleEvent.m_nBorderDefenseWarMINLEVEL,g_pMain->pTempleEvent.m_nBorderDefenseWarMAXLEVEL));
			return true;
		}



		if (isEventUser()){

			TempleOperations(TEMPLE_EVENT_DISBAND);
			g_pMain->SendColorfulNotice(this, "You have removed Border defense war register succesfully.");
			//printf("There are %d users registered to Border defense war.\n",g_pMain->pTempleEvent.AllUserCount);
			return true;
		}else{
			TempleEventJoin();
			g_pMain->SendColorfulNotice(this, "You have registered Border defense war succesfully.");
			//printf("There are %d users registered to Border defense war.\n",g_pMain->pTempleEvent.AllUserCount);

		}
	} 
	else if(nActiveEvent == TEMPLE_EVENT_BORDER_DEFENCE_WAR && g_pMain->pTempleEvent.isActive && GetZoneID() == ZONE_BORDER_DEFENSE_WAR){
		Packet resultmer;
		std::string bufferpro = string_format("[Event Message] Border Defance War Human: %d, Karus: %d, Remaining time: %d sec",g_pMain->pTempleEvent.KarusDeathCount[GetEventRoom()],g_pMain->pTempleEvent.ElmoDeathCount[GetEventRoom()],g_pMain->m_nTempleEventRemainSeconds); 
		ChatPacket::Construct(&resultmer, 7, &bufferpro);
		Send(&resultmer);
	}else{	
		g_pMain->SendColorfulNotice(this, "You cannot register Border defense war now.");
	}

	return true;
}

COMMAND_HANDLER(CUser::HandleNPtoKCCommand)
{
	if(!g_pMain->KCaktifmi 
		|| isMerchanting() 
		|| isStoreOpen() 
		|| isTrading()
		|| m_bMerchantStatex
		|| isDead())
		return true;

	if (vargs.size() < 1)
	{
		g_pMain->SendHelpDescription(this,"Example: +nptokc 1000");
		return true;
	}else if (vargs.size() > 1)
	{
		g_pMain->SendHelpDescription(this,"Example: +nptokc 1000");
		return true;

	}


	uint32 nLoyalty = atoi(vargs.front().c_str());

	if (nLoyalty > 1000000)
	{
		g_pMain->SendHelpDescription(this,"You can't exchange more than 1 million NP.");
		return true;
	}

	int32 CLoyalty;

	if (nLoyalty < 1)
	{
		g_pMain->SendHelpDescription(this,"You can't exchange this much NP.");
		return true;
	}

	if(GetLoyalty() < nLoyalty){
		g_pMain->SendHelpDescription(this,"You haven't got this much NP.");
		return true;
	}
	CLoyalty = nLoyalty;
	if (CLoyalty%1000!=0){
		g_pMain->SendHelpDescription(this,"You can only exchange 1000,2000 or x000 NP.");
		return true;
	}


	int32 ExchangeKC = (CLoyalty / 1000) * (g_pMain->KCmiktari);

	if (NpExchangeAsk && NpExchangeValue == CLoyalty)
	{
		g_pMain->SendHelpDescription(this,string_format("You earned [%d] Knight Cash!",ExchangeKC));
		SendLoyaltyChange(-CLoyalty,false,false,false);
		g_DBAgent.UpdateAccountKnightCash(GetAccountName(),ExchangeKC);
		g_DBAgent.UpdateUser(GetName(),UPDATE_LOGOUT,this);
	}
	else
	{
		g_pMain->SendHelpDescription(this,string_format("Do you really want to give [%d] loyalty for earn [%d] Knight Cash?",CLoyalty,ExchangeKC));
		g_pMain->SendHelpDescription(this,string_format("Rechat <+nptokc %d> to earn [%d] Knight Cash.",CLoyalty,ExchangeKC));
		NpExchangeAsk = 1;
		NpExchangeValue = CLoyalty;
		return true;
	}

	return true;
}


COMMAND_HANDLER(CGameServerDlg::HandleShutdownCommand)
{
	printf("Server shutdown, %d users kicked out.\n", KickOutAllUsers());
	m_socketMgr.Shutdown();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleDiscountCommand)
{
	m_sDiscount = 1;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleGlobalDiscountCommand)
{
	m_sDiscount = 2;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleDiscountOffCommand)
{
	m_sDiscount = 0;
	return true;
}

COMMAND_HANDLER(CUser::HandleCaptainCommand) { return !isGM() ? false : g_pMain->HandleCaptainCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleCaptainCommand)
{
	m_KnightsRatingArray[KARUS_ARRAY].DeleteAllData();
	m_KnightsRatingArray[ELMORAD_ARRAY].DeleteAllData();
	LoadKnightsRankTable(true, true);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleSantaCommand)
{
	m_bSantaOrAngel = FLYING_SANTA;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleSantaOffCommand)
{
	m_bSantaOrAngel = FLYING_NONE;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleAngelCommand)
{
	m_bSantaOrAngel = FLYING_ANGEL;
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandlePermanentChatCommand)
{
	if (vargs.empty())
	{
		// send error saying we need args (unlike the previous implementation of this command)
		return true;
	}

	SetPermanentMessage("%s", args);
	return true;
}


COMMAND_HANDLER(CUser::HandleWarResultCommand) { return !isGM() ? false : g_pMain->HandleWarResultCommand(vargs, args, description); }
COMMAND_HANDLER(CGameServerDlg::HandleWarResultCommand)
{
	// Nation number
	if (vargs.size() < 1)
	{
		// send description
		printf("Using Sample : +warresult 1/2 (KARUS/HUMAN)\n");
		return true;
	}

	if (!isWarOpen())
	{
		// send description
		printf("Warning : Battle is not open.\n");
		return true;
	}

	uint8 winner_nation;
	winner_nation = atoi(vargs.front().c_str());

	if (winner_nation > 0 && winner_nation < 3)
		BattleZoneResult(winner_nation);

	return true;
}

void CGameServerDlg::SendHelpDescription(CUser *pUser, std::string sHelpMessage)
{
	if (pUser == nullptr || sHelpMessage == "")
		return;

	Packet result(WIZ_CHAT, (uint8)PUBLIC_CHAT);
	result << pUser->GetNation() << pUser->GetSocketID() << (uint8)0 << sHelpMessage;
	pUser->Send(&result);
}

void CGameServerDlg::SendColorfulNotice(CUser *pUser, std::string sHelpMessage)
{
	if (pUser == nullptr || sHelpMessage == "")
		return;

	Packet result(WIZ_CHAT, (uint8)MERCHANT_CHAT);
	result << pUser->GetNation() << pUser->GetSocketID() << (uint8)0 << sHelpMessage;
	pUser->Send(&result);
}

void CGameServerDlg::SetPermanentMessage(const char * format, ...)
{
	char buffer[128];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buffer, 128, format, ap);
	va_end(ap);

	m_bPermanentChatMode = true;
	m_strPermanentChat = buffer;

	Packet result;
	ChatPacket::Construct(&result, PERMANENT_CHAT, &m_strPermanentChat);
	Send_All(&result);
}

COMMAND_HANDLER(CGameServerDlg::HandlePermanentChatOffCommand)
{
	Packet result;
	ChatPacket::Construct(&result, END_PERMANENT_CHAT);
	m_bPermanentChatMode = false;
	Send_All(&result);
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadNoticeCommand)
{
	// Reload the notice data
	LoadNoticeData();

	// and update all logged in players
	// if we're using the new notice format that's always shown
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		CUser * pUser = TO_USER(itr->second);
		if (pUser->isInGame())
			pUser->SendNotice();
	}
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadTablesCommand)
{
	m_StartPositionArray.DeleteAllData();
	LoadStartPositionTable();

	m_StartPositionRandomArray.DeleteAllData();
	LoadStartPositionRandomTable();

	m_ExpirationArray.DeleteAllData();
	LoadExpiration();

	/*m_ItemtableArray.DeleteAllData();
	LoadItemTable();*/

	m_ItemExchangeArray.DeleteAllData();
	LoadItemExchangeTable();

	m_ItemUpgradeArray.DeleteAllData();
	LoadItemUpgradeTable();

	m_EventTriggerArray.DeleteAllData();
	LoadEventTriggerTable();

	m_ServerResourceArray.DeleteAllData();
	LoadServerResourceTable();

	m_MonsterChallengeArray.DeleteAllData();
	LoadMonsterChallengeTable();

	m_MonsterChallengeSummonListArray.DeleteAllData();
	LoadMonsterChallengeSummonListTable();

	m_MonsterRespawnListArray.DeleteAllData();
	LoadMonsterRespawnListTable();

	m_MonsterRespawnListInformationArray.DeleteAllData();
	LoadMonsterRespawnListInformationTable();

	ReloadKnightAndUserRanks();

	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadMagicsCommand)
{
	m_IsMagicTableInUpdateProcess = true;
	m_MagictableArray.DeleteAllData();
	m_Magictype1Array.DeleteAllData();
	m_Magictype2Array.DeleteAllData();
	m_Magictype3Array.DeleteAllData();
	m_Magictype4Array.DeleteAllData();
	m_Magictype5Array.DeleteAllData();
	m_Magictype6Array.DeleteAllData();
	m_Magictype8Array.DeleteAllData();
	m_Magictype9Array.DeleteAllData();
	LoadMagicTable();
	LoadMagicType1();
	LoadMagicType2();
	LoadMagicType3();
	LoadMagicType4();
	LoadMagicType5();
	LoadMagicType6();
	LoadMagicType7();
	LoadMagicType8();
	LoadMagicType9();
	m_IsMagicTableInUpdateProcess = false;

	return true;
}


COMMAND_HANDLER(CGameServerDlg::HandleReloadQuestCommand)
{
	m_QuestHelperArray.DeleteAllData();
	LoadQuestHelperTable();
	m_QuestMonsterArray.DeleteAllData();
	LoadQuestMonsterTable();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleReloadRanksCommand)
{
	ReloadKnightAndUserRanks();
	return true;
}

COMMAND_HANDLER(CGameServerDlg::HandleCountCommand)
{
	uint16 count = 0;
	SessionMap sessMap = g_pMain->m_socketMgr.GetActiveSessionMap();
	foreach (itr, sessMap)
	{
		if (TO_USER(itr->second)->isInGame())
			count++;
	}

	printf("Online User Count : %d\n",count);
	return true;
}

void CGameServerDlg::SendFormattedResource(uint32 nResourceID, uint8 byNation, bool bIsNotice, ...)
{
	_SERVER_RESOURCE *pResource = m_ServerResourceArray.GetData(nResourceID);
	if (pResource == nullptr)
		return;

	string buffer;
	va_list ap;
	va_start(ap, bIsNotice);
	_string_format(pResource->strResource, &buffer, ap);
	va_end(ap);

	if (bIsNotice)
		SendNotice(buffer.c_str(), byNation);
	else
		SendAnnouncement(buffer.c_str(), byNation);
}
