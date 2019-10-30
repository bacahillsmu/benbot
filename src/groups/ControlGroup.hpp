#pragma once

#include "core/WrappedUnits.hpp"


class ControlGroup
{

public:

	ControlGroup();
	virtual ~ControlGroup() = default;

	bool IsTaskFinished();

	void OnStep();
	void Update();
	void UpdateMovement();
	void AbortMovement();

	// Our Units;
	void AddUnit(WrappedUnit* unit_);
	void RemoveUnit(WrappedUnit* unit_);
	const WrappedUnits& GetUnits() const;

	// Enemy Units;
	void AddEnemy(WrappedUnit* enemy_);
	void RemoveEnemy(WrappedUnit* enemy_);
	const WrappedUnits& GetEnemyUnits() const;
	void SetEnemies(WrappedUnits enemies);

	void Send();
	void Approach(const sc2::Point2D& position);
	void RegroupAt(const sc2::Point2D& position);

	inline const sc2::Point2D& GetCenter() const { return m_center; }
	inline float GetSpreadRadius() const { return m_spreadRadius; }
	inline const sc2::Point2D& GetEnemyCenter() const { return m_enemyCenter; }
	inline float GetEnemySpreadRadius() const { return m_enemySpreadRadius; }
	inline bool IsMoving() const { return m_moveState != MovementState::IDLE; }
	inline const sc2::Point2D& GetAttackMovePoint() const {	return m_enemyCenter; }
	inline bool IsSent() const { return m_sent; }
	inline int size() const { return (int)m_units.size(); }

private:

	void CalculateCenter();
	sc2::Point2D NextHarassTarget() const;
	void IssueMoveCommand(const sc2::Point2D& position);

	WrappedUnits m_units;
	WrappedUnits m_enemies;
	sc2::Point2D m_center;
	sc2::Point2D m_enemyCenter;
	sc2::Point2D m_regroupPos;
	sc2::Point2D m_approachPos;
	float m_spreadRadius = 0;
	float m_enemySpreadRadius = 0;
	bool m_sent = true;
	bool m_wasApproaching = false;

	enum class MovementState 
	{
		IDLE,
		APPROACH,
		REGROUP
	};
	MovementState m_moveState = MovementState::IDLE;
	
	static constexpr float AggroRadius = 15.0f;
	static constexpr float RegroupRadius = 15.0f;
};