/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Subscriptions.h"

class mod_acore_subscriptions_playerscript : public PlayerScript
{
public:
    mod_acore_subscriptions_playerscript() : PlayerScript("mod_acore_subscriptions_playerscript",
    {
        PLAYERHOOK_ON_LOGIN
    }) { }

    void OnPlayerLogin(Player* player) override
    {
        if (!sSubscriptions->IsEnabled())
            return;

        uint32 accountId = 0;
        uint8 membershipLevel = 0;

        if (player->GetSession())
            accountId = player->GetSession()->GetAccountId();

        if (QueryResult resultAcc = LoginDatabase.Query("SELECT `membership_level`  FROM `acore_cms_subscriptions` WHERE `account_name` COLLATE utf8mb4_general_ci = (SELECT `username` FROM `account` WHERE `id` = {})", accountId))
            membershipLevel = sSubscriptions->GetConvertedMembershipLevel((*resultAcc)[0].Get<uint32>());

        player->UpdatePlayerSetting(ModName, SETTING_ACORE_MEMBERSHIP_LEVEL, membershipLevel);
    }
};

class mod_acore_subscriptions_worldscript : public WorldScript
{
public:
    mod_acore_subscriptions_worldscript() : WorldScript("mod_acore_subscriptions_worldscript", {
        WORLDHOOK_ON_AFTER_CONFIG_LOAD,
        }) {
    }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        sSubscriptions->SetEnabled(sConfigMgr->GetOption<bool>("ModAcoreSubscriptions.Enable", false));
    }
};

void AddModAcoreSubscriptionsScripts()
{
    new mod_acore_subscriptions_playerscript();
    new mod_acore_subscriptions_worldscript();
}
