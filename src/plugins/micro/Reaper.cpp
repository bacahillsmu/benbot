#include "Reaper.h"
#include "core/API.h"
#include "Composites/Selector.hpp"

Reaper::Reaper(WrappedUnit* unit)
        : MicroPlugin(unit)
{
    // This is so ugly... I liked the c++ tree based creation, idk if I can do that. It had lots of tricks;
    Blackboard* blackboard = m_behaviorTree.GetBlackboard();

    Selector* weaponSelector = MakeSelector();
    FireWeapon* fireWeapon = new FireWeapon(blackboard);
    BackUpAndThrowKD8Charge* backUpAndThrowKD8Charge = new BackUpAndThrowKD8Charge(blackboard);
    weaponSelector->AddChild(fireWeapon);
    weaponSelector->AddChild(backUpAndThrowKD8Charge);

    Selector* reaperSelector = MakeSelector();
    RetreatBecauseOfHealth* retreatBecauseOfHealth = new RetreatBecauseOfHealth(blackboard);
    reaperSelector->AddChild(retreatBecauseOfHealth);
    reaperSelector->AddChild(weaponSelector);

    m_behaviorTree.SetRootNode(reaperSelector);
}

void Reaper::OnCombatStep(const WrappedUnits& enemies, const WrappedUnits& allies)
{
    m_behaviorTree.GetBlackboard()->SetKeyValue("self", m_self);
    m_behaviorTree.GetBlackboard()->SetKeyValue("target", enemies.GetClosestUnit(m_self->pos));

    m_behaviorTree.Run();
}
