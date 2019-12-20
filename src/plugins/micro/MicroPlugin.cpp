#include "MicroPlugin.h"
#include "DefaultUnit.h"
#include "Reaper.h"
#include "Hub.h"
#include "core/API.h"


std::unique_ptr<MicroPlugin> MicroPlugin::MakePlugin(WrappedUnit* unit)
{
    switch (unit->unit_type.ToType())
    {
        case sc2::UNIT_TYPEID::TERRAN_REAPER:
        {
            return std::make_unique<Reaper>(unit);
        }

        default:
        {
            return std::make_unique<DefaultUnit>(unit);
        }
    }
}

MicroPlugin::MicroPlugin(WrappedUnit* unit)
    : m_self(unit)
    , m_target(nullptr)
    , m_moving(false)
{
}

void MicroPlugin::OnCombatFrame(WrappedUnit* self, const WrappedUnits& enemies, const WrappedUnits& allies, const sc2::Point2D& attackMovePos)
{
    m_self = self;
    m_attackMovePos = attackMovePos;

    // Request scan logic
    for (auto& enemy : enemies)
    {
        if (self->CanAttack(gAPI->observer().GetUnit(enemy->tag)) == WrappedUnit::Attackable::need_scan)
        {
            gHub->RequestScan(enemy->pos);
            break;
        }
    }

    OnCombatStep(enemies, allies);
}

void MicroPlugin::OnCombatOver(WrappedUnit* self)
{
    m_self = self;

    OnCombatEnded();

    m_target = nullptr;
    m_moving = false;
}

bool MicroPlugin::CanCast(sc2::ABILITY_ID ability_id) const
{
    if (!m_self)
    {
        return false;
    }

    for (auto& ability : gAPI->query().GetAbilitiesForUnit(m_self, false).abilities)
    {
        if (ability.ability_id.ToType() == ability_id)
        {
            return true;
        }
    }

    return false;
}

void MicroPlugin::Attack(const WrappedUnit* target)
{
    if (m_self && !IsAttacking(target))
    {
        gAPI->action().Attack(*m_self, *target);
        m_target = target;
        m_moving = false;
    }
}

void MicroPlugin::AttackMove()
{
    AttackMove(m_attackMovePos);
}

void MicroPlugin::AttackMove(const sc2::Point2D& pos)
{
    if (m_self)
    {
        if (!CanCast(sc2::ABILITY_ID::ATTACK_ATTACK))
        {
            gAPI->action().Cast(*m_self, sc2::ABILITY_ID::ATTACK, pos);
        }
        else
        {
            gAPI->action().Attack(*m_self, pos);
        }
            
        m_target = nullptr;
        m_moving = false;
    }
}

void MicroPlugin::MoveTo(const sc2::Point2D& pos)
{
    if (m_self)
    {
        gAPI->action().MoveTo(*m_self, pos);

        m_target = nullptr;
        m_moving = true;
    }
}

bool MicroPlugin::HasBuff(sc2::BUFF_ID buff)
{
    if (m_self)
    {
        return false;
    }

    return std::find(m_self->buffs.begin(), m_self->buffs.end(), buff) != m_self->buffs.end();
}

void MicroPlugin::Cast(sc2::ABILITY_ID ability)
{
    if (m_self && CanCast(ability))
    {
        gAPI->action().Cast(*m_self, ability);
    }
}

void MicroPlugin::Cast(sc2::ABILITY_ID ability, const WrappedUnit* target)
{
    if (m_self && CanCast(ability))
    {
        gAPI->action().Cast(*m_self, ability, *target);
    }
}

void MicroPlugin::Cast(sc2::ABILITY_ID ability, const sc2::Point2D& point)
{
    if (m_self && CanCast(ability))
    {
        gAPI->action().Cast(*m_self, ability, point);
    }
}

bool MicroPlugin::IsAttacking(const WrappedUnit* target)
{
    return m_target == target;
}

bool MicroPlugin::IsMoving() const
{
    return m_moving;
}

// bool MicroPlugin::IsAttackMoving() const
// {
//     return m_self
//         && !m_target
//         && !m_self->GetPreviousStepOrders().empty()
//         && m_self->GetPreviousStepOrders().front().ability_id == sc2::ABILITY_ID::ATTACK;
// }
// 
// bool MicroPlugin::IsAttackMoving(const sc2::Point2D& pos) const
// {
//     return IsAttackMoving() &&
//         sc2::DistanceSquared2D(pos, m_self->GetPreviousStepOrders().front().target_pos) <=
//         AttackMoveOutOfDateDistance * AttackMoveOutOfDateDistance;
// }

