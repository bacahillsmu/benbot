// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

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

    const sc2::Unit* m_townHall;
    sc2::Point3D m_townHallLocation;
    sc2::Unit::Alliance m_alliance;
    Owner m_owner;
    std::vector<sc2::Point2D> geysersPosition;
    std::unordered_map<std::shared_ptr<Expansion>, float> m_expansionGroundDistances;


    float GetDistanceToOtherExpansion(const std::shared_ptr<Expansion>& otherExpansion_) const;
};

//typedef std::vector<Expansion> Expansions;
typedef std::vector<std::shared_ptr<Expansion>> Expansions;


// NOTE (alkurbatov): Slightly optimized version of the built in function.
Expansions CalculateExpansionLocations();

extern std::unique_ptr<Overseer::MapImpl> gOverseerMap;
