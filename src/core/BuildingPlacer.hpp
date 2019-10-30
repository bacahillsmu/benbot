#pragma once

#include "Order.h"

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>
#include "Overseer/src/Region.h"

class BuildingPlacer
{

public:

	void OnGameStart();
	void OnUnitCreated(WrappedUnit* unit_);
	void OnUnitDestroyed(WrappedUnit* unit_);
	void OnUnitEnterVision(WrappedUnit* unit_);

	// Point Hash;
	struct Point2DIHasher
	{
		std::size_t operator()(const sc2::Point2DI& k) const
		{
			// Knuth's hash
			auto return_value = static_cast<size_t>(k.x);
			return_value *= 2654435761u;
			return return_value ^ static_cast<size_t>(k.y);
		}
	};

	// Contain a Region from Overseer in a wrapper with extra
	//  information about the position of the region
	//  and tiles that can built on;
	struct RegionWrapper
	{
		explicit RegionWrapper(std::shared_ptr<Overseer::Region> region_)
			:m_region(std::move(region_))
		{
		}

		const std::shared_ptr<Overseer::Region> m_region;
		std::unordered_map<sc2::Point2DI, std::shared_ptr<Overseer::Tile>, Point2DIHasher> m_buildableTiles;
		
		int regionMinX = std::numeric_limits<int>::max();
		int regionMaxX = 0;
		int regionMinY = std::numeric_limits<int>::max();
		int regionMaxY = 0;
	};
	std::vector<RegionWrapper> m_regions;

	// Enum to help understand what is on a tile;
	enum class TileOccupationStatus
	{
		RESERVED,
		HASBUILDING
	};
	std::unordered_map<sc2::Point2DI, TileOccupationStatus, Point2DIHasher> m_occupiedTiles;

	sc2::Point3D ReserveBuildingSpace(const Order& order_, bool reserveAddonSpace_ = false);
	void FreeReservedBuildingSpace(const sc2::Point3D& buildingPosition_, const sc2::UNIT_TYPEID& buildingType_, bool includeAddonSpace_ = false);
	bool IsGeyserUnoccupied(WrappedUnit* geyser_) const;
	bool ReserveGeyser(WrappedUnit* geyser_);
	
	void AddBuildingToOccupiedTiles(WrappedUnit* unit_, TileOccupationStatus tileOccupationStatus_);
	void RemoveBuildingFromOccupiedTiles(WrappedUnit* unit_);
	bool IsBuildSpaceFree(const sc2::Point2DI& bottomLeftTilePoint_, int width_, int height_, const std::unordered_map<sc2::Point2DI, std::shared_ptr<Overseer::Tile>, Point2DIHasher>& buildableTiles_) const;
	void MarkTilesAsReserved(const sc2::Point2DI& bottomLeftTilePoint_, int width_, int height_);
	
	static constexpr int DefaultBuildingMargin = 2;
	static constexpr int AddonSize = 2;
};

// ----------------------------------------------------------------------------
extern std::unique_ptr<BuildingPlacer> gBuildingPlacer;