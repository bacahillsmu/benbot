// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Helpers.h"
#include "Historican.h"
#include "Map.h"

#include "sc2lib/sc2_search.h"

#include <cmath>

// ----------------------------------------------------------------------------
namespace {

const float PI = 3.1415927f;
constexpr float PatchNeighborDistance = 5.0f;
constexpr float DistanceErrorMargin = 10.0f;
constexpr int SearchMinOffset = -10;
constexpr int SearchMaxOffset = 10;
constexpr sc2::ABILITY_ID TestAbility = sc2::ABILITY_ID::BUILD_COMMANDCENTER;

// ----------------------------------------------------------------------------
struct MineralLine
{
    explicit MineralLine(const sc2::Point3D& initialPatchPoint_);

    void AddMineralPatch(const sc2::Point3D& additionalPatchPoint_);
    float GetMineralPatchHeight() const;
    sc2::Point2D Center() const;

    sc2::Point2D m_townHallLocation = sc2::Point2D(0.0f, 0.0f);
    std::vector<sc2::Point3D> m_mineralPatchLocations;
};

// ----------------------------------------------------------------------------
MineralLine::MineralLine(const sc2::Point3D& initialPatchPoint_)
{
    m_mineralPatchLocations.reserve(8);
    m_mineralPatchLocations.push_back(initialPatchPoint_);
}

// ----------------------------------------------------------------------------
void MineralLine::AddMineralPatch(const sc2::Point3D& additionalPatchPoint_)
{
    m_mineralPatchLocations.push_back(additionalPatchPoint_);
}

// ----------------------------------------------------------------------------
float MineralLine::GetMineralPatchHeight() const
{
    if(m_mineralPatchLocations.empty())
    {
        return 0.0f;
    }

    return m_mineralPatchLocations.back().z;
}

// ----------------------------------------------------------------------------
sc2::Point2D MineralLine::Center() const
{
    if(m_mineralPatchLocations.empty())
    {
        return sc2::Point2D(0.0f, 0.0f);
    }

    float x = 0;
    float y = 0;
    for (auto& mineralPatch : m_mineralPatchLocations)
    {
        x += mineralPatch.x;
        y += mineralPatch.y;
    }

    float centerX = x / m_mineralPatchLocations.size();
    float centerY = y / m_mineralPatchLocations.size();

    return sc2::Point2D(centerX, centerY);
}

// ----------------------------------------------------------------------------
void CalculateGroundDistances(Expansions& expansions_)
{
    for(auto& expansion : expansions_)
    {
        std::vector<sc2::QueryInterface::PathingQuery> queries;
        queries.reserve(expansions_.size());

        for(auto& expansion2 : expansions_)
        {
            if(expansion == expansion2 || expansion->GetDistanceToOtherExpansion(expansion2) != 0.0f)
            {
                continue;
            }

            sc2::QueryInterface::PathingQuery query;
            query.start_ = expansion->m_townHallLocation;
            query.end_ = expansion2->m_townHallLocation;
            queries.push_back(query);
        }

        std::vector<float> results = gAPI->query().PathingDistances(queries);

        std::size_t i = 0;
        for (auto& expansion2 : expansions_)
        {
            if (expansion == expansion2 || expansion->GetDistanceToOtherExpansion(expansion2) != 0.0f)
            {
                continue;
            }

            expansion->m_expansionGroundDistances[expansion2] = results[i];
            expansion2->m_expansionGroundDistances[expansion] = results[i];
            i++;
        }
    }
}

// ----------------------------------------------------------------------------
WrappedUnits GetMineralPatchesAtBase(const sc2::Point3D& baseLocation_)
{
    return gAPI->observer().GetUnits(
        MultiFilter(MultiFilter::Selector::And,
        {
                IsMineralPatch(),
                IsWithinDistance(baseLocation_, 15.0f)
        }),
        sc2::Unit::Alliance::Self);
}

// ----------------------------------------------------------------------------
sc2::Point3D GetCenterBehindMinerals(const sc2::Point3D& baseLocation_)
{
    auto mineralPatches = GetMineralPatchesAtBase(baseLocation_);

    if (mineralPatches.empty())
    {
        return baseLocation_;
    }

    sc2::Point3D resourceCenter;
    float maxDist = 0;
    for (const auto& mineralPatch : mineralPatches)
    {
        resourceCenter += mineralPatch->pos;
        float dist = sc2::Distance2D(mineralPatch->pos, baseLocation_);
        if (maxDist < dist)
        {
            maxDist = dist;
        }
    }

    resourceCenter.x /= (float)mineralPatches.size();
    resourceCenter.y /= (float)mineralPatches.size();
    resourceCenter.z = baseLocation_.z;

    // Get direction vector
    auto directionVector = resourceCenter - baseLocation_;
    sc2::Normalize3D(directionVector);

    return baseLocation_ + directionVector * (maxDist + 1.5f);
}

// ----------------------------------------------------------------------------
std::vector<MineralLine> GetMineralLines()
{
    auto mineralPatches = gAPI->observer().GetUnits(
        [](const auto& unit)
        {
            return unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD
                || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750
                || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD
                || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750
                || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD
                || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750
                || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD
                || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD750
                || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD
                || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750
                || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD
                || unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD750;
        }, sc2::Unit::Neutral);

    std::vector<MineralLine> mineralLines;
    mineralLines.reserve(16);
    while (!mineralPatches.empty())
    {
        std::queue<sc2::Point3D> mineralFrontier;
        mineralFrontier.push(mineralPatches.front()->pos);

        MineralLine mineralLine(mineralFrontier.front());
        mineralPatches.erase(mineralPatches.begin());

        while (!mineralPatches.empty() && !mineralFrontier.empty())
        {
            auto mineralPosition = mineralFrontier.front();
            auto closestPatch = mineralPatches.GetClosestUnit(mineralPosition);
            auto distance = Distance2D(closestPatch->pos, mineralPosition);

            if (distance >= PatchNeighborDistance)
            {
                mineralFrontier.pop();
                continue;
            }

            mineralFrontier.push(closestPatch->pos);
            mineralLine.AddMineralPatch(closestPatch->pos);
            mineralPatches.remove(closestPatch);
        }

        mineralLines.push_back(mineralLine);
    }

    gHistory.info() << "Map contains " << mineralLines.size() << " mineral lines" << std::endl;

    return mineralLines;
}

// ----------------------------------------------------------------------------
void CalculateGeysers(Expansions& expansions)
{
    auto geysers = gAPI->observer().GetUnits(IsGeyser(), sc2::Unit::Alliance::Neutral);

    for (auto& geyser : geysers)
    {
        auto closest = expansions[0];
        for (auto& expansion : expansions)
        {
            if (sc2::DistanceSquared2D(geyser->pos, expansion->m_townHallLocation) < sc2::DistanceSquared2D(geyser->pos, closest->m_townHallLocation))
            {
                closest = expansion;
            }
        }
        closest->geysersPosition.emplace_back(geyser->pos);
    }
}

// ----------------------------------------------------------------------------
size_t CalculateQueries(float radius, float step_size, const sc2::Point2D& center, std::vector<sc2::QueryInterface::PlacementQuery>* queries)
{
    sc2::Point2D current_grid(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    sc2::Point2D previous_grid(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    size_t valid_queries = 0;

    // Find a buildable location on the circumference of the sphere
    float loc = 0.0f;
    while (loc < 360.0f)
    {
        sc2::Point2D point((radius * std::cos((loc * PI) / 180.0f)) + center.x,(radius * std::sin((loc * PI) / 180.0f)) + center.y);

        sc2::QueryInterface::PlacementQuery query(sc2::ABILITY_ID::BUILD_COMMANDCENTER, point);

        current_grid = sc2::Point2D(std::floor(point.x), std::floor(point.y));

        if (previous_grid != current_grid)
        {
            queries->push_back(query);
            ++valid_queries;
        }

        previous_grid = current_grid;
        loc += step_size;
    }

    return valid_queries;
}

// ----------------------------------------------------------------------------
struct Cluster
{
    explicit Cluster(uint64_t id_);

    void AddPoint(const sc2::Point3D& point_);
    float Height() const;
    sc2::Point3D Center() const;

    uint64_t id;
    sc2::Point3D center_of_mass;
    sc2::Point3D town_hall_location;
    std::vector<sc2::Point3D> points;
};

// ----------------------------------------------------------------------------
typedef std::vector<Cluster> Clusters;

// ----------------------------------------------------------------------------
Cluster::Cluster(uint64_t id_)
    :id(id_)
{
    points.reserve(10);
}

// ----------------------------------------------------------------------------
void Cluster::AddPoint(const sc2::Point3D& point_)
{
    if (points.empty())
    {
        center_of_mass = point_;
    }
    else
    {
        center_of_mass = (center_of_mass * static_cast<float>(points.size() - 1) + point_) / static_cast<float>(points.size());
    }

    points.push_back(point_);
}

// ----------------------------------------------------------------------------
float Cluster::Height() const
{
    if (points.empty())
    {
        return 0.0f;
    }

    return points.back().z;
}

// ----------------------------------------------------------------------------
sc2::Point3D Cluster::Center() const
{
    return sc2::Point3D(center_of_mass.x, center_of_mass.y, Height());
}

}  // namespace

// ----------------------------------------------------------------------------
Expansion::Expansion(const sc2::Point3D& town_hall_location_)
    :m_townHallLocation(town_hall_location_)
    ,m_owner(Owner::NEUTRAL)
{
}

// ----------------------------------------------------------------------------
float Expansion::GetDistanceToOtherExpansion(const std::shared_ptr<Expansion>& otherExpansion_) const
{
    auto foundExpansion = m_expansionGroundDistances.find(otherExpansion_);
    if (foundExpansion != m_expansionGroundDistances.end())
    {
        return foundExpansion->second;
    }

    return 0.0f;
}

// ----------------------------------------------------------------------------
Expansions CalculateExpansionLocations()
{
    auto mineralLines = GetMineralLines();

    if (mineralLines.empty())
    {
        gHistory.error() << "No expansion locations could be found!" << std::endl;
        return Expansions();
    }

    Expansions expansions;

    auto myTownHall = gAPI->observer().GetUnits(IsUnit(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER), sc2::Unit::Alliance::Self);

    if (!myTownHall.empty())
    {
        for (auto itr = mineralLines.begin(); itr != mineralLines.end(); ++itr)
        {
            if (sc2::Distance2D(itr->Center(), myTownHall.front()->pos) < DistanceErrorMargin)
            {
                auto expansion = std::make_shared<Expansion>(myTownHall.front()->pos);
                expansion->m_alliance = sc2::Unit::Alliance::Self;
                expansion->m_townHall = myTownHall.front();
                expansions.emplace_back(std::move(expansion));
                mineralLines.erase(itr);
                break;
            }
        }
    }

    for (auto& mineralLine : mineralLines)
    {
        auto center = mineralLine.Center();
        center.x = (int)(center.x) + 0.5f;
        center.y = (int)(center.y) + 0.5f;

        std::vector<sc2::QueryInterface::PlacementQuery> queries;
        queries.reserve((SearchMaxOffset - SearchMinOffset + 1) * (SearchMaxOffset - SearchMinOffset + 1));
        for (int x_offset = SearchMinOffset; x_offset <= SearchMaxOffset; x_offset++)
        {
            for (int y_offset = SearchMinOffset; y_offset <= SearchMaxOffset; y_offset++)
            {
                sc2::Point2D pos(center.x + x_offset, center.y + y_offset);
                queries.emplace_back(TestAbility, pos);
            }
        }
        auto results = gAPI->query().CanBePlaced(queries);

        for (int x_offset = SearchMinOffset; x_offset <= SearchMaxOffset; x_offset++)
        {
            for (int y_offset = SearchMinOffset; y_offset <= SearchMaxOffset; y_offset++)
            {
                sc2::Point2D pos(center.x + x_offset, center.y + y_offset);

                int index = (x_offset + 0 - SearchMinOffset) * (SearchMaxOffset - SearchMinOffset + 1) + (y_offset + 0 - SearchMinOffset);
                assert(0 <= index && index < (int)(results.size()));
                if (!results[static_cast<std::size_t>(index)])
                {
                    continue;
                }

                if (mineralLine.m_townHallLocation != sc2::Point2D(0.0f, 0.0f))
                {
                    if (sc2::DistanceSquared2D(center, pos) < sc2::DistanceSquared2D(center, mineralLine.m_townHallLocation))
                    {
                        mineralLine.m_townHallLocation = pos;
                    }
                }
                else
                {
                    mineralLine.m_townHallLocation = pos;
                }
            }
        }

        auto p = mineralLine.m_townHallLocation;
        expansions.emplace_back(std::make_shared<Expansion>(sc2::Point3D(p.x, p.y, mineralLine.GetMineralPatchHeight())));
    }

    CalculateGroundDistances(expansions);
    CalculateGeysers(expansions);

    return expansions;
}

// ----------------------------------------------------------------------------
bool IsPointReachable(const sc2::Unit* unit_, const sc2::Point2D& point)
{
    if (!gOverseerMap->valid(point))
    {
        return false;
    }

    const auto& tile = gOverseerMap->getTile(point);
    if (tile->getTileTerrain() == Overseer::TileTerrain::buildAndPath
     || tile->getTileTerrain() == Overseer::TileTerrain::path
     || (unit_->is_flying && tile->getTileTerrain() == Overseer::TileTerrain::build)
     || (unit_->is_flying && tile->getTileTerrain() == Overseer::TileTerrain::flyOnly))
    {
        float distance = gAPI->query().PathingDistance(unit_, point);
        if (distance != 0.0f)
        {
            return true;
        }
    }

    return false;
}

// ----------------------------------------------------------------------------
bool IsPointReachable(const WrappedUnit* unit_, const sc2::Point2D& point)
{
    if (!gOverseerMap->valid(point))
    {
        return false;
    }

    const auto& tile = gOverseerMap->getTile(point);
    if (tile->getTileTerrain() == Overseer::TileTerrain::buildAndPath
    || (tile->getTileTerrain() == Overseer::TileTerrain::path)
    || (unit_->is_flying && tile->getTileTerrain() == Overseer::TileTerrain::build)
    || (unit_->is_flying && tile->getTileTerrain() == Overseer::TileTerrain::flyOnly))
    {
        float distance = gAPI->query().PathingDistance(unit_, point);
        if (distance != 0.0f)
        {
            return true;
        }
    }

    return false;
}

// ----------------------------------------------------------------------------
void RemovePointsUnreachableByUnit(const sc2::Unit* unit_, std::vector<sc2::Point2D>& points_)
{
    std::vector<sc2::QueryInterface::PathingQuery> queries;

    for (auto itr = points_.begin(); itr != points_.end();)
    {
        if (!gOverseerMap->valid(*itr))
        {
            itr = points_.erase(itr);
            continue;
        }

        const auto& tile = gOverseerMap->getTile(*itr);
        if (tile->getTileTerrain() == Overseer::TileTerrain::buildAndPath
         || tile->getTileTerrain() == Overseer::TileTerrain::path
         || (unit_->is_flying && tile->getTileTerrain() == Overseer::TileTerrain::build)
         || (unit_->is_flying && tile->getTileTerrain() == Overseer::TileTerrain::flyOnly))
        {
            sc2::QueryInterface::PathingQuery query;
            query.start_unit_tag_ = unit_->tag;
            query.end_ = *itr;
            queries.emplace_back(query);
            itr++;
        }
        else
        {
            itr = points_.erase(itr);
        }
    }

    const auto& result = gAPI->query().PathingDistances(queries);
    size_t i = 0;
    for (auto itr = points_.begin(); itr != points_.end(); i++)
    {
        if (result.at(i) == 0)
        {
            itr = points_.erase(itr);
        }
        else
        {
            itr++;
        }
    }
}



std::unique_ptr<Overseer::MapImpl> gOverseerMap;