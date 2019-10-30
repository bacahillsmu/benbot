#pragma once

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_typeenums.h>
#include <sc2api/sc2_unit.h>

#include <memory>
#include <vector>

class WrappedUnit;
class WrappedUnits;

class MicroPlugin
{

public:

    explicit MicroPlugin(const WrappedUnit* unit);
    virtual ~MicroPlugin() = default;

    void OnCombatFrame(const WrappedUnit* self, const WrappedUnits& enemies, const WrappedUnits& allies, const sc2::Point2D& attackMovePos);
    void OnCombatOver(const WrappedUnit* self);

    static std::unique_ptr<MicroPlugin> MakePlugin(const WrappedUnit* unit);

protected:

    virtual void OnCombatStep(const WrappedUnits& enemies, const WrappedUnits& allies) = 0;
    virtual void OnCombatEnded() { }

    bool CanCast(sc2::ABILITY_ID ability_id);
    void Attack(const WrappedUnit* target);
    void AttackMove();                          // Attack move towards enemies;   
    void AttackMove(const sc2::Point2D& pos);   // Attack move towards specific spot;
    void MoveTo(const sc2::Point2D& pos);
    bool HasBuff(sc2::BUFF_ID buff);
    void Cast(sc2::ABILITY_ID ability);
    void Cast(sc2::ABILITY_ID ability, const WrappedUnit* target);
    void Cast(sc2::ABILITY_ID ability, const sc2::Point2D& point);
    bool IsAttacking(const WrappedUnit* target) const;
    bool IsMoving() const;
    //bool IsAttackMoving() const;
    //bool IsAttackMoving(const sc2::Point2D& pos) const;

    const WrappedUnit* m_self;

private:

    const WrappedUnit* m_target;
    sc2::Point2D m_attackMovePos;
    bool m_moving;                  // Move command; does not include Attack Move command

    static constexpr float AttackMoveOutOfDateDistance = 10.0f; // Update attack move position if it moves this much
};
