// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill

#pragma once

#include "Builder.h"
#include "Plugin.h"

struct ReaperFirst : Plugin
{
	ReaperFirst();

	void OnUnitIdle(WrappedUnit* unit_, Builder* builder_) final;
	
private:
	enum State 
	{
		WAIT_BARRACKS,
		BUILDING_REAPER,
		MICRO_REAPER,
		HEAL_REAPER,
		FINISHED,
	};

	State m_state;
	sc2::Tag m_reaper;
};