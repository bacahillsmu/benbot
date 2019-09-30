// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Converter.h"
#include "Helpers.h"
#include "Hub.h"
#include "blueprints/Unit.h"
#include "core/API.h"

#include <algorithm>
#include <vector>

// ----------------------------------------------------------------------------
IsUnit::IsUnit(sc2::UNIT_TYPEID type_):
    m_type(type_)
{
}

// ----------------------------------------------------------------------------
bool IsUnit::operator()(const sc2::Unit& unit_) const
{
    return unit_.unit_type == m_type && unit_.build_progress >= 1.0f;
}

// ----------------------------------------------------------------------------
IsIdleUnit::IsIdleUnit(sc2::UNIT_TYPEID type_, bool non_full_reactor_idle /*= true*/)
    :m_type(type_)
    , m_non_full_reactor_idle(non_full_reactor_idle)
{
}

// ----------------------------------------------------------------------------
bool IsIdleUnit::operator()(const sc2::Unit& unit_) const
{
    if (IsUnit(m_type)(unit_))
    {
        if (m_non_full_reactor_idle && HasAddon(sc2::UNIT_TYPEID::TERRAN_REACTOR)(unit_))
        {
            return unit_.orders.size() < 2;
        }
        else
        {
            return unit_.orders.empty();
        }
    }

    return false;
}

// ----------------------------------------------------------------------------
OneOfUnits::OneOfUnits(const std::set<sc2::UNIT_TYPEID>& types_):
    m_types(types_)
{
}

// ----------------------------------------------------------------------------
bool OneOfUnits::operator()(const sc2::Unit& unit_) const
{
    return unit_.build_progress == 1.0f && m_types.find(unit_.unit_type) != m_types.end();
}

// ----------------------------------------------------------------------------
bool IsCombatUnit::operator()(const sc2::Unit& unit_) const
{
    switch (unit_.unit_type.ToType())
    {
       case sc2::UNIT_TYPEID::TERRAN_BANSHEE:
       case sc2::UNIT_TYPEID::TERRAN_CYCLONE:
       case sc2::UNIT_TYPEID::TERRAN_GHOST:
       case sc2::UNIT_TYPEID::TERRAN_HELLION:
       case sc2::UNIT_TYPEID::TERRAN_HELLIONTANK:
       case sc2::UNIT_TYPEID::TERRAN_LIBERATOR:
       case sc2::UNIT_TYPEID::TERRAN_LIBERATORAG:
       case sc2::UNIT_TYPEID::TERRAN_MARAUDER:
       case sc2::UNIT_TYPEID::TERRAN_MARINE:
       case sc2::UNIT_TYPEID::TERRAN_MEDIVAC:
       case sc2::UNIT_TYPEID::TERRAN_RAVEN:
       case sc2::UNIT_TYPEID::TERRAN_REAPER:
       case sc2::UNIT_TYPEID::TERRAN_SIEGETANK:
       case sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
       case sc2::UNIT_TYPEID::TERRAN_THOR:
       case sc2::UNIT_TYPEID::TERRAN_THORAP:
       case sc2::UNIT_TYPEID::TERRAN_VIKINGASSAULT:
       case sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
       case sc2::UNIT_TYPEID::TERRAN_WIDOWMINE:
       case sc2::UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED:

       case sc2::UNIT_TYPEID::ZERG_BANELING:
       case sc2::UNIT_TYPEID::ZERG_BANELINGBURROWED:
       case sc2::UNIT_TYPEID::ZERG_BROODLORD:
       case sc2::UNIT_TYPEID::ZERG_CORRUPTOR:
       case sc2::UNIT_TYPEID::ZERG_HYDRALISK:
       case sc2::UNIT_TYPEID::ZERG_HYDRALISKBURROWED:
       case sc2::UNIT_TYPEID::ZERG_INFESTOR:
       case sc2::UNIT_TYPEID::ZERG_INFESTORBURROWED:
       case sc2::UNIT_TYPEID::ZERG_INFESTORTERRAN:
       case sc2::UNIT_TYPEID::ZERG_LURKERMP:
       case sc2::UNIT_TYPEID::ZERG_LURKERMPBURROWED:
       case sc2::UNIT_TYPEID::ZERG_MUTALISK:
       case sc2::UNIT_TYPEID::ZERG_RAVAGER:
       case sc2::UNIT_TYPEID::ZERG_ROACH:
       case sc2::UNIT_TYPEID::ZERG_ROACHBURROWED:
       case sc2::UNIT_TYPEID::ZERG_ULTRALISK:
       case sc2::UNIT_TYPEID::ZERG_VIPER:
       case sc2::UNIT_TYPEID::ZERG_ZERGLING:
       case sc2::UNIT_TYPEID::ZERG_ZERGLINGBURROWED:

       case sc2::UNIT_TYPEID::PROTOSS_ADEPT:
       case sc2::UNIT_TYPEID::PROTOSS_ADEPTPHASESHIFT:
       case sc2::UNIT_TYPEID::PROTOSS_ARCHON:
       case sc2::UNIT_TYPEID::PROTOSS_CARRIER:
       case sc2::UNIT_TYPEID::PROTOSS_COLOSSUS:
       case sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
       case sc2::UNIT_TYPEID::PROTOSS_DISRUPTOR:
       case sc2::UNIT_TYPEID::PROTOSS_DISRUPTORPHASED:
       case sc2::UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
       case sc2::UNIT_TYPEID::PROTOSS_IMMORTAL:
       case sc2::UNIT_TYPEID::PROTOSS_MOTHERSHIP:
       case sc2::UNIT_TYPEID::PROTOSS_ORACLE:
       case sc2::UNIT_TYPEID::PROTOSS_PHOENIX:
       case sc2::UNIT_TYPEID::PROTOSS_SENTRY:
       case sc2::UNIT_TYPEID::PROTOSS_STALKER:
       case sc2::UNIT_TYPEID::PROTOSS_TEMPEST:
       case sc2::UNIT_TYPEID::PROTOSS_VOIDRAY:
       case sc2::UNIT_TYPEID::PROTOSS_ZEALOT:
            return true;

       default:
            return false;
    }
}

// ----------------------------------------------------------------------------
bool IsVisibleMineralPatch::operator()(const sc2::Unit& unit_) const
{
    return unit_.mineral_contents > 0;
}

// ----------------------------------------------------------------------------
bool IsFoggyResource::operator()(const sc2::Unit& unit_) const {
    switch (unit_.unit_type.ToType()) {
        // Mineral types.
        case sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD750:
        case sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD:
        case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750:
        case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD:
        case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750:
        case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD:
        case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750:
        case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD:
        case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD750:
        case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD:
        case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750:
        case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD:

        // Geyser types.
        case sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER:
        case sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER:
        case sc2::UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER:
        case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER:
        case sc2::UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER:
        case sc2::UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER:
            return unit_.display_type != sc2::Unit::DisplayType::Visible;

        default:
            return false;
    }
}

// ----------------------------------------------------------------------------
bool IsVisibleGeyser::operator()(const sc2::Unit& unit_) const {
    return unit_.vespene_contents > 0;
}

// ----------------------------------------------------------------------------
bool IsFreeGeyser::operator()(const sc2::Unit& unit_) const {
    return IsVisibleGeyser()(unit_) && !gHub->IsOccupied(unit_);
}

// ----------------------------------------------------------------------------
bool IsMineralPatch::operator()(const sc2::Unit& unit_) const
{
    switch (unit_.unit_type.ToType())
    {
        case sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD:
        case sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD750:
        case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD:
        case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750:
        case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD:
        case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750:
        case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD:
        case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750:
        case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD:
        case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD750:
        case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD:
        case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750:
            return true;

        default:
            return false;
    }
}

// ----------------------------------------------------------------------------
bool IsGeyser::operator()(const sc2::Unit& unit_) const
{
    switch (unit_.unit_type.ToType())
    {
        case sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER:
        case sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER:
        case sc2::UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER:
        case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER:
        case sc2::UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER:
        case sc2::UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER:
            return true;
        default:
            return false;
    }
}

// ----------------------------------------------------------------------------
bool IsRefinery::operator()(const sc2::Unit& unit_) const
{
    if (unit_.build_progress != 1.0f)
        return false;

    return unit_.unit_type == sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR ||
        unit_.unit_type == sc2::UNIT_TYPEID::ZERG_EXTRACTOR         ||
        unit_.unit_type == sc2::UNIT_TYPEID::TERRAN_REFINERY;
}

// ----------------------------------------------------------------------------
bool IsWorker::operator()(const sc2::Unit& unit_) const
{
    return unit_.unit_type == sc2::UNIT_TYPEID::TERRAN_SCV ||
        unit_.unit_type == sc2::UNIT_TYPEID::ZERG_DRONE ||
        unit_.unit_type == sc2::UNIT_TYPEID::PROTOSS_PROBE;
}

// ----------------------------------------------------------------------------
bool IsGasWorker::operator()(const sc2::Unit& unit_) const {
    if (!IsWorker()(unit_))
        return false;

    if (unit_.orders.empty())
        return false;

    if (unit_.orders.front().ability_id == sc2::ABILITY_ID::HARVEST_RETURN) {
        if (unit_.buffs.empty())
            return false;

        return unit_.buffs.front() == sc2::BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGAS ||
            unit_.buffs.front() == sc2::BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGASZERG ||
            unit_.buffs.front() == sc2::BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGASPROTOSS;
    }

    if (unit_.orders.front().ability_id == sc2::ABILITY_ID::HARVEST_GATHER)
        return gHub->IsTargetOccupied(unit_.orders.front());

    return false;
}

// ----------------------------------------------------------------------------
bool IsTownHall::operator()(const sc2::Unit& unit_) const {
    return unit_.unit_type == sc2::UNIT_TYPEID::PROTOSS_NEXUS ||
           unit_.unit_type == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER ||
           unit_.unit_type == sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND ||
           unit_.unit_type == sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS ||
           unit_.unit_type == sc2::UNIT_TYPEID::ZERG_HATCHERY ||
           unit_.unit_type == sc2::UNIT_TYPEID::ZERG_HIVE ||
           unit_.unit_type == sc2::UNIT_TYPEID::ZERG_LAIR;
}

// ----------------------------------------------------------------------------
bool IsIdleTownHall::operator()(const sc2::Unit& unit_) const
{
    return IsTownHall()(unit_) && unit_.orders.empty() && unit_.build_progress == 1.0f;
}

// ----------------------------------------------------------------------------
bool IsCommandCenter::operator()(const sc2::Unit& unit_) const
{
    return unit_.unit_type == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER       
        || unit_.unit_type == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING 
        || unit_.unit_type == sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND      
        || unit_.unit_type == sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING
        || unit_.unit_type == sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS;
}

// ----------------------------------------------------------------------------
bool IsBuilding::operator()(const sc2::Unit& unit_) const
{
    return (*this)(unit_.unit_type);
}

// ----------------------------------------------------------------------------
bool IsBuilding::operator()(sc2::UNIT_TYPEID type_) const
{
    // This counts as a structure, but not in the cases that we want to use this for;
    if(type_ == sc2::UNIT_TYPEID::TERRAN_AUTOTURRET)
    {
        return false;
    }

    sc2::UnitTypeData data = gAPI->observer().GetUnitTypeData(type_);
    return std::find(data.attributes.begin(), data.attributes.end(), sc2::Attribute::Structure) != data.attributes.end();

}

// ----------------------------------------------------------------------------
bool IsAddonBuilding::operator()(const sc2::Unit& unit_) const
{
    return (*this)(unit_.unit_type);
}

// ----------------------------------------------------------------------------
bool IsAddonBuilding::operator()(sc2::UNIT_TYPEID type_) const
{
    switch (type_)
    {
        case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING:
        case sc2::UNIT_TYPEID::TERRAN_FACTORY:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYFLYING:
        case sc2::UNIT_TYPEID::TERRAN_STARPORT:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTFLYING:
        {
            return true;
        }

        default:
        {
            return false;
        }
    }
}

// ----------------------------------------------------------------------------
IsOrdered::IsOrdered(sc2::UNIT_TYPEID type_):
    m_type(type_)
{
}

// ----------------------------------------------------------------------------
bool IsOrdered::operator()(const Order& order_) const
{
    return order_.unit_type_id == m_type;
}

// ----------------------------------------------------------------------------
bool IsWithinDistance::operator()(const sc2::Unit& unit_) const
{
    if (m_2d)
    {
        return sc2::DistanceSquared2D(m_center, unit_.pos) < m_distSq;
    }

    return sc2::DistanceSquared3D(m_center, unit_.pos) < m_distSq;
}

// ----------------------------------------------------------------------------
HasAddon::HasAddon(sc2::UNIT_TYPEID addon_type_)
    :m_addon_type(addon_type_)
{
}

// ----------------------------------------------------------------------------
bool HasAddon::operator()(const sc2::Unit& unit_) const
{
    if(unit_.add_on_tag == sc2::NullTag && m_addon_type == sc2::UNIT_TYPEID::INVALID)
    {
        return true;
    }
    if (unit_.add_on_tag == sc2::NullTag && m_addon_type != sc2::UNIT_TYPEID::INVALID)
    {
        return false;
    }

    const sc2::Unit* addon = gAPI->observer().GetUnit(unit_.add_on_tag);
    sc2::UNIT_TYPEID addonType = addon->unit_type.ToType();
    auto addonAlias = gAPI->observer().GetUnitTypeData(addon->unit_type).tech_alias.front();

    return addonType == m_addon_type || addonAlias == m_addon_type;
}

// ----------------------------------------------------------------------------
MultiFilter::MultiFilter(Selector selector_, std::initializer_list<sc2::Filter> filters_)
    :m_filters(filters_)
    ,m_selector(selector_)
{
}

// ----------------------------------------------------------------------------
bool MultiFilter::operator()(const sc2::Unit& unit_) const
{
    if(m_selector == Selector::And)
    {
        for(auto& filter : m_filters)
        {
            if(!filter(unit_))
            {
                return false;
            }
        }
        return true;
    }
    else if(m_selector == Selector::Or)
    {
        for(auto& filter : m_filters)
        {
            if (filter(unit_))
            {
                return true;
            }
        }
        return false;
    }
    return false;
}

// ----------------------------------------------------------------------------
sc2::Point2D GetTerranAddonPosition(const sc2::Unit& unit_)
{
    return GetTerranAddonPosition(unit_.pos);
}

// ----------------------------------------------------------------------------
sc2::Point2D GetTerranAddonPosition(const sc2::Point2D& parent_building_position_)
{
    sc2::Point2D pos = parent_building_position_;
    pos.x += ADDON_DISPLACEMENT_IN_X;
    pos.y += ADDON_DISPLACEMENT_IN_Y;
    return pos;
}


