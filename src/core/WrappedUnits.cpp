

#include "WrappedUnits.hpp"
#include "core/API.h"

#include <algorithm>
#include <numeric>
#include <limits>

// ----------------------------------------------------------------------------
WrappedUnits::WrappedUnits(std::vector<WrappedUnit*> units_)
{
    m_wrappedUnits = std::move(units_);
}

// ----------------------------------------------------------------------------
WrappedUnit* WrappedUnits::GetClosestUnit(const sc2::Point2D& point_)
{
    return const_cast<WrappedUnit*>(const_cast<const WrappedUnits*>(this)->GetClosestUnit(point_));
}

// ----------------------------------------------------------------------------
const WrappedUnit* WrappedUnits::GetClosestUnit(const sc2::Point2D& point_) const
{
    float distance = std::numeric_limits<float>::max();

    WrappedUnit* target = nullptr;
    for (const auto& i : m_wrappedUnits)
    {
        float d = sc2::DistanceSquared2D(i->pos, point_);
        if (d < distance)
        {
            distance = d;
            target = i;
        }
    }

    return target;
}

// ----------------------------------------------------------------------------
WrappedUnit* WrappedUnits::GetClosestUnit(sc2::Tag tag_)
{
    return const_cast<WrappedUnit*>(const_cast<const WrappedUnits*>(this)->GetClosestUnit(tag_));
}

// ----------------------------------------------------------------------------
const WrappedUnit* WrappedUnits::GetClosestUnit(sc2::Tag tag_) const
{
    WrappedUnit* unit = gAPI->observer().GetUnit(tag_);
    if (!unit)
    {
        return nullptr;
    }

    return GetClosestUnit(unit->pos);
}

// ----------------------------------------------------------------------------
WrappedUnit* WrappedUnits::GetRandomUnit()
{
    return const_cast<WrappedUnit*>(const_cast<const WrappedUnits*>(this)->GetRandomUnit());
}

// ----------------------------------------------------------------------------
const WrappedUnit* WrappedUnits::GetRandomUnit() const
{
    if (empty())
    {
        return nullptr;
    }

    int index = sc2::GetRandomInteger(0, static_cast<int>(size()) - 1);

    return m_wrappedUnits[static_cast<unsigned>(index)];
}

// ----------------------------------------------------------------------------
std::pair<sc2::Point2D, float> WrappedUnits::CalculateCircle() const
{
    if (m_wrappedUnits.empty())
    {
        return std::make_pair(sc2::Point2D(), 0.0f);
    }

    // Centroid of a finite set of points
    sc2::Point2D center = std::accumulate(m_wrappedUnits.begin()
                                        , m_wrappedUnits.end()
                                        , sc2::Point2D(0, 0),
                                        [](const sc2::Point2D& p, const WrappedUnit* u)
                                        {
                                            return p + u->pos;
                                        });

    center /= static_cast<float>(m_wrappedUnits.size());

    // Find unit furthest from center
    auto u = std::max_element(m_wrappedUnits.begin()
                            , m_wrappedUnits.end(),
                            [&center](const WrappedUnit* a, const WrappedUnit* b)
                            {
                                return DistanceSquared2D(center, a->pos) < DistanceSquared2D(center, b->pos);
                            });

    // Use that to calculate radius of circle
    float radius = Distance2D((*u)->pos, center);

    return std::make_pair(center, radius);
}

// ----------------------------------------------------------------------------
sc2::Units WrappedUnits::ToAPI() const
{
    sc2::Units apiUnits;
    apiUnits.reserve(m_wrappedUnits.size());

    for (auto& unit : m_wrappedUnits)
    {
        apiUnits.push_back(unit);
    }

    return apiUnits;
}

// ----------------------------------------------------------------------------
void WrappedUnits::remove(const WrappedUnit* unit)
{
    auto itr = std::find(m_wrappedUnits.begin(), m_wrappedUnits.end(), unit);

    if (itr != m_wrappedUnits.end())
    {
        m_wrappedUnits.erase(itr);
    }
}