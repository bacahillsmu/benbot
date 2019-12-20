#pragma once
#include "BehaviorTree/Composite.hpp"


class Sequence : public Composite
{

public:

	void Start() override;

	Status Run() override;

};

static Sequence* MakeSequence()
{
	return new Sequence;
}