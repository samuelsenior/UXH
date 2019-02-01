//
//  main.cpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  Test enviroment for UPPE codes.
//

#include <mpi.h>
#include "../../src/maths_textbook.hpp"
#include "../../src/physics_textbook.hpp"
#include <mkl.h>
#include "../../src/DHT.hpp"
#include "../../src/grid_rkr.hpp"
#include "../../src/grid_tw.hpp"
#include "laser_pulse.hpp"
#include "capillary_fibre.hpp"
#include "../../src/keldysh_gas.hpp"
#include "../../Eigen/Dense"
#include "../../src/IO.hpp"

#include "config_settings.hpp"

#include <iostream>
#include <string>

#include "../../XNLO/lib/XNLO.hpp"
#include "../../HHGP/lib/HHGP.hpp"

#include "../../XNLO/src/config_settings.hpp"
#include "../../XNLO/src/grid_tw.hpp"

#include "../../HHGP/src/propagation.hpp"
//#include "../../HHGP/src/config_settings.hpp"

using namespace Eigen;

/*!
Originally created by Patrick Anderson.
Modified by Samuel Senior on 10/03/2017.
Test enviroment for UPPE codes.
*/
int main(int argc, char** argv){

    // Get config file path passed in from command line with "-cf" flag
    std::string args[argc];
    std::string config_file_path;
    std::string config_XNLO_file_path = "../configFiles/config_XNLO.txt";
    std::string config_HHGP_file_path = "./config_HHGP_test.txt";
    for (int i = 0; i < argc; i++) {
      args[i] = argv[i];
    }
    for (int i = 0; i < argc; i++) {
      if (args[i] == "-cf") {
        config_file_path = argv[i+1];
      }
    }

    // MPI
    int this_process;
    int total_processes;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &this_process);

    if (this_process == 0) {
        std::cout << "-------------------------------------------------------------------------------\n";
        std::cout << "                                  UPPE\n";
        std::cout << "-------------------------------------------------------------------------------\n";
    }

    //--------------------------------------------------------------------------------------------//
    // 1. Program input
    //--------------------------------------------------------------------------------------------//

    // Input Settings and Parameters
    Config_Settings config;
    if(this_process == 0 && config_file_path.empty()) {
      std::cout << "Using default config file path " << config.path_config_file() << std::endl;
    } else {
        config.path_config_file_set(config_file_path);
        config.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
        if (this_process == 0) {
            std::cout << "Using config file path " << config.path_config_file() << std::endl;
        }
    }
    config.read_in(config.path_config_file(), false);
    if (config.read_in_laser_pulse() == 0) {
        config.check_paths(false);
    }
    if (total_processes > 1) {
        config.n_m_set(total_processes-1);
        config.n_r_set(total_processes-1);
    }
    if (this_process == 0) {
        config.print();
    }

    XNLO::Config_Settings config_XNLO;
    if (total_processes > 1) {
        if(this_process == 0 && config_XNLO_file_path.empty()) {
          std::cout << "Using default config file path " << config_XNLO.path_config_file() << std::endl;
        } else {
            config_XNLO.path_config_file_set(config_XNLO_file_path);
            config_XNLO.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
            if (this_process == 0) {
                std::cout << "Using config file path " << config_XNLO.path_config_file() << std::endl;
            }
        }
        config_XNLO.read_in(config_XNLO.path_config_file(), false);
        config_XNLO.check_paths(false);
        if (this_process == 0) {
            config_XNLO.print();
        }
    }

    HH::Config_Settings config_HHGP;
    if (total_processes > 1) {
        if(this_process == 0 && config_HHGP_file_path.empty()) {
          std::cout << "Using default config file path " << config_HHGP.path_config_file() << std::endl;
        } else {
            config_HHGP.path_config_file_set(config_HHGP_file_path);
            config_HHGP.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
            if (this_process == 0) {
                std::cout << "Using config file path " << config_HHGP.path_config_file() << std::endl;
            }
        }
    }
    config_HHGP.read_in(config_HHGP.path_config_file(), false);
    config_HHGP.check_paths(false);
    if (total_processes > 1) {
        config_HHGP.n_m_set(total_processes-1);
        config_HHGP.n_r_set(total_processes-1);
    }
    if (this_process == 0) {
        config_HHGP.print();
    }

    //--------------------------------------------------------------------------------------------//
    // 2. Constructors
    //--------------------------------------------------------------------------------------------//

    // General
    maths_textbook maths(config.path_input_j0());
    physics_textbook physics;

    MKL_LONG dimensions = 1;
    MKL_LONG length = config.n_t();
    double scale = 1.0 / config.n_t();
    DFTI_DESCRIPTOR_HANDLE ft;
    DftiCreateDescriptor(&ft, DFTI_DOUBLE, DFTI_COMPLEX, dimensions, length);
    DftiSetValue(ft, DFTI_BACKWARD_SCALE, scale);
    DftiCommitDescriptor(ft);

    MKL_LONG dimensions_HHG = 1;
    MKL_LONG length_HHG = config_XNLO.N_t();
    double scale_HHG = 1.0 / config_XNLO.N_t();
    DFTI_DESCRIPTOR_HANDLE ft_HHG;
    DftiCreateDescriptor(&ft_HHG, DFTI_DOUBLE, DFTI_COMPLEX, dimensions_HHG, length_HHG);
    DftiSetValue(ft_HHG, DFTI_BACKWARD_SCALE, scale_HHG);
    DftiCommitDescriptor(ft_HHG);

    DHT ht(config.n_r(), maths);
    // Grids
    grid_rkr rkr(config.n_r(), config.R(), config.n_m(), maths);
    grid_tw tw(config.n_t(), config.T(), config.w_active_min(), config.w_active_max(), maths);

    double dz = config.Z() / double(config.n_z());
    int initial_step;
    int propagation_step;
    double initial_position;
    if (config.read_in_laser_pulse() == 1) {
        std::size_t found = config.path_A_w_R().find_last_of("/");
        std::string tmp = config.path_A_w_R().substr(found+1);
        found = tmp.find_first_of("_");

        std::string sim_no_str = tmp.substr(0, found);
        

        std::size_t found_2 = tmp.find_first_of("_", found+1);

        std::string tmp_2 = tmp.substr(found+1, found_2-found-1);
        int sim_no = std::stoi(sim_no_str);

        initial_step = stoi(tmp_2);  
        propagation_step = initial_step + 1;  // The way the main loop works is that it propagates the laser to step i to start with, so the
                                              // initial step being read in is step i-1
        initial_position = dz * initial_step;
std::cout << "Simulation run (read in): " << sim_no << std::endl;
std::cout << "Next propagation step (read in from initial step): " << propagation_step << std::endl;

        //std::string tmp_prop_step_str = 

        //if (found == -1) {
        //path = pre_path + "_" + path;
        //} else {
        //path = path.substr(0, found+1) + pre_path + "_" + path.substr(found+1);
        //}
        //return path;
    } else {
        propagation_step = 1;
    }
    initial_position = dz * propagation_step;

    // Physical
    laser_pulse laser_driving(config.p_av(), config.rep(), config.fwhm(), config.l_0(), config.ceo(), config.waist(),
                              tw, rkr, ft, ht, maths,
                              config,
                              config.read_in_laser_pulse(), initial_position);

std::cout << "laser_driving.A_w_active.real().rows(): " << laser_driving.A_w_active.real().rows() << ", laser_driving.A_w_active.real().cols():" << laser_driving.A_w_active.real().cols() << std::endl;
    capillary_fibre capillary_driving(config.Z(), rkr, tw, physics, maths);
    keldysh_gas gas(config.press(), tw, ft, maths, config.gas_pressure_profile());

    //--------------------------------------------------------------------------------------------//
    // 3. Propagation
    //--------------------------------------------------------------------------------------------//
    // Main loop
    //double dz = capillary_driving.Z / double(config.n_z());

    if (this_process == 0) {
        config.print(config.path_config_log());

        std::cout << "-------------------------------------------------------------------------------\n";
        std::cout << "Main Program:\n";
        std::cout << "-------------------------------------------------------------------------------\n";

        //std::cout << "Laser p_pk: " << laser_driving.p_pk << std::endl;
        //std::cout << "Laser E_pk: " << laser_driving.E_pk << std::endl;
    }
    
    IO file_prop_step;

    ArrayXXd acceleration_HHG = ArrayXXd::Zero(config_XNLO.N_t(), config.n_r());
    ArrayXXd w = ArrayXXd::Zero(config_XNLO.N_t(), config.n_r());
    ArrayXXd E = ArrayXXd::Zero(config_XNLO.N_t(), config.n_r());
    XNLO::Result atomResponse;
    //ArrayXd neutral_atoms = ArrayXd::Zero(config.n_r());
    ArrayXXd neutral_atoms = ArrayXXd::Zero(config.n_t(), config.n_r());
    ArrayXd temp_linSpace_HHG_acceleration = ArrayXd::LinSpaced(config_XNLO.N_t(), -500.0e-15, 500.0e-15);
    ArrayXd window_HHG_acceleration = (1 - ((0.5 * maths.pi * temp_linSpace_HHG_acceleration / 500e-15).sin()).pow(50));

    ArrayXXcd A_w_active;

    ArrayXd w_tmp = ArrayXd::Zero(config_XNLO.N_t());
    XNLO::grid_tw tw_XNLO(config_XNLO.N_t(), config_XNLO.t_min(), config_XNLO.t_max());

    double lamda_min_HHG = 6.0e-9;
    double lamda_max_HHG = 1200e-9;//150e-9;
    double w_active_min_HHG = 2.0 * maths.pi * physics.c / lamda_max_HHG;
    double w_active_max_HHG = 2.0 * maths.pi * physics.c / lamda_min_HHG;
    double E_min = 10.0;
    int n_active_HHG = 0;
    ArrayXd w_active_HHG;

    w_tmp = tw_XNLO.w;

    int w_active_min_index_HHG = 0;
    while (w_tmp(w_active_min_index_HHG) < w_active_min_HHG)
        w_active_min_index_HHG++;
    int count = 0;
    while (w_tmp(count) < w_active_max_HHG) {
        count++;
    }

    n_active_HHG = count - w_active_min_index_HHG;
    w_active_HHG = w_tmp.segment(w_active_min_index_HHG, n_active_HHG);

    propagation prop;
    HHGP hhgp;
    if (this_process == 0) {
        prop = propagation(E_min, w_active_HHG,
                           gas, rkr,
                           physics, maths, ht);
        w_active_HHG = prop.segment(w_active_HHG);
        hhgp = HHGP(prop,
                    config_HHGP,
                    rkr, gas,
                    maths, ht);
    }

    ArrayXXcd hhg;
    ArrayXXcd hhg_new;
    ArrayXXcd hhg_source;
    ArrayXXcd hhg_previous;

    MPI_Barrier(MPI_COMM_WORLD);

        for (int ii = propagation_step; ii < config.n_z() + 1; ii++) {
            if (this_process == 0) {
                std::cout << "Propagation step: " << ii << std::endl;
                laser_driving.propagate(dz, capillary_driving, gas);

                // Driving pulse:
                config.step_path(ii);
                file_prop_step.write(laser_driving.A_w_active.real(), config.path_A_w_R(), false);
                file_prop_step.write(laser_driving.A_w_active.imag(), config.path_A_w_I(), false);
                file_prop_step.write(tw.w_active, config.path_w_active(), false);
                file_prop_step.write(laser_driving.electron_density, config.path_electron_density(), false);

                A_w_active = laser_driving.A_w_active;

                if (total_processes > 1) {
                    // Send
                    for (int j = 1; j < total_processes; j++) {
                        MPI_Send(laser_driving.A_w_active.real().data(),
                                 laser_driving.A_w_active.cols() * laser_driving.A_w_active.rows(),
                                 MPI_DOUBLE, j, j, MPI_COMM_WORLD);
                    }
                }
            } else {
                // Receive
                A_w_active = ArrayXXd::Zero(laser_driving.A_w_active.cols(), laser_driving.A_w_active.rows());
                MPI_Recv(A_w_active.real().data(), laser_driving.A_w_active.cols() * laser_driving.A_w_active.rows(),
                         MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD, &status);
            }

            int response_rate = 1;//config.n_z() / 10;
            if (total_processes > 1 && ((ii % response_rate == 0) || ii == 1)) {
                atomResponse = XNLO::XNLO(A_w_active, tw.w_active);
            }

            if (this_process == 0 && total_processes > 1) {
                // Do we just take the electron density at the last time step or at all of them?
                for (int j = 0; j < rkr.n_r; j++) {
                    for (int i = 0; i < config.n_t(); i++) {
                        neutral_atoms.row(i).col(j) = (gas.atom_density(double(ii)*dz) - laser_driving.electron_density.row(i).col(j));
                    }
                }

                E = atomResponse.E;
                // Delete atomResponse after use to save ram
                acceleration_HHG = atomResponse.acceleration;
                for (int j = 0; j < rkr.n_r; j++) {
                    for (int i = 0; i < config_XNLO.N_t(); i++) {
                        // Or is it (0, 0)?
                        // So, physically, what is going on?
                        // The laser pulse at each propagation step interacts with the gas.
                        // Some atoms become ionised by the laser, these atoms do not add to the
                        // HHG response
                        // The neutral atoms do add to the response
                        // The acceleration at this point is as a function of time, as is the neutral atoms
                        // these are both on different, independent time grid though, as...?
                        // So, we only want the number of neutral atoms at one point in time, but which point?
                        // At the start time there are a maximum number of neutral atoms
                        // At the end time there are a minimum
                        // So, laser comes through, ionises gas, causes HHG response
                        // if gas is ionised it can't add to HHG response
                        // as the atom has no electron to it
                        // So it needs to be the final number of neutral atoms?
                        // If so then can save a load of ram by only storing this value and not for all time steps
// --- TAKE NOTE --- // Change back once testing done!
                        //acceleration_HHG.row(i).col(j) *= dz;  // Should be done in the HHG prop program rather than here
                                                                 // to keep the sources terms as unmodified source terms
                                                                 // for now.
                        //acceleration_HHG.row(i).col(j) *= neutral_atoms.row(neutral_atoms.rows() - 1).col(j) * dz;
                        acceleration_HHG.row(i).col(j) *= window_HHG_acceleration.row(i);// / (w.row(i)).pow(2);

                        // How to do volume normalisation? Step increases to integrate over, or trapezoidal rule
                        // First is easier to implement, second is more accurate
                    }
                }
                // Apply forward spectral transform
                ArrayXXcd accelerationToHHSource = acceleration_HHG.cast<std::complex<double> >();
                for (int i = 0; i < rkr.n_r; i++)
                    DftiComputeForward(ft_HHG, accelerationToHHSource.col(i).data());

                hhg = prop.block(accelerationToHHSource.block(0, 0, n_active_HHG, rkr.n_r));
                for (int j = 0; j < rkr.n_r; j++) {
                    for (int i = 0; i < prop.n_k; i++) {//n_active_HHG; i++) {
                        hhg.row(i).col(j) /= (w_active_HHG.row(i)).pow(2);
                    }
                }

                if (ii == 1) {
                    hhg_previous = hhg;
                } else {
                    double z = dz * double(ii);
                    hhg_source = hhg;
                    //hhg_new = hhgp.nearFieldStep(hhg_source, hhg_previous,
                    //                             w_active_HHG,
                    //                             z, dz);
                    //hhg_previous = hhg_new;
                    //hhg = hhg_new;
                }
                // Explaination of the above:
                // -At the first step we just want the source term as nothing from any previous steps is
                //  propagated to the first position since nothing before
                // -Keep the hhg term here as the sources from the previous step
                // -At the next step, take the HHG as the current source term
                //  and use previous as the sources from the previous steps
                //  propagate previous to current and add source to it
                // -This is now also the previous term for the next step
                // -Repeat

                file_prop_step.write(hhg.real(), config.path_HHG_R(), false);
                file_prop_step.write(hhg.imag(), config.path_HHG_I(), false);
                file_prop_step.write(w_active_HHG, config.path_HHG_w(), false);
                file_prop_step.write(E, config.path_HHG_E(), false);

            }
        }
        std::cout << "-------------------------------------------------------------------------------\n";

        if (this_process == 0) {
            // Output
            IO file;
            file.write(laser_driving.A_w_active.real(), config.path_A_w_R());
            file.write(laser_driving.A_w_active.imag(), config.path_A_w_I());
            file.write(tw.w_active, config.path_w_active());
        }

        // Clean up
        DftiFreeDescriptor(&ft);
        MPI_Finalize();

        std::cout << "\n-------------------------------------------------------------------------------\n";
        std::cout << "UPPE successfully ran!\n";
        std::cout << "-------------------------------------------------------------------------------\n";
}
