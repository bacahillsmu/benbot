#include "ReaperRush.hpp"
#include "core/WrappedUnits.hpp"
#include "core/Helpers.h"


// ----------------------------------------------------------------------------
ReaperRush::ReaperRush()
{
}

// ----------------------------------------------------------------------------
void ReaperRush::OnGameStart(Builder* builder_)
{
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_BARRACKS);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_REFINERY);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_BARRACKS);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_REFINERY);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_BARRACKS);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER);
}

// ----------------------------------------------------------------------------
void ReaperRush::OnUnitIdle(WrappedUnit* unit_, Builder* builder_)
{
    // We only care about the Barracks with this Opener;
    if (unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_BARRACKS)
    {
        // Every 5th Reaper we will ask this Barracks if it needs an addon;
        if(m_reapersBuilt % 5 == 0 && !HasAddon(sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR)(unit_))
        {
            builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR, true);
        }

        if(HasAddon(sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR)(unit_))
        {
            builder_->ScheduleTraining(sc2::UNIT_TYPEID::TERRAN_REAPER, unit_);
            builder_->ScheduleTraining(sc2::UNIT_TYPEID::TERRAN_REAPER, unit_);
        }
        else
        {
            builder_->ScheduleTraining(sc2::UNIT_TYPEID::TERRAN_REAPER, unit_);
        }
    }
}

// ----------------------------------------------------------------------------
void ReaperRush::OnUnitCreated(WrappedUnit* unit_, Builder* builder_)
{
    builder_;

    if(unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REAPER)
    {
        m_reapersBuilt++;
    }
}

// ----------------------------------------------------------------------------
void ReaperRush::OnStep(Builder* builder_)
{
    builder_;
}
