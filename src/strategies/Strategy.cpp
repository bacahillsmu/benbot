// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Historican.h"
#include "Strategy.h"
#include "core/API.h"
#include "core/Helpers.h"

#include <sc2api/sc2_map_info.h>

#include <algorithm>

Strategy::Strategy(float attack_limit_):
    m_attack_limit(attack_limit_)
{
}

void Strategy::OnStep(Builder*)
{
    // Clean up dead bodies.
    auto it = std::remove_if(
        m_units.begin(),
        m_units.end(),
        [](const sc2::Unit* unit_)
        {
            return !unit_->is_alive;
        });

    m_units.erase(it, m_units.end());

    if (static_cast<float>(m_units.size()) < m_attack_limit)
    {
        return;
    }

    auto targets = gAPI->observer().GameInfo().enemy_start_locations;
    gAPI->action().Attack(m_units, targets.front());

    m_units.clear();
    m_attack_limit = std::min<float>(m_attack_limit * 1.5f, 170.0f);
}

void Strategy::OnUnitCreated(WrappedUnit* unit_, Builder*)
{
    if (!IsCombatUnit()(unit_))
    {
        return;
    }

    if(unit_->unit_type == sc2::UNIT_TYPEID::TERRAN_REAPER)
    {
        return;
    }

    gHistory.info() << sc2::UnitTypeToName(unit_->unit_type) << " added to attack group" << std::endl;

    m_units.push_back(unit_);
}
