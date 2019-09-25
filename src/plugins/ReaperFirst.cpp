// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill


#include "ReaperFirst.hpp"
#include "Historican.h"
#include "core/API.h"
#include <sc2api/sc2_map_info.h>
//#include "core/Helpers.h"

//#include <sc2api/sc2_typeenums.h>

// ----------------------------------------------------------------------------
ReaperFirst::ReaperFirst()
    :m_state(State::WAIT_BARRACKS)
{
}

// ----------------------------------------------------------------------------
void ReaperFirst::OnStep(Builder*)
{
    if(m_state == State::MICRO_REAPER)
    {
        //const auto reaper = gAPI->observer().GetUnit(m_reaper);

    }
    else if(m_state == State::HEAL_REAPER)
    {
        //const auto reaper = gAPI->observer().GetUnit(m_reaper);

    }
}

// ----------------------------------------------------------------------------
void ReaperFirst::OnUnitCreated(const sc2::Unit* unit_, Builder* builder_)
{
    unit_;
    builder_;
}

// ----------------------------------------------------------------------------
void ReaperFirst::OnUnitIdle(const sc2::Unit* unit_, Builder* builder_)
{
    if (m_state == State::WAIT_BARRACKS)
    {
        if (unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_BARRACKS)
        {
            gHistory.info() << "ReaperFirst: Barracks was just finished. Training Reaper." << std::endl;
            builder_->ScheduleTraining(sc2::UNIT_TYPEID::TERRAN_REAPER, unit_);
            m_state = State::BUILDING_REAPER;
        }
    }

    if (m_state == State::BUILDING_REAPER)
    {
        if (unit_->unit_type.ToType() != sc2::UNIT_TYPEID::TERRAN_REAPER)
        {
            return;
        }

        gHistory.info() << "ReaperFirst: Reaper was just finished. Sending Reaper to Enemy Start Location." << std::endl;

        m_reaper = unit_->tag;
        sc2::Units reapers;
        reapers.push_back(unit_);
        auto targets = gAPI->observer().GameInfo().enemy_start_locations;
        gAPI->action().Attack(reapers, targets.front());
        m_state = State::MICRO_REAPER;
    }
}

// ----------------------------------------------------------------------------
void ReaperFirst::OnUnitDestroyed(const sc2::Unit* unit_, Builder*)
{
    if(m_state == State::FINISHED)
    {
        return;
    }

    if(unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REAPER)
    {
        m_state = State::FINISHED;
    }
}
