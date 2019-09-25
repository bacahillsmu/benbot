// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill

#pragma once

#include "core/Order.h"

#include <list>
#include "Builder.h"
#include "strategies/Strategy.h"

struct HatchFirst17 : Strategy
{

    HatchFirst17();

    void OnGameStart(Builder* builder_) final;
    void OnStep(Builder* builder_) final;
    void OnUnitIdle(const sc2::Unit* unit_, Builder* builder_) final;
    void OnUnitCreated(const sc2::Unit* unit_, Builder*) final;

private:

    std::list<Order> m_opening_orders;
};


