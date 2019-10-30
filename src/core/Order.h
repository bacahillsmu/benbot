// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include <sc2api/sc2_data.h>
#include <sc2api/sc2_unit.h>

class WrappedUnit;

struct Order
{
    explicit Order(const sc2::UnitTypeData& data_, WrappedUnit* assignee_ = nullptr)
        : name(data_.name)
        , mineral_cost(data_.mineral_cost)
        , vespene_cost(data_.vespene_cost)
        , food_required(data_.food_required)
        , tech_requirement(data_.tech_requirement)
        , unit_type_id(data_.unit_type_id)
        , ability_id(data_.ability_id)
        , tech_alias(data_.tech_alias)
        , assignee(assignee_)
    {
    }

    explicit Order(const sc2::UpgradeData& data_)
        : name(data_.name)
        , mineral_cost(data_.mineral_cost)
        , vespene_cost(data_.vespene_cost)
        , food_required(0.0f)
        , tech_requirement(sc2::UNIT_TYPEID::INVALID)
        , unit_type_id(sc2::UNIT_TYPEID::INVALID)
        , ability_id(data_.ability_id)
    {
    }

    std::string name;

    uint32_t mineral_cost;
    uint32_t vespene_cost;

    float food_required;
    sc2::UnitTypeID tech_requirement;

    sc2::UnitTypeID unit_type_id;
    sc2::AbilityID ability_id;
    std::vector<sc2::UnitTypeID> tech_alias;

    WrappedUnit* assignee = nullptr;
};
