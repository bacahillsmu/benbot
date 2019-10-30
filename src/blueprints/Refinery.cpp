// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Hub.h"
#include "Refinery.h"
#include "core/API.h"
#include "core/Helpers.h"
#include "core/BuildingPlacer.hpp"

bool Refinery::CanBeBuilt(const Order* order_)
{
    order_;
    return true;
}

bool Refinery::Build(Order* order_)
{
    WrappedUnits geysers = gAPI->observer().GetUnits(IsFreeGeyser(), sc2::Unit::Alliance::Neutral);

    for (auto& expansion : gHub->GetOurExpansions())
    {
        for (auto& geyserPosition : expansion->geysersPosition)
        {
            WrappedUnit* geyser = geysers.GetClosestUnit(geyserPosition);

            if (geyser->vespene_contents > 0
             && gBuildingPlacer->ReserveGeyser(geyser))
            {
                WrappedUnit* unit = GetClosestFreeWorker(geyser->pos);
                Worker* worker = (Worker*)unit;

                if (!worker)
                {
                    WrappedUnit* gasUnit = GetClosestFreeWorker(geyser->pos, true);
                    worker = (Worker*)gasUnit;
                }

                if (worker)
                {
                    worker->BuildRefinery(order_, geyser);
                    return true;
                }
                else
                {
                    assert(false && "Refinery space reserved but no free worker was found!");
                }
            }
        }
    }
    return false;
}
