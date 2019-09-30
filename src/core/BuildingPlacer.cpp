#include "BuildingPlacer.hpp"
#include "Helpers.h"
#include "Hub.h"
#include "API.h"


// ----------------------------------------------------------------------------
void BuildingPlacer::OnGameStart()
{
    sc2::Point2DI point;
    for(const auto& region : gOverseerMap->getRegions())
    {
        RegionWrapper regionWrapper(region);
        for(const auto& tile : region->getTilePositions())
        {
            const auto& tileTerrain = tile.second->getTileTerrain();
            if(tileTerrain == Overseer::TileTerrain::buildAndPath
            || tileTerrain == Overseer::TileTerrain::build)
            {
                point.x = (int)tile.first.x;
                point.y = (int)tile.first.y;
                regionWrapper.m_buildableTiles[point] = tile.second;

                if(regionWrapper.regionMinX > point.x)
                {
                    regionWrapper.regionMinX = point.x;
                }
                else if (regionWrapper.regionMaxX < point.x)
                {
                    regionWrapper.regionMaxX = point.x;
                }

                if (regionWrapper.regionMinY > point.y)
                {
                    regionWrapper.regionMinY = point.y;
                }
                else if (regionWrapper.regionMaxY < point.y)
                {
                    regionWrapper.regionMaxY = point.y;
                }
            }

            m_regions.emplace_back(std::move(regionWrapper));
        }
    }
}

// ----------------------------------------------------------------------------
void BuildingPlacer::OnUnitCreated(const sc2::Unit& unit_)
{
    if(IsBuilding()(unit_)
    && !unit_.is_flying)
    {
        AddBuildingToOccupiedTiles(unit_, TileOccupationStatus::HASBUILDING);
    }
}

// ----------------------------------------------------------------------------
void BuildingPlacer::OnUnitDestroyed(const sc2::Unit& unit_)
{
    if(IsBuilding()(unit_)
    && !unit_.is_flying)
    {
        RemoveBuildingFromOccupiedTiles(unit_);
    }
}

// ----------------------------------------------------------------------------
void BuildingPlacer::OnUnitEnterVision(const sc2::Unit& unit_)
{
    if(IsBuilding()(unit_)
    && !unit_.is_flying)
    {
        AddBuildingToOccupiedTiles(unit_, TileOccupationStatus::HASBUILDING);
    }
}

// ----------------------------------------------------------------------------
sc2::Point3D BuildingPlacer::ReserveBuildingSpace(const Order& order_, bool reserveAddonSpace_ /*= false*/)
{
    assert(IsBuilding()(order_.unit_type_id));

    if(reserveAddonSpace_)
    {
        assert(IsAddonBuilding()(order_.unit_type_id));
    }

    sc2::Point2DI point;
    sc2::Point2DI addonPoint;   // May not be used, dependent on reserveAddonSpace_;
    float footprintRadius = gAPI->observer().GetAbilityData(order_.ability_id).footprint_radius;
    int width = (int)(footprintRadius * 2.0f);
    int height = width;
    int buildingMargin;         // This is used to make sure gaps are between buildings for unit movement;

    // Reserve space first for Expansions;
    if(order_.unit_type_id == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER)
    {
        for(const auto& expansion : gHub->GetExpansions())
        {
            if(expansion->m_owner == Owner::NEUTRAL)
            {
                sc2::Point3D townHallLocation = expansion->m_townHallLocation;
                point.x = (int)(townHallLocation.x - footprintRadius);
                point.y = (int)(townHallLocation.y - footprintRadius);

                const auto& closestRegion = gOverseerMap->getNearestRegion(townHallLocation);
                auto& closestRegionBuildableTiles = m_regions[closestRegion->getId() - 1].m_buildableTiles;
                
                if(IsBuildSpaceFree(point, width, height, closestRegionBuildableTiles))
                {
                    if(gAPI->query().CanBePlaced(order_, townHallLocation))
                    {
                        MarkTilesAsReserved(point, width, height);
                        return townHallLocation;
                    }
                }
            }
        }
    }

    buildingMargin = order_.unit_type_id == sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT ? 0 : DefaultBuildingMargin;

    for (const auto& expansion : gHub->GetExpansions())
    {
        const auto& closestRegion = gOverseerMap->getNearestRegion(expansion->m_townHallLocation);
        auto& wrappedRegion = m_regions[closestRegion->getId() - 1];

        for (int x = wrappedRegion.regionMinX; x <= wrappedRegion.regionMaxX; x++)
        {
            for (int y = wrappedRegion.regionMinY; y <= wrappedRegion.regionMaxY; y++)
            {
                point.x = x;
                point.y = y;
                if (IsBuildSpaceFree(point, width + (buildingMargin * 2), height + (buildingMargin * 2), wrappedRegion.m_buildableTiles))
                {
                    if (reserveAddonSpace_)
                    {
                        addonPoint = point;
                        addonPoint.x += width;
                        if (IsBuildSpaceFree(addonPoint, AddonSize + (buildingMargin * 2), AddonSize + (buildingMargin * 2), wrappedRegion.m_buildableTiles))
                        {
                            addonPoint.x += buildingMargin;
                            addonPoint.y += buildingMargin;
                        }
                        else
                        {
                            continue;
                        }
                    }

                    point.x += buildingMargin;
                    point.y += buildingMargin;
                    sc2::Point3D position(point.x + footprintRadius, point.y + footprintRadius, wrappedRegion.m_region->getTilePositions().front().second->getTerrainHeight());

                    if (gAPI->query().CanBePlaced(order_, position))
                    {
                        MarkTilesAsReserved(point, width, height);
                        if (reserveAddonSpace_)
                        {
                            MarkTilesAsReserved(addonPoint, AddonSize, AddonSize);
                        }
                        return position;
                    }
                }
            }
        }
    }

    return sc2::Point3D(0.0f, 0.0f, 0.0f);

}

// ----------------------------------------------------------------------------
void BuildingPlacer::FreeReservedBuildingSpace(const sc2::Point3D& buildingPosition_, const sc2::UNIT_TYPEID& buildingType_, bool includeAddonSpace_ /*= false*/)
{
    assert(IsBuilding()(buildingType_));

    if (includeAddonSpace_)
    {
        assert(IsAddonBuilding()(buildingType_));
    }

    const auto& abilityID = gAPI->observer().GetUnitTypeData(buildingType_).ability_id;
    float footprintRadius = gAPI->observer().GetAbilityData(abilityID).footprint_radius;
    int minX = (int)(buildingPosition_.x - footprintRadius);
    int minY = (int)(buildingPosition_.y - footprintRadius);
    int width = (int)(footprintRadius * 2.0f);
    int height = width;

    sc2::Point2DI point;
    for (int x = minX; x < (minX + width); x++)
    {
        for (int y = minY; y < (minY + height); y++)
        {
            point.x = x;
            point.y = y;
            auto tile = m_occupiedTiles.find(point);
            if (tile != m_occupiedTiles.end()
            && tile->second == TileOccupationStatus::RESERVED)
            {
                m_occupiedTiles.erase(tile);
            }
            else
            {
                assert(false && "Tried to free non reserved building space");
            }
        }
    }

    if (includeAddonSpace_)
    {
        sc2::Point2D addonPosition = GetTerranAddonPosition(buildingPosition_);
        FreeReservedBuildingSpace(sc2::Point3D(addonPosition.x, addonPosition.y, buildingPosition_.z), sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT, false);
    }
}

// ----------------------------------------------------------------------------
bool BuildingPlacer::IsGeyserUnoccupied(const sc2::Unit& geyser_) const
{
    assert(IsGeyser()(geyser_));

    auto radius = geyser_.radius;
    int width = (int)(radius * 2.0f);
    int height = width;

    sc2::Point2DI bottomLeftTile;
    bottomLeftTile.x = (int)(geyser_.pos.x - radius);
    bottomLeftTile.y = (int)(geyser_.pos.y - radius);

    sc2::Point2DI point;
    for (int x = bottomLeftTile.x; x < (bottomLeftTile.x + width); x++)
    {
        for (int y = bottomLeftTile.y; y < (bottomLeftTile.y + height); y++)
        {
            point.x = x;
            point.y = y;
            if (m_occupiedTiles.count(point) > 0)
            {
                return false;
            }
        }
    }

    return true;
}

// ----------------------------------------------------------------------------
bool BuildingPlacer::ReserveGeyser(const sc2::Unit& geyser_)
{
    if (IsGeyserUnoccupied(geyser_))
    {
        auto radius = geyser_.radius;
        int width = static_cast<int>(radius * 2);
        int height = width;

        sc2::Point2DI bottomLeftTile;
        bottomLeftTile.x = (int)(geyser_.pos.x - radius);
        bottomLeftTile.y = (int)(geyser_.pos.y - radius);

        MarkTilesAsReserved(bottomLeftTile, width, height);

        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------
void BuildingPlacer::AddBuildingToOccupiedTiles(const sc2::Unit& unit_, TileOccupationStatus tileOccupationStatus_)
{
    assert(IsBuilding()(unit_));

    if (unit_.is_flying)
    {
        return;
    }

    auto unitTypeData = gAPI->observer().GetUnitTypeData(unit_.unit_type);
    if(!unitTypeData.tech_alias.empty())
    {
        unitTypeData = gAPI->observer().GetUnitTypeData(unitTypeData.tech_alias.front());
    }

    float footprintRadius = gAPI->observer().GetAbilityData(unitTypeData.ability_id).footprint_radius;
    int minX = (int)(unit_.pos.x - footprintRadius);
    int minY = (int)(unit_.pos.y - footprintRadius);
    int width = (int)(footprintRadius * 2.0f);
    int height = width;

    sc2::Point2DI point;
    for(int x = minX; x < (minX + width); x++)
    {
        for(int y = minY; y < (minY + height); y++)
        {
            point.x = x;
            point.y = y;
            m_occupiedTiles[point] = tileOccupationStatus_;
        }
    }
}

// ----------------------------------------------------------------------------
void BuildingPlacer::RemoveBuildingFromOccupiedTiles(const sc2::Unit& unit_)
{
    assert(IsBuilding()(unit_));

    if (unit_.is_flying)
    {
        return;
    }

    auto unitTypeData = gAPI->observer().GetUnitTypeData(unit_.unit_type);
    if (!unitTypeData.tech_alias.empty())
    {
        unitTypeData = gAPI->observer().GetUnitTypeData(unitTypeData.tech_alias.front());
    }

    float footprintRadius = gAPI->observer().GetAbilityData(unitTypeData.ability_id).footprint_radius;
    int minX = (int)(unit_.pos.x - footprintRadius);
    int minY = (int)(unit_.pos.y - footprintRadius);
    int width = (int)(footprintRadius * 2.0f);
    int height = width;

    sc2::Point2DI point;
    for (int x = minX; x < (minX + width); x++)
    {
        for (int y = minY; y < (minY + height); y++)
        {
            point.x = x;
            point.y = y;
            m_occupiedTiles.erase(point);
        }
    }
}

// ----------------------------------------------------------------------------
bool BuildingPlacer::IsBuildSpaceFree(const sc2::Point2DI& bottomLeftTilePoint_, int width_, int height_, const std::unordered_map<sc2::Point2DI, std::shared_ptr<Overseer::Tile>, Point2DIHasher>& buildableTiles_) const
{
    sc2::Point2DI point;
    for(int x = bottomLeftTilePoint_.x; x < (bottomLeftTilePoint_.x + width_); x++)
    {
        for(int y = bottomLeftTilePoint_.y; y < (bottomLeftTilePoint_.y + height_); y++)
        {
            point.x = x;
            point.y = y;
            if(m_occupiedTiles.count(point) > 0
            || buildableTiles_.count(point) == 0)
            {
                return false;
            }
        }
    }

    return true;
}

// ----------------------------------------------------------------------------
void BuildingPlacer::MarkTilesAsReserved(const sc2::Point2DI& bottomLeftTilePoint_, int width_, int height_)
{
    sc2::Point2DI point;
    for (int x = bottomLeftTilePoint_.x; x < (bottomLeftTilePoint_.x + width_); x++)
    {
        for (int y = bottomLeftTilePoint_.y; y < (bottomLeftTilePoint_.y + height_); y++)
        {
            point.x = x;
            point.y = y;
            const auto& result = m_occupiedTiles.insert({ point, TileOccupationStatus::RESERVED });
            assert(result.second && "Tried to mark already occupied tile as reserved");
        }
    }
}

// ----------------------------------------------------------------------------
std::unique_ptr<BuildingPlacer> gBuildingPlacer;
