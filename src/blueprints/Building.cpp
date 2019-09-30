// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Building.h"
#include "Hub.h"
#include "core/API.h"
#include "core/Helpers.h"
#include "core/BuildingPlacer.hpp"

bool Building::Build(Order* order_)
{
    bool reserveAddonSpace = IsAddonBuilding()(order_->unit_type_id);
    sc2::Point3D position = gBuildingPlacer->ReserveBuildingSpace(*order_, reserveAddonSpace);
    if(position != sc2::Point3D(0.0f, 0.0f, 0.0f)) // This is a lazy way attempt to mimic std::optional.
    {
        return gHub->AssignBuildTask(order_, position);
    }

    return false;

    // Find place to build the structure
//     sc2::Point3D base = gAPI->observer().StartingLocation();
//     sc2::Point2D point;
// 
//     unsigned attempt = 0;
//     do {
//         point.x = base.x + sc2::GetRandomScalar() * 15.0f;
//         point.y = base.y + sc2::GetRandomScalar() * 15.0f;
// 
//         if (++attempt > 150)
//         {
//             return false;
//         }
//     } while (!gAPI->query().CanBePlaced(*order_, point));
// 
//     return gHub->AssignBuildTask(order_, point);
}
