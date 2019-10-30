// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include "GameObject.h"
#include "core/WrappedUnit.hpp"
#include "core/Order.h"

struct Expansion;

struct Worker: public WrappedUnit
{

public:

    enum class Job
    {
        GATHERING_NOTHING = 0,
        GATHERING_MINERALS,
        GATHERING_VESPENE,
        BUILDING,
        BUILDING_REFINERY,
    };

    explicit Worker(const sc2::Unit& unit_);

    void BuildRefinery(Order* order_, const WrappedUnit* geyser_);
    void Build(Order* order_, const sc2::Point2D& point_);
    void GatherVespene(const WrappedUnit& target_);
    void Mine();

    void SetHomeBase(std::shared_ptr<Expansion> base_);
    std::shared_ptr<Expansion> GetHomeBase() const;


    Job GetJob() const;
    void SetAsNotGathering();

 private:
    Job m_job = Job::GATHERING_NOTHING;
    std::shared_ptr<Expansion> m_homeBase = nullptr;
};
