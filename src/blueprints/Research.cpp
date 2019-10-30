// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Research.h"
#include "core/API.h"
#include "core/Helpers.h"

Research::Research(sc2::UNIT_TYPEID who_builds_)
    : m_who_builds(who_builds_)
{
}

bool Research::CanBeBuilt(const Order* order_)
{
    order_;
    return true;
}

bool Research::Build(Order* order_)
{
    WrappedUnits producers = gAPI->observer().GetUnits(sc2::Unit::Alliance::Self);

    if (producers.empty())
    {
        return false;
    }

    order_->assignee = producers.front();
    gAPI->action().Build(*order_);

    return true;
}
