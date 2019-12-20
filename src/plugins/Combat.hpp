#pragma once


#include "groups/ControlGroup.hpp"


#include "Plugin.h"

class WrappedUnits;

struct Combat : Plugin
{

public:

	void OnStep(Builder* builder_) final;
	void OnUnitCreated(WrappedUnit* unit_, Builder* builder_) final;
	void OnUnitDestroyed(WrappedUnit* unit_, Builder*);


private:

	ControlGroup m_mainControlGroup;
	std::vector<ControlGroup> m_reaperControlGroups;
	
};