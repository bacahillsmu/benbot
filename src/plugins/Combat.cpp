#include "Combat.hpp"

#include "core/WrappedUnits.hpp"
#include "Builder.h"
#include "core/API.h"
#include "core/Helpers.h"


// ----------------------------------------------------------------------------
void Combat::OnStep(Builder* builder_)
{
    builder_;

    if(!m_mainControlGroup.IsSent() && m_mainControlGroup.size() > 0)
    {
        m_mainControlGroup.Send();
    }
    m_mainControlGroup.OnStep();

    
}

// ----------------------------------------------------------------------------
void Combat::OnUnitCreated(WrappedUnit* unit_, Builder* builder_)
{
    builder_;

    if(!IsCombatUnit()(unit_))
    {
        return;
    }

    // We need to have ControlGroups for different types; Main, Harrass, Defense, Scout, Offense;

    // We only support Reaper for the time being;
    if(unit_->unit_type == sc2::UNIT_TYPEID::TERRAN_REAPER)
    {
        m_mainControlGroup.AddUnit(unit_);
    }
}


