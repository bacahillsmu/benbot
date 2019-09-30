// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include <sc2api/sc2_unit.h>

struct Units
{
    explicit Units(const sc2::Units& units_);

    bool Empty() const;

    const sc2::Units& operator()() const;

    const sc2::Unit* GetClosestUnit(const sc2::Point2D& point_) const;
    const sc2::Unit* GetClosestUnit(const sc2::Point3D& point_) const;
    const sc2::Unit* GetClosestUnit(sc2::Tag tag_) const;
    const sc2::Unit* GetRandomUnit() const;
    bool HasOrder(sc2::ABILITY_ID id_) const;

    sc2::Units::iterator begin() { return m_units.begin(); }
    sc2::Units::iterator end() { return m_units.end(); }
    sc2::Units::const_iterator begin() const { return m_units.begin(); }
    sc2::Units::const_iterator end() const { return m_units.end(); }

    std::size_t size() const { return m_units.size(); }
    void reserve(size_t n) { return m_units.reserve(n); }
    bool empty() const { return m_units.empty(); }

    const sc2::Unit* at(std::size_t i) const { return m_units.at(i); }
    const sc2::Unit* operator[](std::size_t i) const { return m_units[i]; }
    const sc2::Unit* front() const { return m_units.front(); }
    const sc2::Unit* back() const { return m_units.back(); }

    void push_back(sc2::Unit* unit) { m_units.push_back(unit); }
    void pop_back() { m_units.pop_back(); }
    void clear() { m_units.clear(); }

    sc2::Units::iterator erase(sc2::Units::iterator it) { return m_units.erase(it); }
    sc2::Units::iterator erase(sc2::Units::iterator a, sc2::Units::iterator b) { return m_units.erase(a, b); }

    void remove(const sc2::Unit* unit);
    bool contains(const sc2::Unit* unit) const { return std::find(m_units.begin(), m_units.end(), unit) != m_units.end(); }

 private:
    sc2::Units m_units;
};
