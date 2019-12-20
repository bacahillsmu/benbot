// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Dispatcher.h"
#include "Historican.h"
#include "Hub.h"
#include "core/API.h"
#include "core/Helpers.h"
#include "core/Map.h"
#include "core/Timer.h"
#include "core/BuildingPlacer.hpp"
#include "plugins/ChatterBox.h"
#include "plugins/Diagnosis.h"
#include "plugins/Miner.h"
#include "plugins/RepairMan.h"
#include "plugins/QuarterMaster.h"
#include "plugins/Combat.hpp"

#include "strategies/terran/ReaperRush.hpp"

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>

// ----------------------------------------------------------------------------
Dispatcher::Dispatcher(const std::string& opponent_id_)
    : m_builder(new Builder())
{
    gAPI = std::make_unique<API::Interface>(Actions(), Control(), Debug(), Observation(), Query());

    gBuildingPlacer = std::make_unique<BuildingPlacer>();
    m_plugins.reserve(10);

    if (opponent_id_.empty())
    {
        return;
    }

    gHistory.info() << "Playing against an opponent with id " << opponent_id_ << std::endl;
}

// ----------------------------------------------------------------------------
void Dispatcher::OnGameStart()
{
    m_plugins.clear();
    gHistory.info() << "New game started!" << std::endl;

    gAPI->Init();

    sc2::Race current_race = gAPI->observer().GetCurrentRace();

    Timer clock;
    clock.Start();
    gHub = std::make_unique<Hub>(current_race, CalculateExpansionLocations());
    float duration = clock.Finish();
    gHistory.info() << "Calculate Expansions took: " << duration << " ms" << std::endl;

    gOverseerMap = std::make_unique<Overseer::MapImpl>();

    clock.Start();
    gOverseerMap->setBot(this);
    gOverseerMap->initialize();
    gHistory.info() << "Overseer has been Initialized!" << std::endl;
    gBuildingPlacer->OnGameStart();
    gHistory.info() << "Building Placer has been Initialized!" << std::endl;

    duration = clock.Finish();
    gHistory.info() << "Map calculations took: " << duration << " ms" << std::endl;
    gHistory.info() << "Tiles in start region: " << gOverseerMap->getNearestRegion(gAPI->observer().StartingLocation())->getTilePositions().size() << std::endl;
    gHistory.info() << "We start in RegionID: " << gOverseerMap->getNearestRegion(gAPI->observer().StartingLocation())->getId() << std::endl;

    m_plugins.emplace_back(new Miner());
    m_plugins.emplace_back(new QuarterMaster());
    m_plugins.emplace_back(new RepairMan());
    m_plugins.emplace_back(new ChatterBox());
    m_plugins.emplace_back(new Combat());

    m_plugins.emplace_back(new ReaperRush());



#ifdef DEBUG
    m_plugins.emplace_back(new Diagnosis());
#endif

    for (const auto& i : m_plugins)
    {
        i->OnGameStart(m_builder.get());
    }
}

// ----------------------------------------------------------------------------
void Dispatcher::OnGameEnd()
{
    gHistory.info() << "Game over!" << std::endl;

    for (const auto& i : m_plugins)
        i->OnGameEnd();
}

// ----------------------------------------------------------------------------
void Dispatcher::OnBuildingConstructionComplete(const sc2::Unit* building_)
{
    gHistory.info() << sc2::UnitTypeToName(building_->unit_type) <<
        ": construction complete" << std::endl;
}

// ----------------------------------------------------------------------------
void Dispatcher::OnStep()
{
    Timer clock;
    clock.Start();

    gAPI->OnStep();
    gHub->OnStep(); // This is currently empty;

    // Figure out what our next steps are around here;
    // Once we make it past our initial build order, we need to kick in some sort of
    //  planning or goal-orientated logic;

    for (const auto& i : m_plugins)
    {
        i->OnStep(m_builder.get());
    }

    // The Builder will attempt to make units and buildings;
    m_builder->OnStep();

    float duration = clock.Finish();
    
    if (duration > 60.0f) // 60ms is disqualification threshold of the ladder;
    {
        gHistory.error() << "Step processing took: " << duration << " ms" << std::endl;
    }
    if (duration > 44.4f) // 44.4ms is highest allowed step time by the ladder
    {
        gHistory.warning() << "Step processing took: " << duration << " ms" << std::endl;
    }
}

// ----------------------------------------------------------------------------
void Dispatcher::OnUnitCreated(const sc2::Unit* unit_)
{
    WrappedUnit* unit = gAPI->WrapAndUpdateUnit(unit_);

    // NOTE (alkurbatov): Could be just a worker exiting a refinery.
    if (unit_->alliance != sc2::Unit::Alliance::Self || IsWorkerWithJob(Worker::Job::GATHERING_VESPENE)(unit))
    {
        return;
    }

    gHistory.info() << sc2::UnitTypeToName(unit_->unit_type) << " was created" << std::endl;

    gHub->OnUnitCreated(unit);

    for (const auto& i : m_plugins)
    {
        i->OnUnitCreated(unit, m_builder.get());
    }
}

// ----------------------------------------------------------------------------
void Dispatcher::OnUnitIdle(const sc2::Unit* unit_)
{
    WrappedUnit* unit = gAPI->WrapAndUpdateUnit(unit_);

    for (const auto& i : m_plugins)
    {
        i->OnUnitIdle(unit, m_builder.get());
    }
}

// ----------------------------------------------------------------------------
void Dispatcher::OnUnitDestroyed(const sc2::Unit* unit_)
{
    WrappedUnit* unit = gAPI->WrapAndUpdateUnit(unit_);

    if (unit_->alliance != sc2::Unit::Alliance::Self)
    {
        return;
    }

    gHistory.info() << sc2::UnitTypeToName(unit->unit_type) << " was destroyed" << std::endl;

    gHub->OnUnitDestroyed(unit);

    for (const auto& i : m_plugins)
    {
        i->OnUnitDestroyed(unit, m_builder.get());
    }
}

// ----------------------------------------------------------------------------
void Dispatcher::OnUpgradeCompleted(sc2::UpgradeID id_)
{
    gHistory.info() << sc2::UpgradeIDToName(id_) << " completed" << std::endl;

    for (const auto& i : m_plugins)
    {
        i->OnUpgradeCompleted(id_);
    }
}

// ----------------------------------------------------------------------------
void Dispatcher::OnError(const std::vector<sc2::ClientError>& client_errors,
        const std::vector<std::string>& protocol_errors /*= {}*/)
{
    for (const auto i : client_errors)
    {
        gHistory.error() << "Encountered client error: " << static_cast<int>(i) << std::endl;
    }

    for (const auto& i : protocol_errors)
    {
        gHistory.error() << "Encountered protocol error: " << i << std::endl;
    }
}
