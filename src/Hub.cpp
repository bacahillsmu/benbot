// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Historican.h"
#include "Hub.h"
#include "core/Helpers.h"

#include <algorithm>
#include <cmath>

namespace
{
struct SortByDistance
{
    explicit SortByDistance(const sc2::Point3D& point_);

    bool operator()(const std::shared_ptr<Expansion>& lhs_, const std::shared_ptr<Expansion>& rhs_) const;

 private:
    sc2::Point3D m_point;
};

SortByDistance::SortByDistance(const sc2::Point3D& point_):
    m_point(point_)
{
}

bool SortByDistance::operator()(const std::shared_ptr<Expansion>& lhs_, const std::shared_ptr<Expansion>& rhs_) const
{
    return sc2::DistanceSquared2D(lhs_->m_townHallLocation, m_point) < sc2::DistanceSquared2D(rhs_->m_townHallLocation, m_point);
}

}  // namespace

Hub::Hub(sc2::Race current_race_, Expansions expansions_)
    : m_current_race(current_race_)
    , m_expansions(std::move(expansions_))
    , m_current_worker_type(sc2::UNIT_TYPEID::INVALID)
{
    std::sort(m_expansions.begin(), m_expansions.end(), SortByDistance(gAPI->observer().StartingLocation()));

    switch (m_current_race)
    {
        case sc2::Race::Protoss:
            m_current_worker_type = sc2::UNIT_TYPEID::PROTOSS_PROBE;
            return;

        case sc2::Race::Terran:
            m_current_worker_type = sc2::UNIT_TYPEID::TERRAN_SCV;
            return;

        case sc2::Race::Zerg:
            m_current_worker_type = sc2::UNIT_TYPEID::ZERG_DRONE;
            return;

        default:
            return;
    }
}

void Hub::OnStep()
{
}

void Hub::OnUnitCreated(WrappedUnit* unit_)
{
    if (IsTownHall()(unit_))
    {
        for (auto& i : m_expansions)
        {
            if (std::floor(i->m_townHallLocation.x) != std::floor(unit_->pos.x)
             || std::floor(i->m_townHallLocation.y) != std::floor(unit_->pos.y))
            {
                continue;
            }

            i->m_alliance = sc2::Unit::Alliance::Self;
            i->m_townHall = unit_;

            if (i->m_alliance != sc2::Unit::Alliance::Self)
            {
                gHistory.info() << "Captured region: (" << unit_->pos.x << ", " << unit_->pos.y << ")" << std::endl;
            }
            break;
        }
    }
    else if (IsRefinery()(unit_))
    {
        for (auto& i : m_expansions)
        {
            for (const auto& geyser_position : i->geysersPosition)
            {
                if (unit_->pos.x == geyser_position.x && unit_->pos.y == geyser_position.y)
                {
                    i->refineries.emplace_back(unit_);
                    return;
                }
            }
        }
    }
}

void Hub::OnUnitDestroyed(WrappedUnit* unit_)
{
    if (IsTownHall()(unit_))
    {
        for (const auto& i : m_expansions)
        {
            if (unit_ == i->m_townHall)
            {
                i->m_alliance = sc2::Unit::Alliance::Neutral;
                i->m_townHall = nullptr;
                gHistory.info() << "We lost region: (" << unit_->pos.x << ", " << unit_->pos.y << ")" << std::endl;
                break;
            }
        }
    }
    else if (IsRefinery()(unit_))
    {
        for (auto& i : m_expansions)
        {
            for (auto it = i->refineries.begin(); it != i->refineries.end(); it++)
            {
                if (unit_ == *it)
                {
                    i->refineries.erase(it);
                    return;
                }
            }
        }
    }
}

sc2::Race Hub::GetCurrentRace() const
{
    return m_current_race;
}

sc2::UNIT_TYPEID Hub::GetCurrentWorkerType() const
{
    return m_current_worker_type;
}

WrappedUnit* Hub::GetFreeBuildingProductionAssignee(const Order *order_, sc2::UNIT_TYPEID building_)
{
    if (order_->assignee)
    {
        if (IsIdleUnit(order_->assignee->unit_type)(order_->assignee))
        {
            return order_->assignee;
        }
    }
    else
    {
        auto units = gAPI->observer().GetUnits(IsIdleUnit(building_), sc2::Unit::Alliance::Self);
        if (!units.empty())
        {
            return units.front();
        }
    }
    return nullptr;
}

WrappedUnit* Hub::GetFreeBuildingProductionAssignee(const Order *order_, sc2::UNIT_TYPEID building_, sc2::UNIT_TYPEID addon_requirement_)
{
    if (order_->assignee)
    {
        return GetFreeBuildingProductionAssignee(order_);
    }

    auto units = gAPI->observer().GetUnits(
            MultiFilter(MultiFilter::Selector::And,
                {
                    IsIdleUnit(building_),
                    HasAddon(addon_requirement_)
                }), sc2::Unit::Alliance::Self);

    if (!units.empty())
    {
        return units.front();
    }
    return nullptr;
}

bool Hub::AssignBuildingProduction(Order* order_, WrappedUnit* assignee_) const
{
    if (assignee_ && IsIdleUnit(assignee_->unit_type)(assignee_))
    {
        order_->assignee = assignee_;
        return true;
    }
    return false;
}

bool Hub::AssignBuildingProduction(Order* order_, sc2::UNIT_TYPEID building_)
{
    return AssignBuildingProduction(order_, GetFreeBuildingProductionAssignee(order_, building_));
}

bool Hub::AssignBuildingProduction(Order* order_, sc2::UNIT_TYPEID building_, sc2::UNIT_TYPEID addon_requirement_)
{
    return AssignBuildingProduction(order_, GetFreeBuildingProductionAssignee(order_, building_, addon_requirement_));
}

bool Hub::AssignRefineryConstruction(Order* order_, WrappedUnit* geyser_)
{
    WrappedUnit* unit = GetClosestFreeWorker(geyser_->pos);
    Worker* worker = (Worker*)unit;
    if (!worker)
    {
        return false;
    }

    worker->BuildRefinery(order_, geyser_);
    return true;
}

bool Hub::AssignBuildTask(Order* order_, const sc2::Point2D& point_)
{
    WrappedUnit* unit = GetClosestFreeWorker(point_);
    Worker* worker = (Worker*)unit;
    if (!worker)
    {
        return false;
    }

    worker->Build(order_, point_);
    return true;
}

void Hub::AssignVespeneHarvester(WrappedUnit* refinery_)
{
    WrappedUnit* unit = GetClosestFreeWorker(refinery_->pos);
    Worker* worker = (Worker*)unit;
    if (!worker)
    {
        return;
    }

    worker->GatherVespene(*refinery_);
}

const Expansions& Hub::GetExpansions() const
{
    return m_expansions;
}

Expansions Hub::GetOurExpansions() const
{
    Expansions expansions;

    // Grab all of our expansions;
    for (auto& expo : m_expansions)
    {
        if (expo->m_alliance == sc2::Unit::Alliance::Self)
        {
            expansions.emplace_back(expo);
        }
    }

    // Starting from the StartingLocation, sort each expansion, assumed the next closest base to StartingLocation is Natural;
    auto starting = GetClosestExpansion(gAPI->observer().StartingLocation());
    std::sort(expansions.begin(),
              expansions.end(),
              [&starting](auto& e1, auto& e2)
    {
        return starting->distanceTo(e1) < starting->distanceTo(e2);
    });

    return expansions;
}

std::shared_ptr<Expansion> Hub::GetClosestExpansion(const sc2::Point2D& location_) const
{
    assert(!m_expansions.empty());

    auto closest = m_expansions[0];
    for (auto& expansion : m_expansions)
    {
        if (sc2::DistanceSquared2D(location_, expansion->m_townHallLocation)
          < sc2::DistanceSquared2D(location_, closest->m_townHallLocation))
        {
            closest = expansion;
        }
    }

    return closest;
}

const sc2::Point3D* Hub::GetNextExpansion()
{
    for(auto e : m_expansions)
    {
        if(e->m_owner == Owner::NEUTRAL)
        {
            e->m_owner = Owner::CONTESTED;
            return &e->m_townHallLocation;
        }
    }

    return nullptr;
}

int Hub::GetOurExpansionCount() const
{
    int count = 0;
    for (auto& expo : gHub->GetExpansions())
    {
        if (expo->m_alliance == sc2::Unit::Alliance::Self)
        {
            count++;
        }
    }
    return count;
}

void Hub::RequestScan(const sc2::Point2D& pos)
{
    if (gAPI->observer().GetGameLoop() == m_lastStepScan)
    {
        return;
    }

    if (gAPI->observer().GetVisibility(pos) == sc2::Visibility::Visible)
    {
        auto size = gAPI->observer().GetUnits(
            MultiFilter(MultiFilter::Selector::And,
            {
                IsWithinDistance(pos, API::OrbitalScanRadius),
                CloakState(sc2::Unit::Cloaked)
            }),
            sc2::Unit::Alliance::Enemy).size();

        if (size == 0)
        {
            return;
        }
    }

    auto orbitals = gAPI->observer().GetUnits(IsUnit(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND), sc2::Unit::Alliance::Self);
    for (auto& orbital : orbitals)
    {
        if (orbital->energy >= API::OrbitalScanCost)
        {
            gAPI->action().Cast(*orbital, sc2::ABILITY_ID::EFFECT_SCAN, pos);
            m_lastStepScan = gAPI->observer().GetGameLoop();
            break;
        }
    }
}

std::unique_ptr<Hub> gHub;
