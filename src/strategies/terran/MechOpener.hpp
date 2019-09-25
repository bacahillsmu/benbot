// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill

#pragma once

#include "Builder.h"
#include "strategies/Strategy.h"


struct MechOpener : Strategy
{
	MechOpener();

	void OnStep(Builder* builder_) final;
	void OnGameStart(Builder* builder_) final;
	void OnUnitIdle(const sc2::Unit* unit_, Builder* builder_) final;
	void OnUnitCreated(const sc2::Unit* unit_, Builder* builder_) final;

private:

	void BuildMarines(const sc2::Unit* unit_, Builder* builder_);

	enum State
	{
		WAIT_REAPER,
		BUILDING_REAPER,
		BUILDING_REACTOR,
		FACTORY_PRODUCTION
	};

	State m_state;

	bool m_buildMarines = false;

};