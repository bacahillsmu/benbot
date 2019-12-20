#include "DefaultUnit.h"
#include "core/API.h"

DefaultUnit::DefaultUnit(WrappedUnit* unit)
    : MicroPlugin(unit) {}

void DefaultUnit::OnCombatStep(const WrappedUnits& enemies, const WrappedUnits& allies)
{
    // UNUSED;
    enemies;
    allies;

    AttackMove();
}
