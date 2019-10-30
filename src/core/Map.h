// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include "core/WrappedUnits.hpp"

#include <sc2api/sc2_unit.h>
#include <Overseer/src/MapImpl.h>

#include <list>
#include <vector>

enum Owner
{
    NEUTRAL = 0,
    CONTESTED = 1,
    ENEMY = 2,
    SELF = 3,
};

struct Expansion
{

public:

    explicit Expansion(const sc2::Point3D& town_hall_location_);

    const WrappedUnit* m_townHall;
    sc2::Point3D m_townHallLocation;
    sc2::Unit::Alliance m_alliance;
    Owner m_owner;
    std::vector<sc2::Point2D> geysersPosition;
    std::unordered_map<std::shared_ptr<Expansion>, float> m_expansionGroundDistances;
    std::vector<WrappedUnit*> refineries;

    float GetDistanceToOtherExpansion(const std::shared_ptr<Expansion>& otherExpansion_) const;

    float distanceTo(const std::shared_ptr<Expansion>& other_) const {
        auto itr = m_expansionGroundDistances.find(other_);
        if (itr != m_expansionGroundDistances.end())
        {
            return itr->second;
        }
        return 0.0f;
    }
};

//typedef std::vector<Expansion> Expansions;
typedef std::vector<std::shared_ptr<Expansion>> Expansions;


// NOTE (alkurbatov): Slightly optimized version of the built in function.
Expansions CalculateExpansionLocations();
bool IsPointReachable(const WrappedUnit* unit_, const sc2::Point2D& point);

extern std::unique_ptr<Overseer::MapImpl> gOverseerMap;
