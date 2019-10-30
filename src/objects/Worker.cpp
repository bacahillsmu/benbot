// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Worker.h"
#include "core/Helpers.h"
#include "core/API.h"
#include "Hub.h"

// ----------------------------------------------------------------------------
Worker::Worker(const sc2::Unit& unit_)
    : WrappedUnit(unit_)
{
}

// ----------------------------------------------------------------------------
void Worker::BuildRefinery(Order* order_, const WrappedUnit* geyser_)
{
    assert(alliance == sc2::Unit::Alliance::Self);

    gAPI->action().Stop(*this);
    order_->assignee = this;
    gAPI->action().Build(*order_, geyser_);

    m_job = Job::BUILDING_REFINERY;
}

// ----------------------------------------------------------------------------
void Worker::Build(Order* order_, const sc2::Point2D& point_)
{
    assert(alliance == sc2::Unit::Alliance::Self);

    gAPI->action().Stop(*this);
    order_->assignee = this;
    gAPI->action().Build(*order_, point_);

    m_job = Job::BUILDING;
}

// ----------------------------------------------------------------------------
void Worker::GatherVespene(const WrappedUnit& target_)
{
    assert(alliance == sc2::Unit::Alliance::Self);

    gAPI->action().Stop(*this);
    gAPI->action().Cast(*this, sc2::ABILITY_ID::SMART, target_);

    m_job = Job::GATHERING_VESPENE;

    WrappedUnits commandCenters = gAPI->observer().GetUnits(IsTownHall(), sc2::Unit::Alliance::Self);
    if (WrappedUnit* commandCenter = commandCenters.GetClosestUnit(target_.pos))
    {
        SetHomeBase(gHub->GetClosestExpansion(commandCenter->pos));
    }
}

// ----------------------------------------------------------------------------
void Worker::Mine()
{
    assert(alliance == sc2::Unit::Alliance::Self);

    if (m_homeBase)
    {
        assert(m_homeBase->m_alliance == sc2::Unit::Alliance::Self
            && m_homeBase->m_townHall
            && m_homeBase->m_townHall->is_alive);
    }

    // If we have no HomeBase, or our current HomeBase has no minerals, find a new one;
    if (!m_homeBase
        || m_homeBase->m_townHall->ideal_harvesters == 0)
    {
        float distanceToCloseTownHallWithMinerals = std::numeric_limits<float>::max();
        const std::shared_ptr<Expansion>* closestBaseWithMinerals = nullptr;

        for (auto& expansion : gHub->GetExpansions())
        {
            if (expansion->m_alliance == sc2::Unit::Alliance::Self
                && expansion->m_townHall->ideal_harvesters > 0)
            {
                float distance_to_town_hall = sc2::DistanceSquared2D(this->pos, expansion->m_townHall->pos);
                if (distance_to_town_hall < distanceToCloseTownHallWithMinerals)
                {
                    distanceToCloseTownHallWithMinerals = distance_to_town_hall;
                    closestBaseWithMinerals = &expansion;
                }
            }
        }

        if (closestBaseWithMinerals)
        {
            m_homeBase = *closestBaseWithMinerals;
        }
    }

    // If we do have a HomeBase, then go mine minerals;
    if (m_homeBase)
    {
        WrappedUnits patches = gAPI->observer().GetUnits(IsMineralPatch(), sc2::Unit::Alliance::Neutral);
        WrappedUnit* mineral = patches.GetClosestUnit(m_homeBase->m_townHall->pos);

        if (mineral)
        {
            gAPI->action().Cast(*this, sc2::ABILITY_ID::SMART, *mineral);

            m_job = Job::GATHERING_MINERALS;
        }
    }
}

// ----------------------------------------------------------------------------
void Worker::SetHomeBase(std::shared_ptr<Expansion> base)
{
    assert(alliance == sc2::Unit::Alliance::Self);

    m_homeBase = std::move(base);
}

// ----------------------------------------------------------------------------
std::shared_ptr<Expansion> Worker::GetHomeBase() const
{
    return m_homeBase;
}

// ----------------------------------------------------------------------------
Worker::Job Worker::GetJob() const
{
    return m_job;
}

// ----------------------------------------------------------------------------
void Worker::SetAsNotGathering()
{
    assert(alliance == sc2::Unit::Alliance::Self);

    m_job = Job::GATHERING_NOTHING;
}
