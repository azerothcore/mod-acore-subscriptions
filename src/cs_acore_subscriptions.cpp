/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "CharacterCache.h"
#include "Chat.h"
#include "Subscriptions.h"

using namespace Acore::ChatCommands;

class acore_subscriptions_commandscript : public CommandScript
{
public:
    acore_subscriptions_commandscript() : CommandScript("acore_subscriptions_commandscript") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable subscriptionsTable =
        {
            { "info",   HandleSubscriptionInfoCommand, SEC_PLAYER, Console::Yes },
            { "update", HandleSubscriptionUpdateCommand, SEC_MODERATOR, Console::Yes }
        };

        static ChatCommandTable commandTable =
        {
            { "subscriptions", subscriptionsTable },
        };

        return commandTable;
    }

    static bool HandleSubscriptionInfoCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (handler->GetSession() && AccountMgr::IsPlayerAccount(handler->GetSession()->GetSecurity()))
            player = PlayerIdentifier::FromSelf(handler);

        if (!player)
            player = PlayerIdentifier::FromTargetOrSelf(handler);

        if (!player)
        {
            handler->SendErrorMessage("Player not found!");
            return false;
        }

        std::string message = "";

        if (Player* target = player->GetConnectedPlayer())
        {
            message = sSubscriptions->GetSubscriptionInfo(sSubscriptions->GetMembershipLevel(target));
            handler->SendSysMessage(message);
            return true;
        }

        // Player is offline
        uint32 accountId = sCharacterCache->GetCharacterAccountIdByGuid(player->GetGUID());

        if (QueryResult resultAcc = LoginDatabase.Query("SELECT `membership_level`  FROM `acore_cms_subscriptions` WHERE `account_name` COLLATE utf8mb4_general_ci = (SELECT `username` FROM `account` WHERE `id` = {})", accountId))
            message = sSubscriptions->GetSubscriptionInfo(sSubscriptions->GetConvertedMembershipLevel((*resultAcc)[0].Get<uint32>()));

        handler->SendSysMessage(message);
        return true;
    }

    static bool HandleSubscriptionUpdateCommand(ChatHandler* handler, Optional<PlayerIdentifier> player, uint32 membershipLevel)
    {
        if (!player)
            player = PlayerIdentifier::FromTargetOrSelf(handler);

        if (!player)
        {
            handler->SendErrorMessage("Player not found!");
            return false;
        }

        if (!player->GetConnectedPlayer())
        {
            handler->SendErrorMessage("Player is not online!");
            return false;
        }

        uint32 oldMembershipLevel = sSubscriptions->GetMembershipLevel(player->GetConnectedPlayer());
        player->GetConnectedPlayer()->UpdatePlayerSetting(ModName, SETTING_ACORE_MEMBERSHIP_LEVEL, membershipLevel);
        handler->PSendSysMessage("Updated player {}'s subscription from level {} to {}.", player->GetName(), oldMembershipLevel, membershipLevel);
        handler->SendSysMessage("This is temporary and will reset at the next relog. To edit it permanently, update the database directly.");
        return true;
    }
};

void AddSC_acore_subscriptions_commandscript()
{
    new acore_subscriptions_commandscript();
}
