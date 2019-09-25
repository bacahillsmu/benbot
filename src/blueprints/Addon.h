// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill

#pragma once

#include "Blueprint.h"

struct Addon : Blueprint
{
    bool Build(Order* order_) final;
    static sc2::UNIT_TYPEID GetParentStructureFromAbilityId(sc2::ABILITY_ID abilityId);

private:

    const sc2::Unit* GetValidAssignee(const Order* order_);
};