// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "API.h"
#include "Converter.h"
#include "Helpers.h"
#include "objects/GameObject.h"

#include <sc2api/sc2_map_info.h>

namespace API
{
    static std::unordered_map<sc2::ABILITY_ID, sc2::UNIT_TYPEID> AbilityToUnitMap;
    static std::unordered_map<sc2::ABILITY_ID, sc2::UPGRADE_ID> AbilityToUpgradeMap;

// ----------------------------------------------------------------------------
// Action;
// ----------------------------------------------------------------------------
Action::Action(sc2::ActionInterface* action_)
    : m_action(action_)
{
}

// ----------------------------------------------------------------------------
void Action::Build(const Order& order_)
{
    m_action->UnitCommand(order_.assignee, order_.ability_id);
}

// ----------------------------------------------------------------------------
void Action::Build(const Order& order_, const WrappedUnit* unit_)
{
    m_action->UnitCommand(order_.assignee, order_.ability_id, unit_);
}

// ----------------------------------------------------------------------------
void Action::Build(const Order& order_, const sc2::Point2D& point_)
{
    m_action->UnitCommand(order_.assignee, order_.ability_id, point_);
}

// ----------------------------------------------------------------------------
void Action::Attack(const WrappedUnits& units_, const sc2::Point2D& point_)
{
    m_action->UnitCommand(units_.ToAPI(), sc2::ABILITY_ID::ATTACK_ATTACK, point_);
}

// ----------------------------------------------------------------------------
void Action::Attack(const WrappedUnit& unit_, const WrappedUnit& target_)
{
    m_action->UnitCommand(&unit_, sc2::ABILITY_ID::ATTACK_ATTACK, &target_);
}

// ----------------------------------------------------------------------------
void Action::Attack(const WrappedUnit& unit_, const sc2::Point2D& point_)
{
    m_action->UnitCommand(&unit_, sc2::ABILITY_ID::ATTACK_ATTACK, point_);
}

// ----------------------------------------------------------------------------
void Action::MoveTo(const WrappedUnit& unit_, const sc2::Point2D& point_)
{
    m_action->UnitCommand(&unit_, sc2::ABILITY_ID::MOVE, point_);
}

// ----------------------------------------------------------------------------
void Action::MoveTo(const WrappedUnits& units_, const sc2::Point2D& point_)
{
    m_action->UnitCommand(units_.ToAPI(), sc2::ABILITY_ID::MOVE, point_);
}

// ----------------------------------------------------------------------------
void Action::Cast(const WrappedUnit& assignee_, sc2::ABILITY_ID ability_)
{
    m_action->UnitCommand(&assignee_, ability_);
}

// ----------------------------------------------------------------------------
void Action::Cast(const WrappedUnit& assignee_, sc2::ABILITY_ID ability_, const WrappedUnit& target_)
{
    m_action->UnitCommand(&assignee_, convert::ToAbilityID(ability_), &target_);
}

// ----------------------------------------------------------------------------
void Action::Cast(const WrappedUnit& assignee_, sc2::ABILITY_ID ability_, const sc2::Point2D& point_)
{
    m_action->UnitCommand(&assignee_, convert::ToAbilityID(ability_), point_);
}

// ----------------------------------------------------------------------------
void Action::Stop(const WrappedUnit& unit_, bool queue_ /*= false*/)
{
    m_action->UnitCommand(&unit_, sc2::ABILITY_ID::STOP, queue_);
}

// ----------------------------------------------------------------------------
void Action::Stop(const WrappedUnits& units_, bool queue_ /*= false*/)
{
    m_action->UnitCommand(units_.ToAPI(), sc2::ABILITY_ID::STOP, queue_);
}

// ----------------------------------------------------------------------------
void Action::Cancel(const WrappedUnit& assignee_)
{
    m_action->UnitCommand(&assignee_, sc2::ABILITY_ID::CANCEL);
}

// ----------------------------------------------------------------------------
void Action::CancelConstruction(const WrappedUnit& assignee_)
{
    m_action->UnitCommand(&assignee_, sc2::ABILITY_ID::CANCEL_BUILDINPROGRESS);
}

// ----------------------------------------------------------------------------
void Action::OpenGate(const WrappedUnit& assignee_)
{
    m_action->UnitCommand(&assignee_, sc2::ABILITY_ID::MORPH_WARPGATE);
}

// ----------------------------------------------------------------------------
void Action::SendMessage(const std::string& text_)
{
    m_action->SendChat(text_);
}

// ----------------------------------------------------------------------------
// Control;
// ----------------------------------------------------------------------------
Control::Control(sc2::ControlInterface* control_)
    : m_control(control_)
{
}

// ----------------------------------------------------------------------------
void Control::SaveReplay()
{
    m_control->SaveReplay("LastReplay.SC2Replay");
}

// ----------------------------------------------------------------------------
// Debug;
// ----------------------------------------------------------------------------
Debug::Debug(sc2::DebugInterface* debug_): m_debug(debug_)
{
}

// ----------------------------------------------------------------------------
void Debug::DrawText(const std::string& message_) const
{
    m_debug->DebugTextOut(message_);
}

// ----------------------------------------------------------------------------
void Debug::DrawText(const std::string& message_, const sc2::Point2D& point_) const
{
    m_debug->DebugTextOut(message_, sc2::Point3D(point_.x, point_.y, 0.35f));
}

// ----------------------------------------------------------------------------
void Debug::DrawText(const std::string& message_, const sc2::Point2DI& point_) const
{
    sc2::Point3D dst = {
        static_cast<float>(point_.x),
        static_cast<float>(point_.y),
        0.35f
    };
    m_debug->DebugTextOut(message_, dst);
}

// ----------------------------------------------------------------------------
void Debug::DrawText(float value_, const sc2::Point2DI& point_) const
{
    sc2::Point3D dst = {
        static_cast<float>(point_.x),
        static_cast<float>(point_.y),
        0.35f
    };
    m_debug->DebugTextOut(std::to_string(value_), dst);
}

// ----------------------------------------------------------------------------
void Debug::DrawText(const std::string& message_, const sc2::Point3D& pos_) const
{
    m_debug->DebugTextOut(message_, pos_);
}

// ----------------------------------------------------------------------------
void Debug::DrawSphere(const sc2::Point3D& center_, float radius_) const
{
    m_debug->DebugSphereOut(center_, radius_);
}

// ----------------------------------------------------------------------------
void Debug::DrawBox(const sc2::Point3D& min_, const sc2::Point3D& max_) const
{
    m_debug->DebugBoxOut(min_, max_);
}

// ----------------------------------------------------------------------------
void Debug::DrawLine(const sc2::Point3D& start_, const sc2::Point3D& end_) const
{
    m_debug->DebugLineOut(start_, end_);
}

// ----------------------------------------------------------------------------
void Debug::EndGame() const
{
    m_debug->DebugEndGame(true);
    SendDebug();
}

// ----------------------------------------------------------------------------
void Debug::SendDebug() const
{
    m_debug->SendDebug();
}

// ----------------------------------------------------------------------------
// Observer;
// ----------------------------------------------------------------------------
Observer::Observer(const sc2::ObservationInterface* observer_, std::unordered_map<sc2::Tag, std::unique_ptr<WrappedUnit>>& unit_map_, std::vector<WrappedUnit*>& last_step_units_)
    : m_observer(observer_)
    , m_unit_map(unit_map_)
    , m_last_step_units(last_step_units_)
{
}

// ----------------------------------------------------------------------------
WrappedUnit* Observer::GetUnit(sc2::Tag tag_) const
{
    auto itr = m_unit_map.find(tag_);
    if (itr != m_unit_map.end() && itr->second->IsInVision)
    {
        return itr->second.get();
    }
    else
    {
        auto unit = m_observer->GetUnit(tag_);
        if (unit == nullptr)
        {
            return nullptr;
        }
        else
        {
            assert(false && "Unit not in unit_map but exists in the game.");
            return gAPI->WrapAndUpdateUnit(unit);
        }
    }
}

// ----------------------------------------------------------------------------
// WrappedUnits Observer::GetUnits() const
// {
//     return WrappedUnits(m_last_step_units);
// }

// ----------------------------------------------------------------------------
WrappedUnits Observer::GetUnits(sc2::Unit::Alliance alliance_) const
{
    WrappedUnits units;

    for (auto& unit : m_last_step_units)
    {
        if (unit->alliance == alliance_)
        {
            units.push_back(unit);
        }
    }

    return units;
}

// ----------------------------------------------------------------------------
WrappedUnits Observer::GetUnits(const Filter& filter_) const
{
    WrappedUnits units;

    for (auto& unit : m_last_step_units)
    {
        if (filter_(unit))
        {
            units.push_back(unit);
        }
    }
    return units;
}

// ----------------------------------------------------------------------------
WrappedUnits Observer::GetUnits(const Filter& filter_, sc2::Unit::Alliance alliance_) const
{
    WrappedUnits units;

    for (auto& unit : m_last_step_units)
    {
        if (unit->alliance == alliance_ && filter_(unit))
        {
            units.push_back(unit);
        }
    }

    return units;
}

// ----------------------------------------------------------------------------
size_t Observer::CountUnitType(sc2::UNIT_TYPEID type_) const
{
    return GetUnits(IsUnit(type_), sc2::Unit::Alliance::Self).size();
}

// ----------------------------------------------------------------------------
size_t Observer::CountUnitsTypes(const std::set<sc2::UNIT_TYPEID>& types_)
{
    return GetUnits(OneOfUnits(types_), sc2::Unit::Alliance::Self).size();
}

// ----------------------------------------------------------------------------
const sc2::GameInfo& Observer::GameInfo() const
{
    return m_observer->GetGameInfo();
}

// ----------------------------------------------------------------------------
sc2::Point3D Observer::StartingLocation() const
{
    return m_observer->GetStartLocation();
}

// ----------------------------------------------------------------------------
float Observer::GetFoodCap() const
{
    return static_cast<float>(m_observer->GetFoodCap());
}

// ----------------------------------------------------------------------------
float Observer::GetFoodUsed() const
{
    return static_cast<float>(m_observer->GetFoodUsed());
}

// ----------------------------------------------------------------------------
uint32_t Observer::GetMinerals() const
{
    return m_observer->GetMinerals();
}

// ----------------------------------------------------------------------------
uint32_t Observer::GetVespene() const
{
    return m_observer->GetVespene();
}

// ----------------------------------------------------------------------------
float Observer::GetAvailableFood() const
{
    return GetFoodCap() - GetFoodUsed();
}

// ----------------------------------------------------------------------------
sc2::UnitTypeData* Observer::GetUnitTypeData(sc2::UNIT_TYPEID id_)
{
    auto itr = m_unit_data_cache.find(id_);
    if (itr != m_unit_data_cache.end())
    {
        return itr->second.get();
    }

    m_unit_data_cache.emplace(id_, std::make_unique<sc2::UnitTypeData>());
    sc2::UnitTypeData* data = m_unit_data_cache.find(id_)->second.get();

    *data = m_observer->GetUnitTypeData()[convert::ToUnitTypeID(id_)];

    switch (id_)
    {
        // NOTE (alkurbatov): Unfortunately SC2 API returns wrong mineral cost
        // and tech_requirement for orbital command, planetary fortress,
        // lair, hive and greater spire.
        // so we use a workaround.
        // See https://github.com/Blizzard/s2client-api/issues/191
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
            data->mineral_cost = 150;
            data->tech_requirement = sc2::UNIT_TYPEID::TERRAN_BARRACKS;
            break;

        case sc2::UNIT_TYPEID::ZERG_GREATERSPIRE:
            data->mineral_cost = 100;
            data->vespene_cost = 150;
            data->tech_requirement = sc2::UNIT_TYPEID::ZERG_HIVE;
            break;

        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
            data->mineral_cost = 150;
            data->tech_requirement = sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY;
            break;

        case sc2::UNIT_TYPEID::ZERG_BANELING:
            data->mineral_cost = 25;
            data->food_required = 0.0f;
            data->tech_alias.push_back(sc2::UNIT_TYPEID::ZERG_ZERGLING);
            break;

        case sc2::UNIT_TYPEID::ZERG_BROODLORD:
            data->mineral_cost = 150;
            data->vespene_cost = 150;
            data->food_required = 2.0f;
            data->tech_alias.push_back(sc2::UNIT_TYPEID::ZERG_CORRUPTOR);
            data->tech_requirement = sc2::UNIT_TYPEID::ZERG_GREATERSPIRE;
            break;

        case sc2::UNIT_TYPEID::ZERG_LAIR:
            data->mineral_cost = 150;
            data->tech_requirement = sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL;
            break;

        case sc2::UNIT_TYPEID::ZERG_OVERSEER:
            data->mineral_cost = 50;
            data->tech_requirement = sc2::UNIT_TYPEID::ZERG_LAIR;
            break;

        case sc2::UNIT_TYPEID::ZERG_RAVAGER:
            data->mineral_cost = 25;
            data->vespene_cost = 75;
            data->food_required = 1.0f;
            data->tech_alias.push_back(sc2::UNIT_TYPEID::ZERG_ROACH);
            data->tech_requirement = sc2::UNIT_TYPEID::ZERG_ROACHWARREN;
            break;

        case sc2::UNIT_TYPEID::ZERG_HIVE:
            data->mineral_cost = 200;
            data->vespene_cost = 150;
            data->tech_requirement = sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT;
            break;

        case sc2::UNIT_TYPEID::ZERG_LURKERMP:
            data->mineral_cost = 50;
            data->vespene_cost = 100;
            data->ability_id = sc2::ABILITY_ID::MORPH_LURKER;
            data->food_required = 1.0f;
            data->tech_alias.push_back(sc2::UNIT_TYPEID::ZERG_HYDRALISK);
            data->tech_requirement = sc2::UNIT_TYPEID::ZERG_LURKERDENMP;
            break;

        // NOTE (alkurbatov): By some reason all zerg buildings
        // include drone mineral cost.
        case sc2::UNIT_TYPEID::ZERG_BANELINGNEST:
        case sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER:
        case sc2::UNIT_TYPEID::ZERG_EXTRACTOR:
        case sc2::UNIT_TYPEID::ZERG_HYDRALISKDEN:
        case sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT:
        case sc2::UNIT_TYPEID::ZERG_LURKERDENMP:
        case sc2::UNIT_TYPEID::ZERG_ROACHWARREN:
        case sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL:
        case sc2::UNIT_TYPEID::ZERG_SPINECRAWLER:
        case sc2::UNIT_TYPEID::ZERG_SPIRE:
        case sc2::UNIT_TYPEID::ZERG_SPORECRAWLER:
        case sc2::UNIT_TYPEID::ZERG_ULTRALISKCAVERN:
            data->mineral_cost -= 50;
            break;

        // NOTE (alkurbatov): There is no sense in summoning protoss buildings
        // without a pylon.
        case sc2::UNIT_TYPEID::PROTOSS_FORGE:
        case sc2::UNIT_TYPEID::PROTOSS_GATEWAY:
            data->tech_requirement = sc2::UNIT_TYPEID::PROTOSS_PYLON;
            break;

        default:
            break;
    }

    return data;
}

// ----------------------------------------------------------------------------
sc2::UpgradeData Observer::GetUpgradeData(sc2::UPGRADE_ID id_) const
{
    return m_observer->GetUpgradeData()[convert::ToUpgradeID(id_)];
}

// ----------------------------------------------------------------------------
sc2::AbilityData Observer::GetAbilityData(sc2::ABILITY_ID id_) const
{
    return m_observer->GetAbilityData()[convert::ToAbilityID(id_)];
}

// ----------------------------------------------------------------------------
sc2::Race Observer::GetCurrentRace() const
{
    uint32_t id = m_observer->GetPlayerID();
    return m_observer->GetGameInfo().player_info[id - 1].race_actual;
}

// ----------------------------------------------------------------------------
const std::vector<sc2::ChatMessage>& Observer::GetChatMessages() const
{
    return m_observer->GetChatMessages();
}

// ----------------------------------------------------------------------------
uint32_t Observer::GetGameLoop() const
{
    return m_observer->GetGameLoop();
}

// ----------------------------------------------------------------------------
const sc2::ScoreDetails Observer::GetScoreDetails() const
{
    return m_observer->GetScore().score_details;
}

// ----------------------------------------------------------------------------
bool Observer::HasCreep(const sc2::Point2D& point_) const
{
    return m_observer->HasCreep(point_);
}

// ----------------------------------------------------------------------------
void Observer::OnUpgradeCompleted()
{
    m_unit_data_cache.clear();
}

// ----------------------------------------------------------------------------
sc2::Visibility Observer::GetVisibility(const sc2::Point2D& pos_) const
{
    return m_observer->GetVisibility(pos_);
}

// ----------------------------------------------------------------------------
// Query;
// ----------------------------------------------------------------------------
Query::Query(sc2::QueryInterface* query_)
    : m_query(query_)
{
}

// ----------------------------------------------------------------------------
bool Query::CanBePlaced(sc2::ABILITY_ID ability_id_, const sc2::Point2D& point_)
{
    return m_query->Placement(ability_id_, point_);
}

// ----------------------------------------------------------------------------
bool Query::CanBePlaced(const Order& order_, const sc2::Point2D& point_)
{
    return m_query->Placement(order_.ability_id, point_);
}

// ----------------------------------------------------------------------------
std::vector<bool> Query::CanBePlaced(const std::vector<sc2::QueryInterface::PlacementQuery>& queries_)
{
    return m_query->Placement(queries_);
}

// ----------------------------------------------------------------------------
float Query::PathingDistance(const sc2::Point2D& start_, const sc2::Point2D& end_) const
{
    return m_query->PathingDistance(start_, end_);
}

// ----------------------------------------------------------------------------
float Query::PathingDistance(const sc2::Unit* start_, const sc2::Point2D& end_) const
{
    return m_query->PathingDistance(start_, end_);
}

// ----------------------------------------------------------------------------
std::vector<float> Query::PathingDistances(const std::vector<sc2::QueryInterface::PathingQuery>& queries_) const
{
    return m_query->PathingDistance(queries_);
}

// ----------------------------------------------------------------------------
sc2::AvailableAbilities Query::GetAbilitiesForUnit(const sc2::Unit* unit_, bool ignore_resource_requirements_) const
{
    return m_query->GetAbilitiesForUnit(unit_, ignore_resource_requirements_);
}

// ----------------------------------------------------------------------------
// Interfaces;
// ----------------------------------------------------------------------------
Interface::Interface(sc2::ActionInterface* action_,
                     sc2::ControlInterface* control_,
                     sc2::DebugInterface* debug_,
               const sc2::ObservationInterface* observer_,
                     sc2::QueryInterface* query_)
    :m_action(action_)
    ,m_control(control_)
    ,m_debug(debug_)
    ,m_observer(observer_, m_unit_map, m_last_step_units)
    ,m_query(query_)
{
}

// ----------------------------------------------------------------------------
void Interface::Init()
{
    // Make a mapping of ability -> unit, for abilities that construct units
    const auto& unit_datas = m_observer.m_observer->GetUnitTypeData();
    for (auto& data : unit_datas)
    {
        if (data.ability_id != sc2::ABILITY_ID::INVALID)
        {
            AbilityToUnitMap[data.ability_id] = data.unit_type_id;
        }
    }

    // Make a mapping of ability -> upgrade
    const auto& upgrade_datas = m_observer.m_observer->GetUpgradeData();
    for (auto& data : upgrade_datas)
    {
        if (data.ability_id != sc2::ABILITY_ID::INVALID)
        {
            AbilityToUpgradeMap[data.ability_id] = sc2::UpgradeID(data.upgrade_id);
        }
    }
    
    OnStep();
}

// ----------------------------------------------------------------------------
void Interface::OnStep()
{
    // We reset vision on all units to fake FoW. If a unit goes into FoW it is still in map;
    for (auto& pair : m_unit_map)
    {
        pair.second->IsInVision = false;
    }

    sc2::Units units = observer().m_observer->GetUnits();   // Grab all units that are "in-play" in SC2;
    m_last_step_units.clear();
    m_last_step_allies.clear();
    m_last_step_enemies.clear();
    m_last_step_units.reserve(units.size());
    m_last_step_allies.reserve(units.size());
    m_last_step_enemies.reserve(units.size());
    for (const sc2::Unit* unit : units)
    {
        auto itr = m_unit_map.find(unit->tag);
        if (itr == m_unit_map.end())
        {
            auto wrapped_unit = WrappedUnit::Make(*unit);
            m_last_step_units.emplace_back(wrapped_unit.get());

            if(wrapped_unit->alliance == sc2::Unit::Ally)
            {
                m_last_step_allies.emplace_back(wrapped_unit.get());
            }
            else if(wrapped_unit->alliance == sc2::Unit::Enemy)
            {
                m_last_step_enemies.emplace_back(wrapped_unit.get());
            }

            m_unit_map[unit->tag] = std::move(wrapped_unit);
        }
        else
        {
            itr->second->IsInVision = true;
            itr->second->m_order_queued_in_current_step = false;
            itr->second->UpdateAPIData(*unit);
            m_last_step_units.emplace_back(itr->second.get());

            if (itr->second->alliance == sc2::Unit::Ally)
            {
                m_last_step_allies.emplace_back(itr->second.get());
            }
            else if (itr->second->alliance == sc2::Unit::Enemy)
            {
                m_last_step_enemies.emplace_back(itr->second.get());
            }
        }
    }


}

// ----------------------------------------------------------------------------
void Interface::OnUpgradeComplete()
{
    m_observer.OnUpgradeCompleted();
}

// ----------------------------------------------------------------------------
WrappedUnit* Interface::WrapAndUpdateUnit(const sc2::Unit* unit_)
{
    assert(unit_ != nullptr);

    auto itr = m_unit_map.find(unit_->tag);

    if (itr == m_unit_map.end())
    {
        m_unit_map[unit_->tag] = WrappedUnit::Make(*unit_);
        return m_unit_map[unit_->tag].get();
    }

    itr->second->UpdateAPIData(*unit_);
    return itr->second.get();
}

}  // namespace API

std::unique_ptr<API::Interface> gAPI;
