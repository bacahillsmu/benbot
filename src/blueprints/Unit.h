// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include "Blueprint.h"

struct Unit: Blueprint
{
    explicit Unit(sc2::UNIT_TYPEID who_builds_, sc2::UNIT_TYPEID required_addon_ = sc2::UNIT_TYPEID::INVALID);

    bool CanBeBuilt(const Order* order_) final;
    bool Build(Order* order_) final;

 private:

    sc2::UNIT_TYPEID m_who_builds;
    sc2::UNIT_TYPEID m_required_addon;
};
