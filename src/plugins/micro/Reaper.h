#pragma once
#include "MicroPlugin.h"
#include "core/API.h"
#include "BehaviorTree.hpp"
#include "Leaf.hpp"
#include "Blackboard.hpp"


class Reaper : public MicroPlugin
{

public:

    explicit Reaper(WrappedUnit* unit);

    void OnCombatStep(const WrappedUnits& enemies, const WrappedUnits& allies) override;

private:

    BehaviorTree m_behaviorTree;
    

};

// ----------------------------------------------
// Reaper Specific Behaviors
// ----------------------------------------------
class RetreatBecauseOfHealth : public Leaf
{

public:

    RetreatBecauseOfHealth(Blackboard* blackboard_)
        : Leaf(blackboard_)
    {
    }

    Status Run() override
    {
        WrappedUnit* self = m_blackboard->GetWrappedUnitFromKey("self");
        WrappedUnit* target = m_blackboard->GetWrappedUnitFromKey("target");

        if ((self->health) < (35) && !((gAPI->observer().StartingLocation().x == self->pos.x)
            && (gAPI->observer().StartingLocation().y == self->pos.y)))
        {
            if (Distance2D(target->pos, self->pos) < 5)
            {
                self->Micro()->Cast(sc2::ABILITY_ID::EFFECT_KD8CHARGE, target);
            }

            self->Micro()->MoveTo(sc2::Point2D(gAPI->observer().StartingLocation().x, gAPI->observer().StartingLocation().y));

            return Node::Status::SUCCESS;
        }

        return Node::Status::FAILURE;
    }
};

// ----------------------------------------------
class FireWeapon : public Leaf
{

public:

    FireWeapon(Blackboard* blackboard_)
        : Leaf(blackboard_)
    {
    }

    Status Run() override
    {
        WrappedUnit* self = m_blackboard->GetWrappedUnitFromKey("self");
        WrappedUnit* target = m_blackboard->GetWrappedUnitFromKey("target");

        if (self->weapon_cooldown == 0)
        {
            if (Distance2D(target->pos, self->pos) < 25)
            {
                self->Micro()->Cast(sc2::ABILITY_ID::EFFECT_KD8CHARGE, target);
                self->Micro()->Cast(sc2::ABILITY_ID::SMART, target);
            }

            return Node::Status::SUCCESS;
        }

        return Node::Status::FAILURE;
    }

};

// ----------------------------------------------
class BackUpAndThrowKD8Charge : public Leaf
{

public:

    BackUpAndThrowKD8Charge(Blackboard* blackboard_)
        : Leaf(blackboard_)
    {
    }

    Status Run() override
    {
        WrappedUnit* self = m_blackboard->GetWrappedUnitFromKey("self");
        WrappedUnit* target = m_blackboard->GetWrappedUnitFromKey("target");

        if (Distance2D(target->pos, self->pos) < 5)
        {
            self->Micro()->Cast(sc2::ABILITY_ID::EFFECT_KD8CHARGE, target);
        }

        if(self->health != 60.0f)
        {
            self->Micro()->MoveTo(sc2::Point2D(gAPI->observer().StartingLocation().x, gAPI->observer().StartingLocation().y));
        }        

        return Node::Status::SUCCESS;
    }
};
