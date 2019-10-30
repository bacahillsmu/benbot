#pragma once

#include "MicroPlugin.h"


class Reaper : public MicroPlugin
{

public:

    explicit Reaper(const WrappedUnit* unit);

    void OnCombatStep(const WrappedUnits& enemies, const WrappedUnits& allies) override;
};
