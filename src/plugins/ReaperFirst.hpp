// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill

#pragma once

#include "Builder.h"
#include "Plugin.h"

struct ReaperFirst : Plugin
{
	ReaperFirst();

	void OnStep(Builder*) final;
	void OnUnitCreated(const sc2::Unit* unit_, Builder* builder_) final;
	void OnUnitIdle(const sc2::Unit* unit_, Builder* builder_) final;
	void OnUnitDestroyed(const sc2::Unit* unit_, Builder*) final;

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