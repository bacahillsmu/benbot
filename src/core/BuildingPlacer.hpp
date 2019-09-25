#pragma once

#include <memory>

class BuildingPlacer
{

public:

	void OnGameStart();


};




// ----------------------------------------------------------------------------
extern std::unique_ptr<BuildingPlacer> gBuildingPlacer;