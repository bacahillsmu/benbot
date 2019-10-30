// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

//#include "Builder.h"

#include <sc2api/sc2_unit.h>

struct Builder;
class WrappedUnit;

struct Plugin
{
    virtual ~Plugin() {
    }

    virtual void OnGameStart(Builder*) {
    }

    virtual void OnStep(Builder*) {
    }

    virtual void OnUnitCreated(WrappedUnit*, Builder*) {
    }

    virtual void OnUnitDestroyed(WrappedUnit*, Builder*) {
    }

    virtual void OnUnitIdle(WrappedUnit*, Builder*) {
    }

    virtual void OnUpgradeCompleted(sc2::UpgradeID) {
    }

    virtual void OnGameEnd() {
    }
};
