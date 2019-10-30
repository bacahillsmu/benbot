// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include "Order.h"
#include "WrappedUnits.hpp"

#include <sc2api/sc2_control_interfaces.h>
#include <sc2api/sc2_gametypes.h>
#include <sc2api/sc2_interfaces.h>
#include <sc2api/sc2_score.h>

#include <memory>
#include <set>

namespace API
{

    constexpr float OrbitalScanCost = 50.0f;
    constexpr float OrbitalMuleCost = 50.0f;
    constexpr float OrbitalScanRadius = 12.3f;

    typedef std::function<bool(const WrappedUnit* unit)> Filter;

// ----------------------------------------------------------------------------
// Action;
// ----------------------------------------------------------------------------
struct Action
{
    explicit Action(sc2::ActionInterface* action_);

    void Build(const Order& order_);
    void Build(const Order& order_, const WrappedUnit* unit_);
    void Build(const Order& order_, const sc2::Point2D& point_);
    void Attack(const WrappedUnits& units_, const sc2::Point2D& point_);
    void Attack(const WrappedUnit& unit_, const WrappedUnit& target_);
    void Attack(const WrappedUnit& unit_, const sc2::Point2D& point_);
    void MoveTo(const WrappedUnit& unit_, const sc2::Point2D& point_);
    void MoveTo(const WrappedUnits& units_, const sc2::Point2D& point_);
    void Cast(const WrappedUnit& assignee_, sc2::ABILITY_ID ability_);
    void Cast(const WrappedUnit& assignee_, sc2::ABILITY_ID ability_, const WrappedUnit& target_);
    void Cast(const WrappedUnit& assignee_, sc2::ABILITY_ID ability_, const sc2::Point2D& point_);
    void Stop(const WrappedUnit& unit_, bool queue_ = false);
    void Stop(const WrappedUnits& units_, bool queue_ = false);
    void Cancel(const WrappedUnit& assignee_);
    void CancelConstruction(const WrappedUnit& assignee_);
    void OpenGate(const WrappedUnit& assignee_);
    void SendMessage(const std::string& text_);

 private:

    sc2::ActionInterface* m_action;
};

// ----------------------------------------------------------------------------
// Control;
// ----------------------------------------------------------------------------
struct Control
{
    explicit Control(sc2::ControlInterface* control_);

    void SaveReplay();


 private:

    sc2::ControlInterface* m_control;
};

// ----------------------------------------------------------------------------
// Debug;
// ----------------------------------------------------------------------------
struct Debug
{
    explicit Debug(sc2::DebugInterface* debug_);

    void DrawText(const std::string& message_) const;
    void DrawText(const std::string& message_, const sc2::Point2D& point_) const;
    void DrawText(const std::string& message_, const sc2::Point2DI& point_) const;
    void DrawText(float value_, const sc2::Point2DI& point_) const;
    void DrawText(const std::string& message_, const sc2::Point3D& pos_) const;

    void DrawSphere(const sc2::Point3D& center_, float radius_) const;
    void DrawBox(const sc2::Point3D& min_, const sc2::Point3D& max_) const;
    void DrawLine(const sc2::Point3D& start_, const sc2::Point3D& end_) const;

    void EndGame() const;

    void SendDebug() const;

 private:

    sc2::DebugInterface* m_debug;
};

// ----------------------------------------------------------------------------
// Observer;
// ----------------------------------------------------------------------------
struct Observer
{
    explicit Observer(const sc2::ObservationInterface* observer_, std::unordered_map<sc2::Tag, std::unique_ptr<WrappedUnit>>& unit_map_, std::vector<WrappedUnit*>& last_step_units_);

    //WrappedUnits GetUnits() const;
    WrappedUnit* GetUnit(sc2::Tag tag_) const;
    WrappedUnits GetUnits(sc2::Unit::Alliance alliance_) const;
    WrappedUnits GetUnits(const Filter& filter_) const;
    WrappedUnits GetUnits(const Filter& filter_, sc2::Unit::Alliance alliance_ = sc2::Unit::Alliance::Self) const;

    size_t CountUnitType(sc2::UNIT_TYPEID type_) const;
    size_t CountUnitsTypes(const std::set<sc2::UNIT_TYPEID>& types_);
    const sc2::GameInfo& GameInfo() const;
    sc2::Point3D StartingLocation() const;
    float GetFoodCap() const;
    float GetFoodUsed() const;
    uint32_t GetMinerals() const;
    uint32_t GetVespene() const;
    float GetAvailableFood() const;
    sc2::UnitTypeData* GetUnitTypeData(sc2::UNIT_TYPEID id_);
    sc2::UpgradeData GetUpgradeData(sc2::UPGRADE_ID id_) const;
    sc2::AbilityData GetAbilityData(sc2::ABILITY_ID id_) const;
    sc2::Race GetCurrentRace() const;
    const std::vector<sc2::ChatMessage>& GetChatMessages() const;
    uint32_t GetGameLoop() const;
    const sc2::ScoreDetails GetScoreDetails() const;
    bool HasCreep(const sc2::Point2D& point_) const;
    void OnUpgradeCompleted();
    sc2::Visibility GetVisibility(const sc2::Point2D& pos_) const;

    const sc2::ObservationInterface* m_observer;

 private:
    
    std::unordered_map<sc2::UNIT_TYPEID, std::unique_ptr<sc2::UnitTypeData>> m_unit_data_cache;
    std::unordered_map<sc2::Tag, std::unique_ptr<WrappedUnit>>& m_unit_map;
    std::vector<WrappedUnit*>& m_last_step_units;
};

// ----------------------------------------------------------------------------
// Query;
// ----------------------------------------------------------------------------
struct Query
{
    explicit Query(sc2::QueryInterface* query_);

    bool CanBePlaced(sc2::ABILITY_ID ability_id_, const sc2::Point2D& point_);
    bool CanBePlaced(const Order& order_, const sc2::Point2D& point_);
    std::vector<bool> CanBePlaced(const std::vector<sc2::QueryInterface::PlacementQuery>& queries_);

    float PathingDistance(const sc2::Point2D& start_, const sc2::Point2D& end_) const;
    float PathingDistance(const sc2::Unit* start_, const sc2::Point2D& end_) const;
    std::vector<float> PathingDistances(const std::vector<sc2::QueryInterface::PathingQuery>& queries_) const;

    sc2::AvailableAbilities GetAbilitiesForUnit(const sc2::Unit* unit_, bool ignore_resource_requirements_) const;

 private:

    sc2::QueryInterface* m_query;
};

// ----------------------------------------------------------------------------
// Interface;
// ----------------------------------------------------------------------------
struct Interface
{
    Interface(
        sc2::ActionInterface* action_,
        sc2::ControlInterface* control_,
        sc2::DebugInterface* debug_,
        const sc2::ObservationInterface* observer_,
        sc2::QueryInterface* query_);

    Action&     action()    { return m_action; }
    Control&    control()   { return m_control; }
    Debug&      debug()     { return m_debug; }
    Observer&   observer()  { return m_observer; }
    Query&      query()     { return m_query; }

    void Init();
    void OnStep();
    void OnUpgradeComplete();

    WrappedUnit* WrapAndUpdateUnit(const sc2::Unit* unit_);

    inline const std::vector<WrappedUnit*> GetLastStepUnits()    { return m_last_step_units; }
    inline const WrappedUnits GetLastStepAllies()   { return m_last_step_allies; }
    inline const WrappedUnits GetLastStepEnemies()  { return m_last_step_enemies; }

 private:

     Action m_action;
     Control m_control;
     Debug m_debug;
     Observer m_observer;
     Query m_query;

    std::unordered_map<sc2::Tag, std::unique_ptr<WrappedUnit>> m_unit_map;
    std::vector<WrappedUnit*> m_last_step_units;
    WrappedUnits m_last_step_allies;
    WrappedUnits m_last_step_enemies;
};

}  // namespace API

// Globals
extern std::unique_ptr<API::Interface> gAPI;
