// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include "core/WrappedUnit.hpp"

#include <sc2api/sc2_unit.h>

#include <functional>
#include <memory>
#include <utility>


class WrappedUnits
{

public:

    using T = std::vector<WrappedUnit*>;

    WrappedUnits() = default;
    explicit WrappedUnits(std::vector<WrappedUnit*> units_);

    WrappedUnit* GetClosestUnit(const sc2::Point2D& point_);
    WrappedUnit* GetClosestUnit(const sc2::Point2D& point_) const;

    WrappedUnit* GetClosestUnit(sc2::Tag tag_);
    const WrappedUnit* GetClosestUnit(sc2::Tag tag_) const;

    WrappedUnit* GetRandomUnit();
    const WrappedUnit* GetRandomUnit() const;

    // Calculate the center point of all units and the radius of the circle encompassing them
    std::pair<sc2::Point2D, float> CalculateCircle() const;

    // Returns a copy of Units as an API-recognizable vector with sc2::Unit objects
    sc2::Units ToAPI() const;

    // Common functions found in std::vector implementations
    T::iterator begin() { return m_wrappedUnits.begin(); }
    T::iterator end() { return m_wrappedUnits.end(); }
    T::const_iterator begin() const { return m_wrappedUnits.begin(); }
    T::const_iterator end() const { return m_wrappedUnits.end(); }
    std::size_t size() const { return m_wrappedUnits.size(); }
    void reserve(size_t n) { return m_wrappedUnits.reserve(n); }
    bool empty() const { return m_wrappedUnits.empty(); }
    WrappedUnit* at(std::size_t i) { return m_wrappedUnits.at(i); }
    const WrappedUnit* at(std::size_t i) const { return m_wrappedUnits.at(i); }
    WrappedUnit* operator[](std::size_t i) { return m_wrappedUnits[i]; }
    const WrappedUnit* operator[](std::size_t i) const { return m_wrappedUnits[i]; }
    WrappedUnit* front() { return m_wrappedUnits.front(); }
    const WrappedUnit* front() const { return m_wrappedUnits.front(); }
    WrappedUnit* back() { return m_wrappedUnits.back(); }
    const WrappedUnit* back() const { return m_wrappedUnits.back(); }
    void push_back(WrappedUnit* unit) { m_wrappedUnits.push_back(unit); }
    void emplace_back(WrappedUnit* unit) { m_wrappedUnits.emplace_back(unit); }
    void pop_back() { m_wrappedUnits.pop_back(); }
    void clear() { m_wrappedUnits.clear(); }
    T::iterator erase(T::iterator it) { return m_wrappedUnits.erase(it); }
    T::iterator erase(T::iterator a, T::iterator b) { return m_wrappedUnits.erase(a, b); }

    void remove(const WrappedUnit* unit);
    bool contains(const WrappedUnit* unit) const { return std::find(m_wrappedUnits.begin(), m_wrappedUnits.end(), unit) != m_wrappedUnits.end(); }

private:
    T m_wrappedUnits;
};
