//
//  main.cpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  Test enviroment for UPPE codes.
//

#include <string>
#include "UPPE_simulation.hpp"

/*!
Originally created by Patrick Anderson.
Modified by Samuel Senior on 10/03/2017.
Test enviroment for UPPE codes.
*/
int main(int argc, char** argv){

    // Get config file path passed in from command line with "-cf" flag
    std::string args[argc];
    std::string config_UPPE_file_path;
    std::string config_XNLO_file_path = "../configFiles/config_XNLO.txt";
    std::string config_HHGP_file_path = "../configFiles/config_HHGP.txt";
    for (int i = 0; i < argc; i++) {
        args[i] = argv[i];
    }
    for (int i = 0; i < argc; i++) {
        if (args[i] == "-cf") {
            config_UPPE_file_path = argv[i+1];
        }
    }

    UPPE::UPPE_simulation UPPE_sim(argc, argv);
    UPPE_sim.initialise_UPPE_simulation(config_UPPE_file_path, config_XNLO_file_path, config_HHGP_file_path);
    UPPE_sim.run_UPPE_simulation();
}

