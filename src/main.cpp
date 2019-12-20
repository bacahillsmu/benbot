// The MIT License (MIT)
//
// Copyright (c) 2017-2019 Alexander Kurbatov

#include "Dispatcher.h"
#include "Historican.h"
#include "core/Converter.h"

#include <sc2api/sc2_coordinator.h>
#include <sc2api/sc2_gametypes.h>
#include <sc2utils/sc2_arg_parser.h>
#include <sc2utils/sc2_manage_process.h>

#include <iostream>

// I put this in the Preprocesser Definitions: _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING. Ask Forseth.

// class Human : public sc2::Agent
// {
// public:
//     void OnGameStart() final
//     {
//         Debug()->DebugTextOut("Human");
//         Debug()->SendDebug();
// 
//     }
//     void OnStep()
//     {
//         Control()->GetObservation();
//     }
// };

#ifdef DEBUG
int main(int argc, char* argv[])
{
    // Make sure map name was provided;
    if (argc < 2)
    {
        std::cerr << "Provide either name of the map file or path to it!" << std::endl;
        return -1;
    }

    // Open the History Log file;
    gHistory.Init("history.log");

    // Used to start, step and stop games and replays.
    sc2::Coordinator coordinator;
    //coordinator.LoadSettings(1, argv);
    coordinator.LoadSettings(argc, argv);

    Dispatcher bot("TrainingDummy");
    //Human human;
    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
        //CreateParticipant(sc2::Race::Zerg, &human),
        CreateComputer(sc2::Race::Random, sc2::Difficulty::Hard)
    });

    srand((unsigned int)time(0));
    int random = rand() % 7;
    std::string mapName = "";
    if(random == 0)
    {
        mapName = "AcropolisLE.SC2Map";
    }
    else if(random == 1)
    {
        mapName = "DiscoBloodbathLE.SC2Map";
    }
    else if (random == 2)
    {
        mapName = "EphemeronLE.SC2Map";
    }
    else if (random == 3)
    {
        mapName = "ThunderbirdLE.SC2Map";
    }
    else if (random == 4)
    {
        mapName = "TritonLE.SC2Map";
    }
    else if (random == 5)
    {
        mapName = "WintersGateLE.SC2Map";
    }
    else if (random == 6)
    {
        mapName = "WorldofSleepersLE.SC2Map";
    }

    coordinator.LaunchStarcraft();
    coordinator.StartGame(mapName);
    //coordinator.StartGame(argv[1]);

    while (coordinator.Update())
    {
        // NOTE (alkurbatov): Slow down game speed for better look & feel
        // while making experiments. Uncomment this if needed.
        // sc2::SleepFor(15);
    }

    return 0;
}
#else

namespace {

struct Options {
    Options(): GamePort(0), StartPort(0), ComputerOpponent(false) {
    }

    int32_t GamePort;
    int32_t StartPort;
    std::string ServerAddress;
    std::string OpponentId;
    bool ComputerOpponent;
    sc2::Difficulty ComputerDifficulty;
    sc2::Race ComputerRace;
};

void ParseArguments(int argc, char* argv[], Options* options_) {
    sc2::ArgParser arg_parser(argv[0]);
    arg_parser.AddOptions({
            {"-g", "--GamePort", "Port of client to connect to", false},
            {"-o", "--StartPort", "Starting server port", false},
            {"-l", "--LadderServer", "Ladder server address", false},
            {"-x", "--OpponentId", "PlayerId of opponent", false},
            {"-c", "--ComputerOpponent", "If we set up a computer opponent", false},
            {"-a", "--ComputerRace", "Race of computer oppent", false},
            {"-d", "--ComputerDifficulty", "Difficulty of computer opponent", false}
        });

    arg_parser.Parse(argc, argv);

    std::string GamePortStr;
    if (arg_parser.Get("GamePort", GamePortStr))
        options_->GamePort = atoi(GamePortStr.c_str());

    std::string StartPortStr;
    if (arg_parser.Get("StartPort", StartPortStr))
        options_->StartPort = atoi(StartPortStr.c_str());

    std::string OpponentId;
    if (arg_parser.Get("OpponentId", OpponentId))
        options_->OpponentId = OpponentId;

    arg_parser.Get("LadderServer", options_->ServerAddress);

    std::string CompOpp;
    if (arg_parser.Get("ComputerOpponent", CompOpp)) {
        options_->ComputerOpponent = true;
        std::string CompRace;

        if (arg_parser.Get("ComputerRace", CompRace))
            options_->ComputerRace = convert::StringToRace(CompRace);

        std::string CompDiff;
        if (arg_parser.Get("ComputerDifficulty", CompDiff))
            options_->ComputerDifficulty = convert::StringToDifficulty(CompDiff);
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    Options options;
    ParseArguments(argc, argv, &options);

    sc2::Coordinator coordinator;
    Dispatcher bot(options.OpponentId);

    size_t num_agents;
    if (options.ComputerOpponent) {
        num_agents = 1;
        coordinator.SetParticipants({
            CreateParticipant(sc2::Race::Random, &bot),
            CreateComputer(options.ComputerRace, options.ComputerDifficulty)
            });
    } else {
        num_agents = 2;
        coordinator.SetParticipants({CreateParticipant(sc2::Race::Terran, &bot)});
    }

    std::cout << "Connecting to port " << options.GamePort << std::endl;
    coordinator.Connect(options.GamePort);
    coordinator.SetupPorts(num_agents, options.StartPort, false);
    coordinator.JoinGame();
    coordinator.SetTimeoutMS(10000);
    std::cout << " Successfully joined game" << std::endl;

    while (coordinator.Update()) {
    }

    return 0;
}

#endif
