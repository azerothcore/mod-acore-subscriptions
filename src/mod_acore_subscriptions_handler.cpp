/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Subscriptions.h"

AcoreSubscriptions* AcoreSubscriptions::instance()
{
    static AcoreSubscriptions instance;
    return &instance;
}
