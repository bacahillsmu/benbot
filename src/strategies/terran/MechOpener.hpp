// The Ben License (BEN)
//
// Copyright (c) 2019 Benjamin Cahill

#pragma once

#include "Builder.h"
#include "strategies/Strategy.h"
#include "BrainTree/BrainTree.h"
#include "BrainTree/Action.hpp"

#include "plugins/micro/MicroPlugin.h"

struct MechOpener : Strategy
{
	MechOpener();

	void OnStep(Builder* builder_) final;
	void OnGameStart(Builder* builder_) final;
	void OnUnitIdle(WrappedUnit* unit_, Builder* builder_) final;
	void OnUnitCreated(WrappedUnit* unit_, Builder* builder_) final;

	//void OnCombatStep(const Units& enemies, const Units& allies) override;

private:

	void BuildMarines(WrappedUnit* unit_, Builder* builder_);

	enum State
	{
		WAIT_REAPER,
		BUILDING_REAPER,
		BUILDING_REACTOR,
		FACTORY_PRODUCTION
	};

	State m_state;

	BrainTree::BehaviorTree tree;

	bool m_buildMarines = false;

};