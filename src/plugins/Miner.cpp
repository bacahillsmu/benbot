// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Hub.h"
#include "Miner.h"
#include "core/API.h"
#include "core/Helpers.h"
#include "core/Order.h"
#include "core/WrappedUnit.hpp"
#include "core/Timer.h"

#include <sc2api/sc2_typeenums.h>

#include <vector>

// ----------------------------------------------------------------------------
void Miner::OnStep(Builder* builder_)
{
    WrappedUnits units = gAPI->observer().GetUnits(IsWorkerWithJob(Worker::Job::GATHERING_NOTHING), sc2::Unit::Alliance::Self);
    for (WrappedUnit* unit : units)
    {
        // These are for sure Workers;
        Worker* worker = (Worker*)unit;
        worker->Mine();
    }

    if (gAPI->observer().GetGameLoop() % m_stepsBetweenBalance == 0)
    {
        BalanceWorkers();
    }

    SecureMineralsIncome(builder_);
    SecureVespeneIncome();

    if (gHub->GetCurrentRace() == sc2::Race::Terran)
    {
        CallDownMULE();
    }
}

// ----------------------------------------------------------------------------
void Miner::OnUnitCreated(WrappedUnit* unit_, Builder*)
{
    if (IsTownHall()(unit_))
    {
        WrappedUnits minerals = gAPI->observer().GetUnits(IsVisibleMineralPatch(), sc2::Unit::Alliance::Neutral);

        auto mineralPatch = minerals.GetClosestUnit(unit_->pos);
        if (!mineralPatch)
        {
            return;
        }

        gAPI->action().Cast(*unit_, sc2::ABILITY_ID::RALLY_WORKERS, *mineralPatch);
    }
}

// ----------------------------------------------------------------------------
void Miner::OnUnitDestroyed(WrappedUnit* unit_, Builder*)
{
    if (IsTownHall()(unit_))
    {
        auto expansion = gHub->GetClosestExpansion(unit_->pos);
        ClearWorkersHomeBaseIfNoActiveExpansion(expansion);
        SplitWorkersOffFromThisExpansion(expansion);
    }
}

// ----------------------------------------------------------------------------
void Miner::OnUnitIdle(WrappedUnit* unit_, Builder*)
{
    switch (unit_->unit_type.ToType())
    {
    case sc2::UNIT_TYPEID::TERRAN_SCV:
    case sc2::UNIT_TYPEID::PROTOSS_PROBE:
    case sc2::UNIT_TYPEID::ZERG_DRONE:
    {
        Worker* worker = (Worker*)unit_;
        auto job = worker->GetJob();
        if (job == Worker::Job::GATHERING_MINERALS
         || job == Worker::Job::GATHERING_VESPENE
         || job == Worker::Job::BUILDING)
        {
            worker->SetAsNotGathering();
        }
        break;
    }

    case sc2::UNIT_TYPEID::TERRAN_MULE:
    {
        float distanceToClosestExpansionWithMinerals = std::numeric_limits<float>::max();
        const WrappedUnit* closestExpansionWithMinerals = nullptr;

        for (auto& expansion : gHub->GetExpansions())
        {
            if (expansion->m_alliance == sc2::Unit::Alliance::Self
                && expansion->m_townHall->ideal_harvesters > 0)
            {
                float distance_to_town_hall = sc2::DistanceSquared2D(unit_->pos, expansion->m_townHall->pos);
                if (distance_to_town_hall < distanceToClosestExpansionWithMinerals)
                {
                    distanceToClosestExpansionWithMinerals = distance_to_town_hall;
                    closestExpansionWithMinerals = expansion->m_townHall;
                }
            }
        }

        if (closestExpansionWithMinerals)
        {
            WrappedUnits minerals = gAPI->observer().GetUnits(IsMineralPatch(), sc2::Unit::Alliance::Neutral);
            WrappedUnit* mineralPatch = minerals.GetClosestUnit(closestExpansionWithMinerals->pos);

            if (mineralPatch)
            {
                gAPI->action().Cast(*unit_, sc2::ABILITY_ID::SMART, *mineralPatch);
            }
        }
        break;
    }
    default:
        break;
    }
}

// ----------------------------------------------------------------------------
void Miner::SecureMineralsIncome(Builder* builder_)
{
    std::vector<Order> orders;

    // Grab all Town Halls;
    WrappedUnits commandCenters = gAPI->observer().GetUnits(IsTownHall(), sc2::Unit::Alliance::Self);
    WrappedUnits refineries = gAPI->observer().GetUnits(IsRefinery(), sc2::Unit::Alliance::Self);
    WrappedUnits workers = gAPI->observer().GetUnits(IsWorker(), sc2::Unit::Alliance::Self);
    int numWorkers = (int)workers.size();
    int optimalWorkers = 0;

    for (auto& commandCenter : commandCenters)
    {
        optimalWorkers += static_cast<int>(std::ceil(1.5f * commandCenter->ideal_harvesters));
    }

    for (auto& refinery : refineries)
    {
        optimalWorkers += refinery->ideal_harvesters;
        if (refinery->assigned_harvesters > 0)
        {
            numWorkers++;
        }
    }

    optimalWorkers = std::min(optimalWorkers, m_maximumWorkers);

    if (numWorkers >= optimalWorkers)
    {
        return;
    }

    // Iterate over Town Halls;
    for (const auto& commandCenter : commandCenters)
    {
        // If we already have too many workers, don't worry about any command center;
        if (numWorkers++ >= optimalWorkers)
        {
            break;
        }

        // If our command center is not done building yet, then continue;
        if(commandCenter->build_progress != 1.0f)
        {
            continue;
        }

        // If our orders are not empty and we are doing something, then continue;
        if (commandCenter->NumberOfOrders() > 0)
        {
            continue;
        }
            
        // If we are already scheduled trainings, then continue;
        if (builder_->CountScheduledTrainings(gHub->GetCurrentWorkerType()) > 0)
        {
            continue;
        }

        orders.emplace_back(*gAPI->observer().GetUnitTypeData(gHub->GetCurrentWorkerType()), commandCenter);
    }

    if(orders.empty())
    {
        return;
    }

    builder_->ScheduleOrders(orders);
}

// // ----------------------------------------------------------------------------
void Miner::SecureVespeneIncome()
{
    // Hold onto our resource amounts, we will check ratios;
    float minerals = (float)gAPI->observer().GetMinerals();
    float vespene = (float)gAPI->observer().GetVespene();

    // If we currently not gathering vespene, check to see if our ratios are better or we are below our vespene threshold;
    if (m_vespeneGasStop)
    {
        if (vespene <= m_vespeneStartThreshold
        || (minerals != 0 && (vespene / minerals) <= m_vespeneToMineralsStartRatio))
        {
            m_vespeneGasStop = false;
        }
        
    }
    // If we have too much vespene, then stop gathering vespene and switch as many workers as possible to gathering minerals;
    else if (vespene >= m_vespeneMinimumForStopThreshold
         && (minerals == 0 || (vespene / minerals) >= m_vespeneToMineralsStopRatio))
    {
        WrappedUnits units = gAPI->observer().GetUnits(IsWorkerWithJob(Worker::Job::GATHERING_VESPENE), sc2::Unit::Alliance::Self);
        m_vespeneGasStop = true;
        for (auto& unit : units)
        {
            Worker* gasWorker = (Worker*)unit;
            gasWorker->Mine();
        }
    }

    // We can stop logic if we are not gathering vespene;
    if (m_vespeneGasStop)
    {
        return;
    }

    WrappedUnits units = gAPI->observer().GetUnits(IsWorkerWithJob(Worker::Job::GATHERING_VESPENE), sc2::Unit::Alliance::Self);

    // We are limiting one worker being moved per Step;
    for (const auto& expansion : gHub->GetExpansions())
    {
        if (expansion->m_alliance == sc2::Unit::Alliance::Self)
        {
            for (const auto& refinery : expansion->refineries)
            {
                if (refinery->assigned_harvesters < refinery->ideal_harvesters)
                {
                    WrappedUnit* unit = GetClosestFreeWorker(refinery->pos);
                    Worker* worker = (Worker*)unit;
                    if (worker)
                    {
                        worker->GatherVespene(*refinery);
                    }
                }
                else if (refinery->assigned_harvesters > refinery->ideal_harvesters)
                {
                    for (auto it = units.begin(); it != units.end(); it++)
                    {
                        if (refinery->tag == (*it)->GetPreviousStepOrders().front().target_unit_tag)
                        {
                            Worker* gasWorker = (Worker*)(*it);
                            gasWorker->Mine();
                            units.erase(it);
                            break;
                        }
                    }
                }
            }
        }
    }
}

// // ----------------------------------------------------------------------------
// void SetIdleWorkersToMineMinerals(const WrappedUnit* unit_)
// {
//     auto units = gAPI->observer().GetUnits(IsVisibleMineralPatch(), sc2::Unit::Alliance::Neutral);
// 
//     switch (unit_->unit_type.ToType())
//     {
//         case sc2::UNIT_TYPEID::PROTOSS_PROBE:
//         case sc2::UNIT_TYPEID::TERRAN_SCV:
//         case sc2::UNIT_TYPEID::ZERG_DRONE:
//         {
//             const sc2::Unit* mineral_target = units.GetClosestUnit(gAPI->observer().StartingLocation());
//             if (!mineral_target)
//             {
//                 return;
//             }
// 
//             gAPI->action().Cast(*unit_, sc2::ABILITY_ID::SMART, *mineral_target);
//             break;
//         }
// 
//         default:
//         {
//             break;
//         }
//     }
// }

// ----------------------------------------------------------------------------
void Miner::BalanceWorkers()
{
    Timer timer;
    timer.Start();

    // Build a map of how many mining workers above their ideal each expansion has;
    std::multimap<int, std::pair<const std::shared_ptr<Expansion>&, WrappedUnits>> sorted_expansions; // note: maps are ordered by key

    for (auto& expansion : gHub->GetExpansions())
    {
        if (expansion->m_alliance == sc2::Unit::Alliance::Self && expansion->m_townHall->ideal_harvesters != 0)
        {
            WrappedUnits workersGatheringMinerals = gAPI->observer().GetUnits(
                MultiFilter(MultiFilter::Selector::And,
                    {
                        IsWorkerWithHomeBase(expansion),
                        IsWorkerWithJob(Worker::Job::GATHERING_MINERALS)
                    }),
                sc2::Unit::Alliance::Self);

            int amountGatheringMinerals = (int)workersGatheringMinerals.size();
            int amountOverIdealWorkers = amountGatheringMinerals - expansion->m_townHall->ideal_harvesters;

            auto expansion_worker_pair = std::pair<const std::shared_ptr<Expansion>&, WrappedUnits>(expansion, workersGatheringMinerals);
            sorted_expansions.emplace(std::make_pair(std::max(0, amountOverIdealWorkers), expansion_worker_pair));
        }
    }

    if (sorted_expansions.size() <= 1)
    {
        return;
    }

    // If ideal difference is >= 2 of top and bottom guy => balance workers;
    int diff = sorted_expansions.rbegin()->first - sorted_expansions.begin()->first;
    if (diff >= m_reqImbalanceToTransfer)
    {
        int move = static_cast<int>(std::ceil(diff / 2.0f));
        int moved = move;
        while (move--)
        {
            if (auto unit = sorted_expansions.rbegin()->second.second.back())
            {
                Worker* worker = (Worker*)unit;
                worker->SetHomeBase(sorted_expansions.begin()->second.first);
                worker->Mine();
                sorted_expansions.rbegin()->second.second.pop_back();
            }
        }

        float time = timer.Finish();
        gHistory.info() << "Moved " << moved << " workers (" << time << " ms spent)" << std::endl;
    }
}

// ----------------------------------------------------------------------------
void Miner::CallDownMULE()
{
    auto orbitals = gAPI->observer().GetUnits(IsUnit(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND), sc2::Unit::Alliance::Self);

    if (orbitals.empty())
    {
        return;
    }

    auto units = gAPI->observer().GetUnits(IsVisibleMineralPatch(), sc2::Unit::Alliance::Neutral);

    for (const auto& i : orbitals)
    {
        if (i->energy < m_muleEnergyCost)
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

void Miner::ClearWorkersHomeBaseIfNoActiveExpansion(const std::shared_ptr<Expansion>& expansion_)
{
    bool activeExpansionExists = false;

    // Find another expansion that is ours, not this one, and needs some harvesters;
    for (auto& expansion : gHub->GetExpansions())
    {
        if (expansion->m_alliance == sc2::Unit::Alliance::Self
         && expansion != expansion_
         && expansion->m_townHall->ideal_harvesters > 0)
        {
            activeExpansionExists = true;
            break;
        }
    }

    // If we do not find an expansion, these bros are homeless;
    if (!activeExpansionExists)
    {
        WrappedUnits units = gAPI->observer().GetUnits(IsWorkerWithHomeBase(expansion_), sc2::Unit::Alliance::Self);
        for (auto& unit : units)
        {
            Worker* worker = (Worker*)unit;
            worker->SetHomeBase(nullptr);
        }
    }
}

void Miner::SplitWorkersOffFromThisExpansion(const std::shared_ptr<Expansion>& expansion_)
{
    Expansions activeExpansions;

    for (auto& expansion : gHub->GetExpansions())
    {
        if (expansion->m_alliance == sc2::Unit::Alliance::Self
         && expansion != expansion_
         && expansion->m_townHall->ideal_harvesters > 0)
        {
            activeExpansions.emplace_back(expansion);
        }
    }

    if (activeExpansions.empty())
    {
        return;
    }

    WrappedUnits units = gAPI->observer().GetUnits(IsWorkerWithHomeBase(expansion_), sc2::Unit::Alliance::Self);

    // Go through each base and put one worker at a base, then loop through;
    auto roundRobinItr = activeExpansions.begin();
    for (auto& unit : units)
    {
        Worker* worker = (Worker*)unit;

        worker->SetHomeBase(*roundRobinItr);
        if (worker->GetJob() == Worker::Job::GATHERING_MINERALS
         || worker->GetJob() == Worker::Job::GATHERING_VESPENE)
        {
            worker->Mine();
        }

        if (++roundRobinItr == activeExpansions.end())
        {
            roundRobinItr = activeExpansions.begin();
        }
    }
}



