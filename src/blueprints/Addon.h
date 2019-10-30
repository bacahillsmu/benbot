// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill

#pragma once

#include "Blueprint.h"

class WrappedUnit;

struct Addon : Blueprint
{
    bool CanBeBuilt(const Order* order_) final;
    bool Build(Order* order_) final;
    static sc2::UNIT_TYPEID GetParentStructureFromAbilityId(sc2::ABILITY_ID abilityId);

private:

    WrappedUnit* GetValidAssignee(const Order* order_);
};