#include "Reaper.h"
#include "core/API.h"

Reaper::Reaper(const WrappedUnit* unit)
        : MicroPlugin(unit)
{
}

void Reaper::OnCombatStep(const WrappedUnits& enemies, const WrappedUnits& allies)
{
    allies;

    const WrappedUnit* target = enemies.GetClosestUnit(m_self->pos);

    if ((m_self->health) < (35)
    && !((gAPI->observer().StartingLocation().x == m_self->pos.x)
    && (gAPI->observer().StartingLocation().y == m_self->pos.y)))
    {
        if (Distance2D(target->pos, m_self->pos) < 5)
        {
            Cast(sc2::ABILITY_ID::EFFECT_KD8CHARGE, target);
        }

        MoveTo(sc2::Point2D(gAPI->observer().StartingLocation().x, gAPI->observer().StartingLocation().y));

    }
    else
    {
        if (m_self->weapon_cooldown == 0)
        {
            if (Distance2D(target->pos, m_self->pos) < 25)
            {
                Cast(sc2::ABILITY_ID::EFFECT_KD8CHARGE, target);
                Cast(sc2::ABILITY_ID::SMART, target);
            }
        }
        else
        {
            if (Distance2D(target->pos, m_self->pos) < 5)
            {
                Cast(sc2::ABILITY_ID::EFFECT_KD8CHARGE, target);
            }

            MoveTo(sc2::Point2D(gAPI->observer().StartingLocation().x, gAPI->observer().StartingLocation().y));
        }
    }
}
