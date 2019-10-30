// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#pragma once

#include "Builder.h"
#include "Plugin.h"

struct Miner : Plugin
{
    void OnStep(Builder* builder_) final;
    void OnUnitCreated(WrappedUnit* unit_, Builder*) final;
    void OnUnitDestroyed(WrappedUnit* unit_, Builder*) final;
    void OnUnitIdle(WrappedUnit* unit_, Builder*) final;

    static void SecureMineralsIncome(Builder* builder_);
    void SecureVespeneIncome();
    static void BalanceWorkers();
    static void CallDownMULE();
    static void ClearWorkersHomeBaseIfNoActiveExpansion(const std::shared_ptr<Expansion>& expansion_);
    static void SplitWorkersOffFromThisExpansion(const std::shared_ptr<Expansion>& expansion_);

private:

    bool m_vespeneGasStop = false;

    static constexpr int m_muleEnergyCost = 50;
    static constexpr int m_stepsBetweenBalance = 20;
    static constexpr int m_reqImbalanceToTransfer = 2;
    static constexpr int m_maximumWorkers = 70;
    static constexpr int m_vespeneMinimumForStopThreshold = 1200;
    static constexpr int m_vespeneStartThreshold = 1000;
    static constexpr float m_vespeneToMineralsStopRatio = 3.f;
    static constexpr float m_vespeneToMineralsStartRatio = 2.5f;

};
