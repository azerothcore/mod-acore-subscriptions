/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Subscriptions.h"

enum Settings
{
    SETTING_MEMBERSHIP_LEVEL = 0
};

enum MembershipLevels
{
    MEMBERSHIP_LEVEL_NONE    = 0,
    MEMBERSHIP_LEVEL_ADMIRER = 2, // Level 1: Admirer of Chromie
    MEMBERSHIP_LEVEL_WATCHER = 6, // Level 2: Watcher
    MEMBERSHIP_LEVEL_KEEPER  = 7  // Level 3: Time Keeper
};

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
        {
            switch ((*resultAcc)[0].Get<uint32>())
            {
                case MEMBERSHIP_LEVEL_ADMIRER:
                    membershipLevel = 1; // Admirer of Chromie
                    break;
                case MEMBERSHIP_LEVEL_WATCHER:
                    membershipLevel = 2; // Watcher
                    break;
                case MEMBERSHIP_LEVEL_KEEPER:
                    membershipLevel = 3; // Time Keeper
                    break;
            }
        }

        player->UpdatePlayerSetting(ModName, SETTING_MEMBERSHIP_LEVEL, membershipLevel);
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
