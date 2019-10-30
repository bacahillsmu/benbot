// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include "Order.h"
#include "core/WrappedUnit.hpp"
#include "API.h"

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_unit.h>

#include <set>
#include <functional>
#include <initializer_list>
#include <vector>

#include "objects/Worker.h"

// ----------------------------------------------------------------------------
// Is...
// ----------------------------------------------------------------------------
struct IsUnit
{
    explicit IsUnit(sc2::UNIT_TYPEID type_);

    bool operator()(const WrappedUnit* unit_) const;

 private:
    sc2::UNIT_TYPEID m_type;
};

struct IsIdleUnit
{
    explicit IsIdleUnit(sc2::UNIT_TYPEID type_, bool non_full_reactor_idle = true);

    bool operator()(const WrappedUnit* unit_) const;

private:
    sc2::UNIT_TYPEID m_type;
    bool m_non_full_reactor_idle = true;
};

struct OneOfUnits
{
    explicit OneOfUnits(const std::set<sc2::UNIT_TYPEID>& types_);

    bool operator()(const WrappedUnit* unit_) const;

 private:

    const std::set<sc2::UNIT_TYPEID> m_types;
};

struct IsCombatUnit
{
    bool operator()(const WrappedUnit* unit_) const;
};

struct IsVisibleMineralPatch
{
    // NOTE (alkurbatov): All the visible mineral patches has non-zero mineral
    // contents while the mineral patches covered by the fog of war don't have
    // such parameter (it is always zero) and can't be selected/targeted.
    // This filter returns only the visible and not depleted mineral patches.

    bool operator()(const WrappedUnit* unit_) const;
};

struct IsFoggyResource
{
    bool operator()(const WrappedUnit* unit_) const;
};

struct IsVisibleGeyser
{
    // NOTE (alkurbatov): All the geysers has non-zero vespene contents while
    // the geysers covered by the fog of war don't have such parameter
    // (it is always zero) and can't be selected/targeted.
    // This filter returns only the visible and not depleted geysers.

    bool operator()(const sc2::Unit* unit_) const;
};

// Check that the provided unit is not occupied and not depleted geyser
struct IsFreeGeyser
{
    bool operator()(const WrappedUnit* unit_) const;
};

struct IsMineralPatch
{
    bool operator()(const WrappedUnit* unit_) const;
};

struct IsGeyser
{
    bool operator()(const WrappedUnit* unit_) const;
};

struct IsRefinery
{
    bool operator()(const WrappedUnit* unit_) const;
};

struct IsWorker
{
    bool operator()(const sc2::Unit* unit_) const;
    bool operator()(const WrappedUnit* unit_) const;
};

struct IsWorkerWithJob
{
    explicit IsWorkerWithJob(Worker::Job job_);

    bool operator()(const WrappedUnit* unit_) const;

private:
    Worker::Job m_job;
};

struct IsWorkerWithHomeBase
{
    explicit IsWorkerWithHomeBase(const std::shared_ptr<Expansion>& homeBase_);

    bool operator()(const WrappedUnit* unit_) const;

private:
    const std::shared_ptr<Expansion>& m_homeBase;
};

struct IsTownHall
{
    bool operator()(const WrappedUnit* unit_) const;
};

struct IsIdleTownHall
{
    bool operator()(const WrappedUnit* unit_) const;
};

struct IsCommandCenter
{
    bool operator()(const WrappedUnit* unit_) const;
};

struct IsBuilding
{
    bool operator()(const WrappedUnit* unit_) const;
    bool operator()(sc2::UNIT_TYPEID type_) const;
};

struct IsAddonBuilding
{
    bool operator()(const WrappedUnit* unit_) const;
    bool operator()(sc2::UNIT_TYPEID type_) const;
};

struct IsOrdered
{
    explicit IsOrdered(sc2::UNIT_TYPEID type_);

    bool operator()(const Order& order_) const;

 private:
    sc2::UNIT_TYPEID m_type;
};

struct IsWithinDistance
{
    explicit IsWithinDistance(const sc2::Point3D& center_, float dist_)
        :m_center(center_)
        ,m_distSq(dist_ * dist_)
        ,m_2d(false)
    {
    }
    explicit IsWithinDistance(const sc2::Point2D& center_, float dist_)
        :m_center(sc2::Point3D(center_.x, center_.y, 0))
        ,m_distSq(dist_ * dist_)
        ,m_2d(true)
    {
    }

    bool operator()(const WrappedUnit* unit_) const;

private:
    sc2::Point3D m_center;
    float m_distSq;
    bool m_2d;
};

struct CloakState
{
    explicit CloakState(sc2::Unit::CloakState state_)
        : m_state(state_) { }
    bool operator()(const WrappedUnit* unit_) const;

private:
    sc2::Unit::CloakState m_state;
};

// ----------------------------------------------------------------------------
// Has...
// ----------------------------------------------------------------------------
struct HasAddon
{
    explicit HasAddon(sc2::UNIT_TYPEID addon_type_);

    bool operator()(const WrappedUnit* unit_) const;

private:

    sc2::UNIT_TYPEID m_addon_type;
};

struct MultiFilter
{
    enum class Selector
    {
        And,
        Or
    };

    MultiFilter(Selector selector_, std::initializer_list<API::Filter> filters_);

    bool operator()(const WrappedUnit* unit_) const;

private:
    std::vector<API::Filter> m_filters;
    Selector m_selector;
};

static constexpr float ADDON_DISPLACEMENT_IN_X = 2.5f;
static constexpr float ADDON_DISPLACEMENT_IN_Y = -0.5f;
sc2::Point2D GetTerranAddonPosition(const WrappedUnit* unit_);
sc2::Point2D GetTerranAddonPosition(const sc2::Point2D& parent_building_position_);

WrappedUnits GetFreeWorkers(bool includingGasWorkers = false);
WrappedUnit* GetClosestFreeWorker(const sc2::Point2D& location_, bool includingGasWorkers = false);
bool FreeWorkerExists(bool includingGasWorkers = false);

