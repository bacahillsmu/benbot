#pragma once

#include "plugins/micro/MicroPlugin.h"

#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_data.h>

#include <memory>

struct Worker;

namespace API 
{
	struct Action;
	struct Interface;
}

class WrappedUnit : public sc2::Unit 
{
	friend API::Action;
	friend API::Interface;

public:

	enum class Attackable { yes, no, need_scan };

	// TODO: Make "Make" function and constructor private (and same for worker) so only API can create new Unit objects
	static std::unique_ptr<WrappedUnit> Make(const sc2::Unit& unit_);
	WrappedUnit(const sc2::Unit& unit_);
	WrappedUnit(const WrappedUnit&) = delete;
	virtual ~WrappedUnit() = default;
	bool operator==(const WrappedUnit& other_) const;

	bool IsIdle() const;
	inline bool IsOrdersEmpty() const { return orders.empty(); }
	inline sc2::ABILITY_ID OrdersFrontAbilityID() const { return orders.front().ability_id; }
	inline sc2::UnitOrder OrderFront() const { return orders.front(); }

	int NumberOfOrders() const;
	const std::vector<sc2::UnitOrder>& GetPreviousStepOrders() const;
	MicroPlugin* Micro();
	sc2::UnitTypeData* GetTypeData() const;
	bool HasAttribute(sc2::Attribute attribute_) const;
	WrappedUnit* GetAttachedAddon() const;

	Attackable CanAttack(const WrappedUnit* other_) const;

	bool CanAttackFlying() const;

	int GetValue() const;

	sc2::Point3D GetPos() const;

	// False if unit is no longer visible to us.
	// NOTE: This only says if the unit can be seen at all. Some units in the fog
	// (i.e. above a ramp) can be seen and IsInVision = true for them
	// The name of this variable should probably be changed to clarify this
	bool IsInVision;

	bool m_engaged = false;
	

private:
	
	using sc2::Unit::orders;
	void UpdateAPIData(const sc2::Unit& unit);

	// This is set to true if an order (attack, move, stop etc.) has been given to the unit.
	// This variable should be reset at the start of every step (by calling the Update function).
	// (An alternative to having an update function would be to save the step number instead)
	bool m_order_queued_in_current_step = false;

	std::unique_ptr<MicroPlugin> m_micro;
	

	static constexpr float VespeneCostMod = 1.25f;
};

// Allow hashmap usage
namespace std 
{

	template<> struct hash<WrappedUnit> 
	{
		typedef WrappedUnit argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept 
		{
			return std::hash<sc2::Tag>{}(s.tag);
		}
	};

}

