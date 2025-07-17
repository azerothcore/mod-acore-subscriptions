/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "HoardCollector.h"

HoardCollector* HoardCollector::instance()
{
    static HoardCollector instance;
    return &instance;
}

void HoardCollector::LoadCollectionItems(uint32 guid)
{
    // Load collected items from the database
    QueryResult result = CharacterDatabase.Query("SELECT PlayerGuid, StorageID, ItemEntry FROM mod_collector_items WHERE PlayerGuid = {}", guid);
    if (!result)
        return;
    do
    {
        Field* fields = result->Fetch();
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(fields[0].Get<uint32>());
        uint8 storageId = fields[1].Get<uint8>();
        uint32 itemId = fields[2].Get<uint32>();
        AddItemToCollection(playerGuid, storageId, itemId);
    } while (result->NextRow());
}

bool HoardCollector::IsItemValid(Item* item) const
{
    // Check if the item is valid for collection
    if (!item || !item->GetTemplate())
        return false;

    if (item->GetUInt32Value(ITEM_FIELD_DURATION))
        return false;

    if (item->IsNotEmptyBag())
        return false;

    if (item->GetCount() > 1)
        return false;

    // Check if the item is already in the collection
    if (HasItemInAnyCollection(item->GetOwnerGUID(), item->GetEntry()))
        return false;

    return true;
}

void HoardCollector::GiveBankItemToPlayer(Player* player, Creature* vendor, uint32 itemEntry, uint32 vendorslot)
{
    uint32 playerGuid = player->GetGUID().GetCounter();

    // Get item template
    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemEntry);
    if (!proto)
        return;

    QueryResult result = CharacterDatabase.Query("SELECT ItemGUID FROM mod_collector_items WHERE PlayerGuid = {} AND ItemEntry = {}", playerGuid, itemEntry);

    if (!result)
    {
        player->SendSystemMessage("You do not have this item in your collection.");
        return;
    }

    Field* fields = result->Fetch();
    uint32 itemGUID = fields[0].Get<uint32>();

    // Create item
    Item* pItem = NewItemOrBag(proto);
    if (!pItem)
        return;

    // Load from DB
    QueryResult selectItemQuery = CharacterDatabase.Query(
        "SELECT creatorGuid, giftCreatorGuid, count, duration, charges, flags, enchantments, "
        "randomPropertyId, durability, playedTime, text, itemEntry "
        "FROM item_instance WHERE guid = {}",
        itemGUID
    );

    Field* selectItemField = nullptr;
    if (selectItemQuery)
        selectItemField = selectItemQuery->Fetch();

    if (!selectItemField || !pItem->LoadFromDB(itemGUID, player->GetGUID(), selectItemField, itemEntry))
    {
        delete pItem; // avoid leak
        return;
    }

    // Try to store in player inventory
    ItemPosCountVec dest;
    if (player->CanStoreItem(NULL_BAG, NULL_SLOT, dest, pItem, false) != EQUIP_ERR_OK)
    {
        delete pItem;
        player->SendSystemMessage("Your inventory is full.");
        return;
    }

    // Perform transaction
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    player->MoveItemToInventory(dest, pItem, true, false);
    player->SaveInventoryAndGoldToDB(trans);
    CharacterDatabase.CommitTransaction(trans);

    // Notify client about vendor item buy
    WorldPacket data(SMSG_BUY_ITEM, (8 + 4 + 4 + 4));
    data << vendor->GetGUID();
    data << uint32(vendorslot + 1);                   // numbered from 1 at client
    data << 0;
    data << 1;
    player->GetSession()->SendPacket(&data);

    CharacterDatabase.Execute("DELETE FROM mod_collector_items WHERE PlayerGuid = {} AND ItemEntry = {}", playerGuid, itemEntry);
}

bool HoardCollector::IsStorageAvailable(Player* player, uint8 storageId) const
{
    // Always available to game masters
    if (player->IsGameMaster())
        return true;

    // Module override, allows enabling the storage without subscription
    if (player->GetPlayerSetting(ModName, storageId).IsEnabled())
        return true;

    // Finally, if the storage is available based on the player's subscription.
    // Each storageId corresponds to a specific subscription level.
    return GetSubscriptionLevel(player) > storageId;
}
