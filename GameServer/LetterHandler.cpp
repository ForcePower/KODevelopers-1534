#include "stdafx.h"
#include "DBAgent.h"

using std::string;

void CUser::LetterSystem(Packet & pkt)
{
	uint8 opcode = pkt.read<uint8>();

	switch (opcode)
	{
	case LETTER_UNREAD:
	case LETTER_LIST:
	case LETTER_HISTORY:
	case LETTER_READ:
	case LETTER_GET_ITEM:
	case LETTER_SEND:
		break;
		//bi saniye
	case LETTER_DELETE:
		{
			uint8 bCount = pkt.read<uint8>();
			if (bCount > 5)
			{
				Packet result(WIZ_SHOPPING_MALL, uint8(STORE_LETTER));
				result << uint8(LETTER_DELETE) << int8(-3);
				Send(&result);
				return;
			}
		} break;

	default:
		TRACE("Unknown letter packet: %X\n", opcode);
		return;
	}

	g_pMain->AddDatabaseRequest(pkt, this);
}

void CUser::ReqLetterSystem(Packet & pkt)
{
	uint8 opcode = pkt.read<uint8>();
	// þu an item inventory e geliyor lettere alsana tekrar
	switch (opcode)
	{
		// Are there any letters to be read?
		// This is for the notification at the top of the screen.
	case LETTER_UNREAD:
		ReqLetterUnread();
		break;

		// Lists all the new mail.
	case LETTER_LIST:
		ReqLetterList();
		break;

		// Lists all the old mail.
	case LETTER_HISTORY:
		ReqLetterList(false);
		break;

		// Opens up the letter & marks it as read.
	case LETTER_READ:
		ReqLetterRead(pkt);
		break;

		// Used to send a letter & any coins/items (coins are disabled though)
	case LETTER_SEND:
		ReqLetterSend(pkt);
		break;

		// Used to take an item from a letter. 
	case LETTER_GET_ITEM:
		ReqLetterGetItem(pkt);
		break;

		// Deletes up to 5 old letters at a time.
	case LETTER_DELETE:
		ReqLetterDelete(pkt);
		break;
	}
}

void CUser::ReqLetterUnread()
{
	/*if(GetName() == "Th3Rogue")
	{
	printf("test");
	}*/
	// TODO: Force this to use cached list data (or update if stale). Calling the DB for just this is pointless.
	Packet result(WIZ_SHOPPING_MALL, uint8(STORE_LETTER));
	result	<< uint8(LETTER_UNREAD) 
		<< g_DBAgent.GetUnreadLetterCount(m_strUserID);
	Send(&result);
}

void CUser::ReqLetterList(bool bNewLettersOnly /*= true*/)
{

	Packet result(WIZ_SHOPPING_MALL, uint8(STORE_LETTER));
	result	<< uint8(bNewLettersOnly ? LETTER_LIST : LETTER_HISTORY);



	if (!g_DBAgent.GetLetterList(m_strUserID, result, bNewLettersOnly) || GetEmptySlot() < 1)
		result << int8(-1);

	//printf("letter result : %d\n",);//buraya gelemiyor aynn
	Send(&result);
}

void CUser::ReqLetterRead(Packet & pkt)
{

	Packet result(WIZ_SHOPPING_MALL, uint8(STORE_LETTER));
	uint32 nLetterID = pkt.read<uint32>();
	string strMessage;

	if (isMerchanting() || isTrading() || m_bMerchantStatex == 1)
	{
		Packet resultmer;
		std::string bufferpro = string_format("[Letter System]Please close the merchant to make this process!"); 
		ChatPacket::Construct(&resultmer,6, &bufferpro);
		Send(&resultmer);
		Packet resultmer4;
		std::string bufferpro4 = string_format("[Letter System]Please login again to get item!"); 
		ChatPacket::Construct(&resultmer4, 6, &bufferpro4);
		Send(&resultmer4);
		// TODO: research error codes
		result << uint8(0);
	}
	else

	{

		result << uint8(LETTER_READ);
		if (GetEmptySlot() < 1){
			result << uint8(0);
			Send(&result);
			return;
		}
		if (!g_DBAgent.ReadLetter(m_strUserID, nLetterID, strMessage))
		{
			// TODO: research error codes
			result << uint8(0);
		}
		else
		{
			result.SByte();
			result << uint8(1) << nLetterID << strMessage;
		}
		Send(&result);
	}
}
void CUser::ReqLetterSend(Packet & pkt)
{
	if (isMerchanting() || isTrading() || m_bMerchantStatex == 1)
	{
		Packet resultmer;
		std::string bufferpro = string_format("[Letter Message] Please try again to use this feature"); 
		ChatPacket::Construct(&resultmer,6, &bufferpro);
		Send(&resultmer);
	}
	else
	{

		Packet result(WIZ_SHOPPING_MALL, uint8(STORE_LETTER));
		CUser * pUser;
		string strRecipient, strSubject, strMessage;
		_ITEM_DATA *pItem = nullptr;
		uint32 nItemID = 0, nCoins = 0, nCoinRequirement = 1000;
		uint8 bType, bSrcPos;
		int8 bResult = 1;
		int64 Serial = 0;

		if (isMerchanting() || isTrading() || m_bMerchantStatex == 1)
		{
			bResult = -1;
			goto send_packet;
		}

		pkt.SByte();
		pkt >> strRecipient >> strSubject >> bType;

		// Invalid recipient name length
		if (strRecipient.empty() || strRecipient.length() > MAX_ID_SIZE
			// Invalid subject length
				|| strSubject.empty() || strSubject.length() > 31
				// Invalid type (as far as we're concerned)
				|| bType == 0 || bType > 2)
				bResult = -1;
		else if (STRCASECMP(m_strUserID.c_str(), strRecipient.c_str()) == 0)
			bResult = -6;

		if (bResult != 1)
			goto send_packet;

		if (bType == 2)
		{
			pkt >> nItemID >> bSrcPos >> nCoins; // coins will always be 0 (it's disabled)
			if (nItemID != 0)
				nCoinRequirement = 10000; // if coins were enabled, we'd obviously tack nCoins onto this.
			else
				nCoinRequirement = 5000; // if coins were enabled, we'd obviously tack nCoins onto this.

			_ITEM_TABLE *pTable = g_pMain->GetItemPtr(nItemID);

			// Invalid item (ID doesn't exist)
			if (pTable == nullptr
				// Invalid slot ID
					|| bSrcPos > HAVE_MAX
					// Item doesn't match what the server sees.
					|| (pItem = GetItem(SLOT_MAX + bSrcPos))->nNum != nItemID)
					bResult = -1;
			// Untradeable item
			else if (pTable->m_bRace == RACE_UNTRADEABLE || nItemID >= ITEM_GOLD || pItem->isSealed() || pItem->isRented() || pItem->nExpirationTime != 0 || pItem->isBound() || pItem->isDuplicate())	
			{
				bResult = -32;
				goto send_packet;
			}
		}


		pkt >> strMessage;
		if (pItem != nullptr)
			Serial = pItem->nSerialNum;

		// Invalid message length
		if (strMessage.empty() || strMessage.size() > 128)
			bResult = -1;

		if (bResult != 1 && nCoins == 0)
			goto send_packet;

		// Ensure they have all the coins they need
		if (m_iGold < nCoinRequirement)
		{
			bResult = -1;
			goto send_packet;
		}

		// Leave the rest up to the database (does the character exist, etc?)
		if (pItem != nullptr)
		{
			if (pItem->nNum == nItemID && pItem->nSerialNum == Serial)
				bResult = g_DBAgent.SendLetter(m_strUserID, strRecipient, strSubject, strMessage, bType, pItem, nCoins);
			else
				bResult = 1;
		}
		else
			bResult = g_DBAgent.SendLetter(m_strUserID, strRecipient, strSubject, strMessage, bType, pItem, nCoins);

		if (bResult != 1)
			goto send_packet;

		// Remove the player's coins
		if (nCoins != 0)
			GoldLose(nCoinRequirement+nCoins);
		else
			GoldLose(nCoinRequirement);

		// Remove the player's item
		if (pItem != nullptr)
		{
			memset(pItem, 0, sizeof(_ITEM_DATA));
			SendStackChange(nItemID, pItem->sCount, pItem->sDuration, bSrcPos);
		}

		// If the other player's online, notify them.
		pUser = g_pMain->GetUserPtr(strRecipient, TYPE_CHARACTER);
		if (pUser != nullptr)
		{
			Packet notification(WIZ_SHOPPING_MALL, uint8(STORE_LETTER));
			notification << uint8(LETTER_UNREAD) << true;
			pUser->Send(&notification);
		}

send_packet:
		result	<< uint8(LETTER_SEND) << uint8(bResult);
		Send(&result);
	}
}

void CUser::ReqLetterGetItem(Packet & pkt)
{
	Packet result(WIZ_SHOPPING_MALL, uint8(STORE_LETTER));
	uint64 nSerialNum = 0;
	uint32 nLetterID = pkt.read<uint32>(), nItemID = 0, nCoins = 0;
	uint16 sCount = 0, sDurability = 0;
	int8 bResult = g_DBAgent.GetItemFromLetter(m_strUserID, nLetterID, nItemID, sCount, sDurability, nCoins, nSerialNum);
	int pos = -1;

	if (isMerchanting() || isTrading())
		bResult = -1;

	// If the request was successful, check requirements...
	if (bResult == 1)
	{
		// If we're being given an item, do we have enough room for this item?
		if (nItemID
			&& ((pos = FindSlotForItem(nItemID, sCount)) < 0
			|| !CheckWeight(nItemID, sCount)))
			bResult = -1;

		// If we're being given coins, do they exceed our max?
		if (nCoins
			&& m_iGold + nCoins > COIN_MAX)
			bResult = -1;
	}

	// If all of the requirements passed, we can give the items/coins.
	// But ONLY if ALL requirements are met.
	if (bResult == 1)
	{
		if (nItemID)
		{
			_ITEM_DATA *pItem = GetItem(pos);

			pItem->nNum = nItemID;
			pItem->sCount += sCount;
			pItem->sDuration += sDurability;
			uint32 renttime;

			foreach_stlmap(itr,g_pMain->m_ExpirationArray)
			{
				_ITEM_EXPIRATION *pExpiration = g_pMain->m_ExpirationArray.GetData(itr->first);
				if(pItem->nNum == pExpiration->nItemID)

				{
					pItem->nExpirationTime = uint32(UNIXTIME+24*60*60*pExpiration->nExpirationDay);
					renttime = uint32(UNIXTIME+24*60*60*pExpiration->nExpirationDay);
					/*SendStackChange(nItemID, pItem->sCount, pItem->sDuration, pos - SLOT_MAX,pItem->sCount == sCount,renttime);
					printf("\nenvio1");
					}
					else{
					SendStackChange(nItemID, pItem->sCount, pItem->sDuration, pos - SLOT_MAX,pItem->sCount == sCount);
					printf("\nenvio2");
					}*/
				}
			}
			SendStackChange(nItemID, pItem->sCount, pItem->sDuration, pos - SLOT_MAX,pItem->sCount == sCount,renttime);

		}

		if (nCoins)
			GoldGain(nCoins);
	}

	result << uint8(LETTER_GET_ITEM) << bResult;
	Send(&result);
}

void CUser::ReqLetterDelete(Packet & pkt)
{

	Packet result(WIZ_SHOPPING_MALL, uint8(STORE_LETTER));
	uint8 bCount = pkt.read<uint8>();
	result << uint8(LETTER_DELETE) << bCount;
	for (uint8 i = 0; i < bCount; i++)
	{
		uint32 nLetterID = pkt.read<uint32>();
		g_DBAgent.DeleteLetter(m_strUserID, nLetterID);
		result << nLetterID;
	}
	Send(&result);

}
