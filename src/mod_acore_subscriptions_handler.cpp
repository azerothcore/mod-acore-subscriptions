/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Subscriptions.h"

AcoreSubscriptions* AcoreSubscriptions::instance()
{
    static AcoreSubscriptions instance;
    return &instance;
}

uint32 AcoreSubscriptions::GetConvertedMembershipLevel(uint32 level) const
{
    switch (level)
    {
        case MEMBERSHIP_LEVEL_ADMIRER:
            return 1; // Admirer of Chromie
        case MEMBERSHIP_LEVEL_WATCHER:
            return 2; // Watcher
        case MEMBERSHIP_LEVEL_KEEPER:
            return 3; // Time Keeper
        default:
            return 0; // No membership
    }
}

std::string AcoreSubscriptions::GetSubscriptionInfo(uint32 membershipLevel) const
{
    switch (membershipLevel)
    {
        case 0: // MEMBERSHIP_LEVEL_NONE
            return "You do not have an active subscription.";
        case 1: // MEMBERSHIP_LEVEL_ADMIRER
            return "You are an Admirer of Chromie (level 1). Thank you for your support!";
        case 2: // MEMBERSHIP_LEVEL_WATCHER
            return "You are a Watcher (level 2). Your support is greatly appreciated!";
        case 3: // MEMBERSHIP_LEVEL_KEEPER
            return "You are a Time Keeper (level 3). Your support is invaluable!";
        default:
            return "Unknown membership level. Level: " + membershipLevel;
    }

    return "Unknown membership level.";
}
