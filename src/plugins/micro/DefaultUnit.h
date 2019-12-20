#pragma once

#include "MicroPlugin.h"

class DefaultUnit : public MicroPlugin
{

public:

    explicit DefaultUnit(WrappedUnit* unit);

    void OnCombatStep(const WrappedUnits& enemies, const WrappedUnits& allies) override;
};
