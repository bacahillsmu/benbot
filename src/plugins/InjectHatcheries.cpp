// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill

#include "InjectHatcheries.h"
#include "Historican.h"
#include "core/API.h"
#include "core/Helpers.h"

#include <vector>

namespace{
const int inject_cost = 25;

typedef const sc2::Unit* target_t;
typedef std::vector<const sc2::Unit*> targets_t;

target_t PickTargetForInject(const sc2::Unit* unit_)
{
    targets_t targets;

    auto hatcheries = gAPI->observer().GetUnits(IsTownHall());
    if (hatcheries.Empty())
    {
        return nullptr;
    }

    for(const auto& i : hatcheries())
    {
        if (!i->buffs.empty() && i->buffs.front() == sc2::BUFF_ID::QUEENSPAWNLARVATIMER)
        {
            continue;
        }

        targets.push_back(i);
    }

    if(targets.empty())
    {
        return nullptr;
    }

    Units targetHatcheries(targets);
    const sc2::Unit* targetHatchery = targetHatcheries.GetClosestUnit(unit_->tag);

    return targetHatchery;
}

} // namespace

void InjectHatcheries::OnStep(Builder*)
{
    auto queens = gAPI->observer().GetUnits(IsUnit(sc2::UNIT_TYPEID::ZERG_QUEEN));
    for(const auto& i : queens())
    {
        if (i->energy < inject_cost)
        {
            continue;
        }

        target_t target = PickTargetForInject(i);
        if (!target)
        {
            continue;
        }

        gAPI->action().Cast(*i, sc2::ABILITY_ID::EFFECT_INJECTLARVA, *target);
    }
}

