#ifndef DEF_SUBSCRIPTIONS_H
#define DEF_SUBSCRIPTIONS_H

#include "Player.h"
#include "Config.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"

enum HoarderStorages
{
    STORAGE_ONE,
    STORAGE_TWO,
    STORAGE_THREE,
    MAX_HOARDER_STORAGES,

    STORAGE_SEARCH // temporary
};

enum HoarderSettings
{
    SETTING_MEMBERSHIP_LEVEL = 0 // Acore CMS Subscriptions
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
};

#define sSubscriptions AcoreSubscriptions::instance()

#endif
