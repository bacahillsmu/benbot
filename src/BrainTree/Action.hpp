#pragma once

#include "BrainTree/BrainTree.h"


class AttackAction : public BrainTree::Node
{

public:

	Status update() override
	{
		
		
		return BrainTree::Node::Status::Success;
	}
};


class FleeAction : public BrainTree::Node
{

public:

	Status update() override
	{


		return BrainTree::Node::Status::Success;
	}
};