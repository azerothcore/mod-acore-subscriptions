#ifndef DEF_SUBSCRIPTIONS_H
#define DEF_SUBSCRIPTIONS_H

#include "Player.h"
#include "Config.h"
#include "ScriptMgr.h"

enum Settings
{
    SETTING_ACORE_MEMBERSHIP_LEVEL = 0
};

enum MembershipLevels
{
    MEMBERSHIP_LEVEL_NONE    = 0,
    MEMBERSHIP_LEVEL_ADMIRER = 2, // Level 1: Admirer of Chromie
    MEMBERSHIP_LEVEL_WATCHER = 6, // Level 2: Watcher
    MEMBERSHIP_LEVEL_KEEPER  = 7  // Level 3: Time Keeper
};

std::string const ModName = "acore_cms_subscriptions";

class AcoreSubscriptions
{
private:
    bool Enabled;

public:
    static AcoreSubscriptions* instance();

    [[nodiscard]] bool IsEnabled() const { return Enabled; }
    void SetEnabled(bool enabled) { Enabled = enabled; }

    [[nodiscard]] std::string GetSubscriptionInfo(uint32 membershipLevel) const;
    [[nodiscard]] uint32 GetMembershipLevel(Player* player) const { return player->GetPlayerSetting(ModName, SETTING_ACORE_MEMBERSHIP_LEVEL).value; };
    [[nodiscard]] uint32 GetConvertedMembershipLevel(uint32 level) const;
};

#define sSubscriptions AcoreSubscriptions::instance()

#endif
