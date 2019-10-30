// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include "Blueprint.h"

struct Building: Blueprint
{
    bool CanBeBuilt(const Order* order_) final;
    bool Build(Order* order_) final;
};
