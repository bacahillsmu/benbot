// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill

#include "MechOpener.hpp"
#include "Historican.h"
#include "core/API.h"
#include "core/Helpers.h"



// ----------------------------------------------------------------------------
MechOpener::MechOpener()
    :Strategy(20.0f)
    ,m_state(State::WAIT_REAPER)
{

}

// ----------------------------------------------------------------------------
void MechOpener::OnStep(Builder* builder_)
{
    Strategy::OnStep(builder_);
}

// ----------------------------------------------------------------------------
void MechOpener::OnGameStart(Builder* builder_)
{
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_REFINERY);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_BARRACKS);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_FACTORY);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_REFINERY);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_FACTORY);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_BARRACKS);

    // Behavior Tree;
// 	auto sequence = std::make_shared<BrainTree::Sequence>();
// 	auto myAction = std::make_shared<Action>();
// 	sequence->addChild(myAction);
// 	tree.setRoot(sequence);

}

// ----------------------------------------------------------------------------
void MechOpener::OnUnitIdle(WrappedUnit* unit_, Builder* builder_)
{
    if (m_state == State::BUILDING_REAPER)
    {
        if (unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_BARRACKS)
        {
            //gHistory.info() << "MechOpener: Reaper was just finished. Starting construction on Reactor." << std::endl;

            builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR, true);

            m_state = State::BUILDING_REACTOR;
        }
    }

    if (m_state == State::BUILDING_REACTOR)
    {
        if (unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_BARRACKS)
        {
            //gHistory.info() << "MechOpener: Reactor was just finished. Starting Marine Production." << std::endl;

            m_buildMarines = true;
            m_state = State::FACTORY_PRODUCTION;
        }
    }

    if(m_state == State::FACTORY_PRODUCTION)
    {
        if (unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_FACTORY)
        {
            builder_->ScheduleTraining(sc2::UNIT_TYPEID::TERRAN_HELLION, unit_);
        }
    }

    if(m_buildMarines)
    {
        BuildMarines(unit_, builder_);
    }
}

// ----------------------------------------------------------------------------
void MechOpener::OnUnitCreated(WrappedUnit* unit_, Builder* builder_)
{
    builder_;
    if(m_state == State::WAIT_REAPER)
    {
        if (unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_REAPER)
        {
            m_state = State::BUILDING_REAPER;
        }
    }

    if (unit_->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_FACTORY)
    {
        //builder_->ScheduleTraining(sc2::UNIT_TYPEID::TERRAN_HELLION, unit_);
    }

    Strategy::OnUnitCreated(unit_, builder_);
}

// ----------------------------------------------------------------------------
void MechOpener::BuildMarines(WrappedUnit* unit_, Builder* builder_)
{
    if (unit_->unit_type.ToType() != sc2::UNIT_TYPEID::TERRAN_BARRACKS)
    {
        return;
    }

    if (IsIdleUnit(sc2::UNIT_TYPEID::TERRAN_BARRACKS, true)(unit_)/* && HasAddon(sc2::UNIT_TYPEID::TERRAN_REACTOR)(*unit_)*/)
    {
        //gHistory.info() << "Marines Scheduled." << std::endl;
        builder_->ScheduleTraining(sc2::UNIT_TYPEID::TERRAN_MARINE, unit_);
        builder_->ScheduleTraining(sc2::UNIT_TYPEID::TERRAN_MARINE, unit_);
    }
//     else
//     {
//         gHistory.info() << "Marines Scheduled." << std::endl;
//         builder_->ScheduleTraining(sc2::UNIT_TYPEID::TERRAN_MARINE, unit_);
//     }
}

