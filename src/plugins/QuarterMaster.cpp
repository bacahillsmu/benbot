// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Historican.h"
#include "Hub.h"
#include "QuarterMaster.h"
#include "core/API.h"

#include <numeric>

namespace {

// ----------------------------------------------------------------------------
struct CalcSupplies
{
    float operator()(float sum, const sc2::Unit* unit_) const;
    float operator()(float sum, const Order& order_) const;
};

// ----------------------------------------------------------------------------
float CalcSupplies::operator()(float sum, const sc2::Unit* unit_) const
{
    switch (unit_->unit_type.ToType())
    {
        case sc2::UNIT_TYPEID::PROTOSS_NEXUS:
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING:
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING:
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
        {
            return sum + 15.0f;
        }

        case sc2::UNIT_TYPEID::ZERG_HATCHERY:
        case sc2::UNIT_TYPEID::ZERG_HIVE:
        case sc2::UNIT_TYPEID::ZERG_LAIR:
        {
            return sum + 6.0f;
        }

        case sc2::UNIT_TYPEID::ZERG_EGG:
        {
            if (unit_->orders.front().ability_id == sc2::ABILITY_ID::TRAIN_OVERLORD)
            {
                return sum + 8.0f;
            }
            else
            {
                return sum;
            }
        }

        case sc2::UNIT_TYPEID::PROTOSS_PYLON:
        case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
        case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED:
        case sc2::UNIT_TYPEID::ZERG_OVERLORD:
        case sc2::UNIT_TYPEID::ZERG_OVERLORDCOCOON:
        case sc2::UNIT_TYPEID::ZERG_OVERLORDTRANSPORT:
        case sc2::UNIT_TYPEID::ZERG_OVERSEER:
        {
            return sum + 8.0f;
        }

        default:
        {
            return sum;
        }
    }
}

// ----------------------------------------------------------------------------
float CalcSupplies::operator()(float sum, const Order& order_) const
{
    switch (order_.ability_id.ToType())
    {
        case sc2::ABILITY_ID::BUILD_NEXUS:
        case sc2::ABILITY_ID::BUILD_COMMANDCENTER:
        {
            return sum + 15.0f;
        }

        case sc2::ABILITY_ID::BUILD_PYLON:
        case sc2::ABILITY_ID::BUILD_SUPPLYDEPOT:
        case sc2::ABILITY_ID::TRAIN_OVERLORD:
        {
            return sum + 8.0f;
        }

        default:
        {
            return sum;
        }
    }
}

// ----------------------------------------------------------------------------
struct CalcConsumption
{
    float operator()(float sum, const Order& order_) const;
};

// ----------------------------------------------------------------------------
float CalcConsumption::operator()(float sum, const Order& order_) const
{
    return sum + order_.food_required;
}

}  // namespace

// ----------------------------------------------------------------------------
QuarterMaster::QuarterMaster():
    Plugin(),
    m_skip_turn(false)
{
}

// ----------------------------------------------------------------------------
void QuarterMaster::OnStep(Builder* builder_)
{
    if (m_skip_turn)
    {
        return;
    }

    WrappedUnits units = gAPI->observer().GetUnits(sc2::Unit::Alliance::Self);
    const std::list<Order>& construction_orders = builder_->GetConstructionOrders();
    const std::list<Order>& training_orders = builder_->GetTrainingOrders();

    float expected_consumption =
        gAPI->observer().GetFoodUsed()
        + m_planningAhead
        + std::accumulate(
            training_orders.begin(),
            training_orders.end(),
            0.0f,
            CalcConsumption());

    float expected_supply =
        std::accumulate(
            units.begin(),
            units.end(),
            0.0f,
            CalcSupplies())
        + std::accumulate(
            construction_orders.begin(),
            construction_orders.end(),
            0.0f,
            CalcSupplies())
        + std::accumulate(
            training_orders.begin(),
            training_orders.end(),
            0.0f,
            CalcSupplies());

    if (expected_supply > expected_consumption || expected_supply >= 200.0f)
    {
        return;
    }

    gHistory.info() << "Request additional supplies: " << expected_consumption << " >= " << expected_supply << std::endl;

    m_skip_turn = true;

    builder_->ScheduleConstruction(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT, true);
}

// ----------------------------------------------------------------------------
void QuarterMaster::OnUnitCreated(WrappedUnit* unit_,  Builder*)
{
    if (unit_->unit_type == sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT)
    {
        m_skip_turn = false;
    }
}
