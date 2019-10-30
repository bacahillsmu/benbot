// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill

#include "Addon.h"
#include "core/WrappedUnit.hpp"
#include "core/API.h"
#include "core/Helpers.h"
#include "Hub.h"


bool Addon::CanBeBuilt(const Order* order_)
{
    return GetValidAssignee(order_) != nullptr;
}

// ----------------------------------------------------------------------------
bool Addon::Build(Order* order_)
{
    WrappedUnit* assignee = GetValidAssignee(order_);

    if(assignee)
    {
        if (gHub->AssignBuildingProduction(order_, assignee))
        {
            gAPI->action().Build(*order_);
            return true;
        }
    }
    return false;
}

// ----------------------------------------------------------------------------
sc2::UNIT_TYPEID Addon::GetParentStructureFromAbilityId(sc2::ABILITY_ID abilityId)
{
    switch (abilityId)
    {
        case sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS:
        case sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS:
        {
            return sc2::UNIT_TYPEID::TERRAN_BARRACKS;
        }

        case sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY:
        case sc2::ABILITY_ID::BUILD_REACTOR_FACTORY:
        {
            return sc2::UNIT_TYPEID::TERRAN_FACTORY;
        }

        case sc2::ABILITY_ID::BUILD_TECHLAB_STARPORT:
        case sc2::ABILITY_ID::BUILD_REACTOR_STARPORT:
        {
            return sc2::UNIT_TYPEID::TERRAN_STARPORT;
        }

        default:
        {
            return sc2::UNIT_TYPEID::INVALID;
        }
    }
}

// ----------------------------------------------------------------------------
WrappedUnit* Addon::GetValidAssignee(const Order* order_)
{
    // We return this pointer;
    WrappedUnit* assignee = nullptr;

    // There is a bug with using the actual parentBuilding to check addon placement, fake it with another 2x2 building;
    Order supplyDepotOrder(*gAPI->observer().GetUnitTypeData(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT));
    sc2::UNIT_TYPEID buildingType = GetParentStructureFromAbilityId(order_->ability_id);

    // If the order passed in does not already have an assignee, then find a suitable parentBuilding;
    if(order_->assignee == sc2::NullTag)
    {
        // Get parentBuildings that are idle and have no addon;
        WrappedUnits parentBuildings = gAPI->observer().GetUnits(
            MultiFilter(MultiFilter::Selector::And,
            {
                IsIdleUnit(buildingType, false),
                HasAddon(sc2::UNIT_TYPEID::INVALID)
            }),
            sc2::Unit::Alliance::Self);

        // Go through our parentBuildings;
        for(auto& building : parentBuildings)
        {
            if(gAPI->query().CanBePlaced(supplyDepotOrder, GetTerranAddonPosition(gAPI->observer().GetUnit(building->tag))))
            {
                assignee = building;
                break;
            }
        }

        // No building could be found in the parentBuildings;
        if(!assignee)
        {
            return nullptr;
        }
    }
    else
    {
        // Double check the addon can be placed on the assignee from the order passed in;
        if (!gAPI->query().CanBePlaced(supplyDepotOrder, GetTerranAddonPosition(order_->assignee)))
        {
            return nullptr;
        }
    }

    return assignee;
}
