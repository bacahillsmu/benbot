// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill

#include "HatchFirst17.h"
#include "Hub.h"
#include "Historican.h"
#include "core/Helpers.h"

#include <sc2api/sc2_typeenums.h>

// ----------------------------------------------------------------------------
HatchFirst17::HatchFirst17():
    Strategy(999.0f)
{
}

// ----------------------------------------------------------------------------
void HatchFirst17::OnGameStart(Builder* builder_)
{
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::ZERG_HATCHERY, true);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::ZERG_EXTRACTOR);
    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::ZERG_QUEEN);
    builder_->ScheduleUpgrade(sc2::UPGRADE_ID::ZERGLINGMOVEMENTSPEED);
}

// ----------------------------------------------------------------------------
void HatchFirst17::OnStep(Builder*)
{
    
}

// ----------------------------------------------------------------------------
void HatchFirst17::OnUnitIdle(const sc2::Unit*, Builder*)
{
    
}

void HatchFirst17::OnUnitCreated(const sc2::Unit*, Builder*)
{
    
}

