// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include "Builder.h"
#include "Plugin.h"

struct QuarterMaster : Plugin
{
    QuarterMaster();

    void OnStep(Builder* builder_) final;
    void OnUnitCreated(WrappedUnit* unit_, Builder*) final;


 private:

    bool m_skip_turn;
    float m_planningAhead = 8.0f;
};
