#pragma once
#include "BehaviorTree/Composite.hpp"

class Selector : public Composite
{

public:

	void Start() override;

	Status Run() override;

};

static Selector* MakeSelector()
{
	return new Selector();
}
