#include "ControlGroup.hpp"
#include "core/API.h"
#include "core/Helpers.h"
#include "Hub.h"


// ----------------------------------------------------------------------------
ControlGroup::ControlGroup()
{
}

// ----------------------------------------------------------------------------
void ControlGroup::OnStep()
{
    // Clean up my Units;
    std::vector<WrappedUnit*>::iterator myUnitsToRemove =
    std::remove_if(m_units.begin(), m_units.end(),
        [](auto* myUnit)
        {
            return !myUnit->is_alive || !myUnit->IsInVision;
        });

    // Clean up Enemy Units;
    std::vector<WrappedUnit*>::iterator enemyUnitsToRemove =
    std::remove_if(m_enemies.begin(), m_enemies.end(),
        [](const WrappedUnit* enemyUnit)
        {
            return !enemyUnit->is_alive || !enemyUnit->IsInVision;
        });

    m_units.erase(myUnitsToRemove, m_units.end());
    m_enemies.erase(enemyUnitsToRemove, m_enemies.end());

    CalculateCenter();
    Update();
    UpdateMovement();
}

// ----------------------------------------------------------------------------
void ControlGroup::Update()
{
    if(!m_sent)
    {
        return;
    }

//     if(IsTaskFinished()) // There is no coming back... You die first;
//     {
//         AbortMovement();
//         m_sent = false;
//         return;
//     }

    WrappedUnits enemies = gAPI->observer().GetUnits(
        MultiFilter(MultiFilter::Selector::And,
        {
                IsWithinDistance(GetCenter(),
                AggroRadius)
        }), sc2::Unit::Alliance::Enemy);

//     if (enemies.empty() && IsMoving())
//     {
//         return;
//     }

    if (enemies.empty())
    {
        for (auto& unit : GetUnits())
        {
            unit->Micro()->OnCombatOver(unit);
        }
            
        Approach(NextHarassTarget());
    }
    else
    {
        SetEnemies(std::move(enemies));
        for (auto& unit : GetUnits())
        {
            unit->Micro()->OnCombatFrame(unit, GetEnemyUnits(), GetUnits(), GetAttackMovePoint());
        }
    }
        
}

// ----------------------------------------------------------------------------
void ControlGroup::UpdateMovement()
{
    switch (m_moveState)
    {
        case MovementState::IDLE:
        {
            break;
        }

        case MovementState::APPROACH:
        {
            if (Distance2D(GetCenter(), m_approachPos) < RegroupRadius / 2.0f)
            {
                m_moveState = MovementState::IDLE;
            }
            else if (GetSpreadRadius() > RegroupRadius)
            {
                WrappedUnit* regroup_unit = nullptr;
                for (auto& unit : m_units)
                {
                    if (!unit->is_flying)
                    {
                        regroup_unit = unit;
                    }
                }
                if (!regroup_unit && !m_units.empty())
                {
                    regroup_unit = m_units.front();
                }
                if (regroup_unit && IsPointReachable(regroup_unit, GetCenter()))
                {
                    RegroupAt(GetCenter());
                }
            }
            else
            {
                IssueMoveCommand(m_approachPos);
            }
            break;
        }

        case MovementState::REGROUP:
        {
            if (GetSpreadRadius() < RegroupRadius / 2.0f)
            {
                if (m_wasApproaching)
                {
                    m_moveState = MovementState::APPROACH;
                }
                else
                {
                    m_moveState = MovementState::IDLE;
                }
            }
            else
            {
                IssueMoveCommand(m_regroupPos);
            }

            break;
        }
    }
}

// ----------------------------------------------------------------------------
void ControlGroup::AbortMovement()
{
    m_moveState = MovementState::IDLE;
    m_wasApproaching = false;
}

// ----------------------------------------------------------------------------
void ControlGroup::AddUnit(WrappedUnit* unit_)
{
    m_units.push_back(unit_);
}

// ----------------------------------------------------------------------------
void ControlGroup::RemoveUnit(WrappedUnit* unit_)
{
    m_units.remove(unit_);
}

// ----------------------------------------------------------------------------
const WrappedUnits& ControlGroup::GetUnits() const
{
    return m_units;
}

// ----------------------------------------------------------------------------
void ControlGroup::AddEnemy(WrappedUnit* enemy_)
{
    m_enemies.push_back(enemy_);
}

// ----------------------------------------------------------------------------
void ControlGroup::RemoveEnemy(WrappedUnit* enemy_)
{
    m_enemies.remove(enemy_);
}

// ----------------------------------------------------------------------------
const WrappedUnits& ControlGroup::GetEnemyUnits() const
{
    return m_enemies;
}

// ----------------------------------------------------------------------------
void ControlGroup::SetEnemies(WrappedUnits enemies_)
{
    m_enemies = std::move(enemies_);
}

// ----------------------------------------------------------------------------
void ControlGroup::Send()
{
    m_sent = true;
    Approach(NextHarassTarget());
}

// ----------------------------------------------------------------------------
void ControlGroup::Approach(const sc2::Point2D& position)
{
    m_approachPos = position;
    m_moveState = MovementState::APPROACH;
}

// ----------------------------------------------------------------------------
void ControlGroup::RegroupAt(const sc2::Point2D& position)
{
    m_regroupPos = position;
    m_wasApproaching = m_moveState == MovementState::APPROACH;
    m_moveState = MovementState::REGROUP;
}

// ----------------------------------------------------------------------------
bool ControlGroup::IsTaskFinished()
{
    return GetEnemyUnits().empty();
}

// ----------------------------------------------------------------------------
void ControlGroup::CalculateCenter()
{
    auto unitCircle = m_units.CalculateCircle();
    m_center = unitCircle.first;
    m_spreadRadius = unitCircle.second;

    if (!m_enemies.empty())
    {
        auto enemyCircle = m_enemies.CalculateCircle();
        m_enemyCenter = enemyCircle.first;
        m_enemySpreadRadius = enemyCircle.second;
    }
}

// ----------------------------------------------------------------------------
sc2::Point2D ControlGroup::NextHarassTarget() const
{
    return gAPI->observer().GameInfo().enemy_start_locations.front();
}

// ----------------------------------------------------------------------------
void ControlGroup::IssueMoveCommand(const sc2::Point2D& position)
{
    bool needToWait = false;

    for (auto& unit : GetUnits())
    {
        switch (unit->unit_type.ToType())
        {
            case sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
            {
                gAPI->action().Cast(*unit, sc2::ABILITY_ID::MORPH_UNSIEGE);
                needToWait = true;
                break;
            }
            
            case sc2::UNIT_TYPEID::TERRAN_LIBERATORAG:
            {
                gAPI->action().Cast(*unit, sc2::ABILITY_ID::MORPH_LIBERATORAAMODE);
                needToWait = true;
                break;
            }

            case sc2::UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED:
            {
                gAPI->action().Cast(*unit, sc2::ABILITY_ID::BURROWUP);
                needToWait = true;
                break;
            }

            default:
            {
                break;
            }
        }
    }

    if (!needToWait)
    {
        WrappedUnits units = GetUnits();
        gAPI->action().MoveTo(units, position);
    }
}