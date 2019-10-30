// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include "Builder.h"
#include "Plugin.h"

struct RepairMan : Plugin
{
    void OnStep(Builder* builder_) final;

    void OnUnitDestroyed(WrappedUnit* unit_, Builder* builder_) final;
};
