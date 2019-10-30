// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include "plugins/Plugin.h"
#include "core/WrappedUnits.hpp"

struct Strategy : Plugin
{
    explicit Strategy(float attack_limit_);

    void OnStep(Builder*) override;
    void OnUnitCreated(WrappedUnit* unit_, Builder*) override;

 protected:
    float m_attack_limit;
    WrappedUnits m_units;
};
