#pragma once
#include "Builder.h"
#include "plugins/Plugin.h"

struct ReaperRush : Plugin
{

public:

	ReaperRush();

	void OnGameStart(Builder* builder_) final;
	void OnUnitIdle(WrappedUnit* unit_, Builder* builder_) final;
	void OnUnitCreated(WrappedUnit* unit_, Builder* builder_) final;
	void OnStep(Builder* builder_) final;
	
private:

	int m_reapersBuilt = 1;




};