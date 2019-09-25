// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Blueprint.h"
#include "Building.h"
#include "Creature.h"
#include "GateUnit.h"
#include "Mutation.h"
#include "Queen.h"
#include "Refinery.h"
#include "Research.h"
#include "TownHall.h"
#include "Addon.h"
#include "Unit.h"
#include "core/API.h"
#include "core/Errors.h"

#include <memory>

Blueprint::~Blueprint()
{
}

std::shared_ptr<Blueprint> Blueprint::Plot(sc2::ABILITY_ID ability_)
{
    switch (ability_)
    {
        case sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS:
        case sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS:
        case sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY:
        case sc2::ABILITY_ID::BUILD_REACTOR_FACTORY:
        case sc2::ABILITY_ID::BUILD_TECHLAB_STARPORT:
        case sc2::ABILITY_ID::BUILD_REACTOR_STARPORT:
        {
            return std::make_shared<Addon>();
        }

        case sc2::ABILITY_ID::BUILD_REFINERY:
        {
            return std::make_shared<Refinery>();
        }

        case sc2::ABILITY_ID::TRAIN_BANSHEE:
        case sc2::ABILITY_ID::TRAIN_BATTLECRUISER:
        case sc2::ABILITY_ID::TRAIN_LIBERATOR:
        case sc2::ABILITY_ID::TRAIN_MEDIVAC:
        case sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER:
        {
            return std::make_shared<Unit>(sc2::UNIT_TYPEID::TERRAN_STARPORT);
        }

        case sc2::ABILITY_ID::TRAIN_CYCLONE:
        case sc2::ABILITY_ID::TRAIN_HELLION:
        case sc2::ABILITY_ID::TRAIN_HELLBAT:
        case sc2::ABILITY_ID::TRAIN_SIEGETANK:
        case sc2::ABILITY_ID::TRAIN_THOR:
        case sc2::ABILITY_ID::TRAIN_WIDOWMINE:
        {
            return std::make_shared<Unit>(sc2::UNIT_TYPEID::TERRAN_FACTORY);
        }

        case sc2::ABILITY_ID::TRAIN_GHOST:
        case sc2::ABILITY_ID::TRAIN_MARINE:
        case sc2::ABILITY_ID::TRAIN_MARAUDER:
        case sc2::ABILITY_ID::TRAIN_REAPER:
        {
            return std::make_shared<Unit>(sc2::UNIT_TYPEID::TERRAN_BARRACKS);
        }

        case sc2::ABILITY_ID::TRAIN_SCV:
        {
            return std::make_shared<Unit>(sc2::UNIT_TYPEID::INVALID);
        }

        case sc2::ABILITY_ID::RESEARCH_HISECAUTOTRACKING:
        case sc2::ABILITY_ID::RESEARCH_NEOSTEELFRAME:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANSTRUCTUREARMORUPGRADE:
        {
            return std::make_shared<Research>(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
        }

        case sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL3:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL1:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL2:
        case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL3:
        {
            return std::make_shared<Research>(sc2::UNIT_TYPEID::TERRAN_ARMORY);
        }

        case sc2::ABILITY_ID::RESEARCH_BATTLECRUISERWEAPONREFIT:
        {
            return std::make_shared<Research>(sc2::UNIT_TYPEID::TERRAN_FUSIONCORE);
        }

        case sc2::ABILITY_ID::RESEARCH_PERSONALCLOAKING:
        {
            return std::make_shared<Research>(sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY);
        }

        case sc2::ABILITY_ID::BUILD_COMMANDCENTER:
        {
            return std::make_shared<TownHall>();
        }

        default:
        {
            return std::make_shared<Building>();
        }
    }
}
