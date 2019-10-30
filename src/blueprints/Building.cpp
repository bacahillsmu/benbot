// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Building.h"
#include "Hub.h"
#include "Historican.h"
#include "core/API.h"
#include "core/Helpers.h"
#include "core/BuildingPlacer.hpp"

bool Building::CanBeBuilt(const Order* order_)
{
    order_;
    return FreeWorkerExists(true);
}

bool Building::Build(Order* order_)
{
    bool reserveAddonSpace = IsAddonBuilding()(order_->unit_type_id);
    sc2::Point3D position = gBuildingPlacer->ReserveBuildingSpace(*order_, reserveAddonSpace);

    //gHistory.info() << "Blueprint Builder: Attempted position was: (" << position.x << ", " << position.y << ", " << position.z << ") " << std::endl;
    if(position != sc2::Point3D(0.0f, 0.0f, 0.0f)) // This is a lazy way attempt to mimic std::optional.
    {
        return gHub->AssignBuildTask(order_, position);
    }

    return false;
}
