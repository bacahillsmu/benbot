// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Hub.h"
#include "Miner.h"
#include "core/API.h"
#include "core/Helpers.h"
#include "core/Order.h"

#include <sc2api/sc2_typeenums.h>

#include <vector>

namespace {
const int mule_energy_cost = 50;

// ----------------------------------------------------------------------------
void SecureMineralsIncome(Builder* builder_)
{
    std::vector<Order> orders;

    // Grab all Town Halls;
    auto town_halls = gAPI->observer().GetUnits(IsTownHall());

    // Iterate over Town Halls;
    for (const auto& i : town_halls())
    {
        // If we have enough harvesters, then continue;
        if (i->assigned_harvesters >= i->ideal_harvesters)
        {
            continue;
        }

        // If our orders are not empty and we are doing something, then continue;
        if (!i->orders.empty())
        {
            continue;
        }
            
        // If we are already scheduled trainings, then continue;
        if (builder_->CountScheduledTrainings(gHub->GetCurrentWorkerType()) > 0)
        {
            continue;
        }

        // FIXME (alkurbatov): We should set an assignee for drones
        // and pick a larva closest to the assignee.
        if (gHub->GetCurrentRace() == sc2::Race::Zerg)
        {
            orders.emplace_back(gAPI->observer().GetUnitTypeData(sc2::UNIT_TYPEID::ZERG_DRONE));
            continue;
        }

        orders.emplace_back(gAPI->observer().GetUnitTypeData(gHub->GetCurrentWorkerType()), i);
    }

    builder_->ScheduleOrders(orders);
}

// ----------------------------------------------------------------------------
void SecureVespeneIncome()
{
    auto refineries = gAPI->observer().GetUnits(IsRefinery());

    for (const auto& i : refineries())
    {
        if (i->assigned_harvesters >= i->ideal_harvesters)
        {
            continue;
        }

        gHub->AssignVespeneHarvester(*i);
    }
}

// ----------------------------------------------------------------------------
void NewTownHallSetRallyWorker(const sc2::Unit* unit_)
{
    if (!IsTownHall()(*unit_))
    {
        return;
    }

    auto units = gAPI->observer().GetUnits(IsVisibleMineralPatch(), sc2::Unit::Alliance::Neutral);

    const sc2::Unit* mineral_target = units.GetClosestUnit(unit_->pos);
    if (!mineral_target)
    {
        return;
    }

    gAPI->action().Cast(*unit_, sc2::ABILITY_ID::RALLY_WORKERS, *mineral_target);
}

// ----------------------------------------------------------------------------
void SetIdleWorkersToMineMinerals(const sc2::Unit* unit_)
{
    auto units = gAPI->observer().GetUnits(IsVisibleMineralPatch(), sc2::Unit::Alliance::Neutral);

    switch (unit_->unit_type.ToType())
    {
        case sc2::UNIT_TYPEID::PROTOSS_PROBE:
        case sc2::UNIT_TYPEID::TERRAN_SCV:
        case sc2::UNIT_TYPEID::ZERG_DRONE:
        {
            const sc2::Unit* mineral_target = units.GetClosestUnit(gAPI->observer().StartingLocation());
            if (!mineral_target)
            {
                return;
            }

            gAPI->action().Cast(*unit_, sc2::ABILITY_ID::SMART, *mineral_target);
            break;
        }

        default:
        {
            break;
        }
    }
}

// ----------------------------------------------------------------------------
void CallDownMULE()
{
    auto orbitals = gAPI->observer().GetUnits(IsUnit(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND));

    if (orbitals.Empty())
    {
        return;
    }

    auto units = gAPI->observer().GetUnits(IsVisibleMineralPatch(), sc2::Unit::Alliance::Neutral);

    for (const auto& i : orbitals())
    {
        if (i->energy < mule_energy_cost)
        {
            continue;
        }
            
        const sc2::Unit* mineral_target = units.GetClosestUnit(i->pos);
        if (!mineral_target)
        {
            continue;
        }

        gAPI->action().Cast(*i, sc2::ABILITY_ID::EFFECT_CALLDOWNMULE, *mineral_target);
    }
}

}  // namespace

// ----------------------------------------------------------------------------
void Miner::OnStep(Builder* builder_)
{
    SecureMineralsIncome(builder_);
    SecureVespeneIncome();

    if (gHub->GetCurrentRace() == sc2::Race::Terran)
    {
        CallDownMULE();
    }

    
}

// ----------------------------------------------------------------------------
void Miner::OnUnitCreated(const sc2::Unit* unit_, Builder*)
{
    NewTownHallSetRallyWorker(unit_);
}

// ----------------------------------------------------------------------------
void Miner::OnUnitIdle(const sc2::Unit* unit_, Builder*)
{
    SetIdleWorkersToMineMinerals(unit_);
}
