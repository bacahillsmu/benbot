// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill

#pragma once

#include "Plugin.h"

struct InjectHatcheries : Plugin
{
    void OnStep(Builder*) final;

};
