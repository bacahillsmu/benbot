
#include "WrappedUnit.hpp"
#include "API.h"
#include "Helpers.h"
#include "objects/Worker.h"
#include "plugins/micro/MicroPlugin.h"

WrappedUnit::WrappedUnit(const sc2::Unit& unit_)
    : sc2::Unit(unit_)
    , IsInVision(true)
{
    if (unit_.alliance == Unit::Alliance::Self)
    {
        m_micro = MicroPlugin::MakePlugin(this);
    }
}

bool WrappedUnit::operator==(const WrappedUnit& other_) const
{
    return tag == other_.tag;
}

MicroPlugin* WrappedUnit::Micro()
{
    return m_micro.get();
}

std::unique_ptr<WrappedUnit> WrappedUnit::Make(const sc2::Unit& unit_)
{
    switch (unit_.unit_type.ToType())
    {
        case sc2::UNIT_TYPEID::PROTOSS_PROBE:
        case sc2::UNIT_TYPEID::TERRAN_SCV:
        case sc2::UNIT_TYPEID::ZERG_DRONE:
        {
            return std::make_unique<Worker>(unit_);
        }

        default:
            return std::make_unique<WrappedUnit>(unit_);
    }
}

void WrappedUnit::UpdateAPIData(const sc2::Unit& unit)
{
    assert(tag == unit.tag);
    sc2::Unit::operator=(unit);
}

bool WrappedUnit::IsIdle() const
{
    return orders.empty() && !m_order_queued_in_current_step;
}

int WrappedUnit::NumberOfOrders() const
{
    if (m_order_queued_in_current_step)
    {
        return static_cast<int>(orders.size() + 1);
    }
    else
    {
        return static_cast<int>(orders.size());
    }
}

const std::vector<sc2::UnitOrder>& WrappedUnit::GetPreviousStepOrders() const
{
    return orders;
}

sc2::UnitTypeData* WrappedUnit::GetTypeData() const
{
    return gAPI->observer().GetUnitTypeData(this->unit_type);
}

bool WrappedUnit::HasAttribute(sc2::Attribute attribute_) const
{
    auto data = GetTypeData();
    return std::find(data->attributes.begin(), data->attributes.end(), attribute_) != data->attributes.end();
}

WrappedUnit* WrappedUnit::GetAttachedAddon() const
{
    return gAPI->observer().GetUnit(this->add_on_tag);
}

WrappedUnit::Attackable WrappedUnit::CanAttack(const WrappedUnit* other_) const
{
    auto myUnitData = gAPI->observer().GetUnitTypeData(unit_type);

    bool canAttack = false;
    for (auto& weapon : myUnitData->weapons)
    {
        if (sc2::Distance3D(pos, other_->pos) < weapon.range)
        {
            continue;
        }

        if (weapon.type == sc2::Weapon::TargetType::Any)
        {
            canAttack = true;
        }
        else if (weapon.type == sc2::Weapon::TargetType::Ground && !other_->is_flying)
        {
            canAttack = true;
        }
        else if (weapon.type == sc2::Weapon::TargetType::Air && other_->is_flying)
        {
            canAttack = true;
        }

        if (canAttack)
        {
            break;
        }
    }

    if (!canAttack)
    {
        return Attackable::no;
    }

    if (other_->cloak == sc2::Unit::Cloaked)
    {
        return Attackable::need_scan;
    }

    return Attackable::yes;
}

bool WrappedUnit::CanAttackFlying() const
{
    auto myUnitData = gAPI->observer().GetUnitTypeData(unit_type);

    for (auto& weapon : myUnitData->weapons)
    {
        if (weapon.type == sc2::Weapon::TargetType::Any
        || weapon.type == sc2::Weapon::TargetType::Air)
        {
            return true;
        }
    }

    return false;
}

int WrappedUnit::GetValue() const
{
    auto typeData = GetTypeData();
    return typeData->mineral_cost + static_cast<int>(typeData->vespene_cost * VespeneCostMod);
}

sc2::Point3D WrappedUnit::GetPos() const
{
    return pos;
}
