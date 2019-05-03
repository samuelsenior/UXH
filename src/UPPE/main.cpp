//
//  main.cpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  Test enviroment for UPPE codes.
//

#include <mpi.h>
#include <mkl.h>
#include <iostream>
#include <string>

#include "../../Eigen/Dense"

#include "config_settings.hpp"

#include "../capillary/capillary_fibre.hpp"
#include "../DHT/DHT.hpp"
#include "../gas/keldysh_gas.hpp"
#include "../grid/grid_rkr.hpp"
#include "../grid/grid_tw.hpp"
#include "../IO/IO.hpp"
#include "../laser_pulse/laser_pulse.hpp"
#include "../maths/maths_textbook.hpp"
#include "../physics/physics_textbook.hpp"

#include "../XNLO/XNLO.hpp"
#include "../XNLO/config_settings.hpp"

#include "../HHGP/HHGP.hpp"
#include "../HHGP/propagation.hpp"

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
    std::string config_HHGP_file_path = "../configFiles/config_HHGP.txt";
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
    if(config_file_path.empty()) {
      if (this_process == 0) { std::cout << "Using default config file path " << config.path_config_file() << std::endl; }
    } else {
        config.path_config_file_set(config_file_path);
        config.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
        if (this_process == 0) { std::cout << "Using config file path " << config.path_config_file() << std::endl; }
    }
    config.read_in(config.path_config_file(), false);
    if (config.read_in_laser_pulse() == 0) { config.check_paths(false); }
    if (total_processes > 1) {
        config.n_m_set(total_processes-1);
        config.n_r_set(total_processes-1);
    }


    //if (this_process == 0) { config.print(); }

    XNLO::Config_Settings config_XNLO;
    config_XNLO.print_to_screen = false;
    if (total_processes > 1) {
        if(config_XNLO_file_path.empty()) {
          if(this_process == 0) { std::cout << "Using default config file path " << config_XNLO.path_config_file() << std::endl; }
        } else {
            config_XNLO.path_config_file_set(config_XNLO_file_path);
            config_XNLO.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
            if (this_process == 0) { std::cout << "Using config file path " << config_XNLO.path_config_file() << std::endl; }
        }
        config_XNLO.read_in(config_XNLO.path_config_file(), false);
        config_XNLO.check_paths(false);
        //if (this_process == 0) { config_XNLO.print(); }
    }

    HH::Config_Settings config_HHGP;
    if (total_processes > 1) {
        if(config_HHGP_file_path.empty()) {
          if(this_process == 0) { std::cout << "Using default config file path " << config_HHGP.path_config_file() << std::endl; }
        } else {
            config_HHGP.path_config_file_set(config_HHGP_file_path);
            config_HHGP.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
            if (this_process == 0) { std::cout << "Using config file path " << config_HHGP.path_config_file() << std::endl; }
        }
        config_HHGP.read_in(config_HHGP.path_config_file(), false);
        config_HHGP.check_paths(false);
        //if (total_processes > 1) {
        //    config_HHGP.n_m_set(total_processes-1);
        //    config_HHGP.n_r_set(total_processes-1);
        //}
        //if (this_process == 0) { config_HHGP.print(); }
    }

maths_textbook maths(config.path_input_j0());
grid_rkr rkr(config.n_r(), config.R(), config.n_m(), maths);

if (total_processes > 1) {
    config_XNLO.x_min_set(rkr.r(0));
    config_XNLO.x_min_description_set("(double) Minimum radial posiiton, set through UPPE");

    config_XNLO.x_max_set(config.R());
    config_XNLO.x_max_description_set("(double) Maximum radial posiiton, set through UPPE");

    config_XNLO.P_av_set(config.p_av());
    config_XNLO.P_av_description_set("(double) Average laser power, set through UPPE");

    config_XNLO.RR_set(config.rep());
    config_XNLO.RR_description_set("(double) Laser repetition, set through UPPE");

    config_XNLO.FWHM_set(config.fwhm());
    config_XNLO.FWHM_description_set("(double) Laser full width half maximum, set through UPPE");

    config_XNLO.l_0_set(config.l_0());
    config_XNLO.l_0_description_set("(double) Laser wavelength, set through UPPE");

    config_XNLO.CEO_set(config.ceo());
    config_XNLO.CEO_description_set("(double) Ceo value, set through UPPE");

    config_XNLO.spot_radius_set(config.waist());
    config_XNLO.spot_radius_description_set("(double) Laser spot radius, set through UPPE");


    config.n_m_set(config_XNLO.atoms_per_worker()*(total_processes-1));
    config.n_m_description_set("(int) Number of modes, set through UPPE");
    config.n_r_set(config_XNLO.atoms_per_worker()*(total_processes-1));
    config.n_r_description_set("(int) Number of radial points, set through UPPE");

    config_HHGP.n_m_set(config_XNLO.atoms_per_worker()*(total_processes-1));
    config_HHGP.n_m_description_set("(int) Number of modes, set through UPPE");
    config_HHGP.n_r_set(config_XNLO.atoms_per_worker()*(total_processes-1));
    config_HHGP.n_r_description_set("(int) Number of radial points, set through UPPE");

    config_HHGP.n_z_set(config.n_z());
    config_HHGP.n_z_description_set("(int) Number of steps in Z, set through UPPE");

    config_HHGP.T_set(config.T());
    config_HHGP.T_description_set("(double) The T value, set through UPPE");

    config_HHGP.w_active_min_set(config.w_active_min());
    config_HHGP.w_active_min_description_set("(double) Minimum angular frequency, set through UPPE");

    config_HHGP.w_active_max_set(config.w_active_max());
    config_HHGP.w_active_max_description_set("(double) Maximum angular frequency, set through UPPE");

    config_HHGP.Z_set(config.Z());
    config_HHGP.Z_description_set("(double) Length of capillary, set through UPPE");

    config_HHGP.R_set(config.R());
    config_HHGP.R_description_set("(double) Radius of capillary, set through UPPE");

    config_HHGP.press_set(config.press());
    config_HHGP.press_description_set("(double) Pressure fo the gas, set through UPPE");

    config_HHGP.p_av_set(config.p_av());
    config_HHGP.p_av_description_set("(double) Average laser power, set through UPPE");

    config_HHGP.rep_set(config.rep());
    config_HHGP.rep_description_set("(double) Laser repetition, set through UPPE");

    config_HHGP.fwhm_set(config.fwhm());
    config_HHGP.fwhm_description_set("(double) Laser full width half maximum, set through UPPE");

    config_HHGP.l_0_set(config.l_0());
    config_HHGP.l_0_description_set("(double) Laser wavelength, set through UPPE");

    config_HHGP.ceo_set(config.ceo());
    config_HHGP.ceo_description_set("(double) Ceo value, set through UPPE");

    config_HHGP.waist_set(config.waist());
    config_HHGP.waist_description_set("(double) Laser spot radius, set through UPPE");
    
    config_HHGP.gas_pressure_profile_set(config.gas_pressure_profile());
    config_HHGP.gas_pressure_profile_description_set("(std::string) LSwitch for gas pressure profile, set through UPPE");
}

if (this_process == 0) { config.print(); }
if (total_processes > 1) {
    if (this_process == 0) { config_XNLO.print(); }
    if (this_process == 0) { config_HHGP.print(); }
}

    //HH::Config_Settings config_HHGP;
    //if (total_processes > 1) {
    //    if(config_HHGP_file_path.empty()) {
    //      if(this_process == 0) { std::cout << "Using default config file path " << config_HHGP.path_config_file() << std::endl; }
    //    } else {
    //        config_HHGP.path_config_file_set(config_HHGP_file_path);
    //        config_HHGP.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
    //        if (this_process == 0) { std::cout << "Using config file path " << config_HHGP.path_config_file() << std::endl; }
    //    }
    //    config_HHGP.read_in(config_HHGP.path_config_file(), false);
    //    config_HHGP.check_paths(false);
    //    //if (total_processes > 1) {
    //    //    config_HHGP.n_m_set(total_processes-1);
    //    //    config_HHGP.n_r_set(total_processes-1);
    //    //}
    //    if (this_process == 0) { config_HHGP.print(); }
    //}

    //--------------------------------------------------------------------------------------------//
    // 2. Constructors
    //--------------------------------------------------------------------------------------------//

    // General
    //maths_textbook maths(config.path_input_j0());
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
    //grid_rkr rkr(config.n_r(), config.R(), config.n_m(), maths);
    grid_tw tw(config.n_t(), config.T(), config.w_active_min(), config.w_active_max(), maths);
if (this_process == 0) std::cout << "w_active_min_index: " << tw.w_active_min_index << std::endl;

    double dz = config.Z() / double(config.n_z());
    int initial_step = 0;
    int propagation_step;
    double initial_position;
    if (config.read_in_laser_pulse() == 1) {
        std::size_t found = config.path_A_w_R_initial().find_last_of("/");
        std::string tmp = config.path_A_w_R_initial().substr(found+1);
        found = tmp.find_first_of("_");
        std::string sim_no_str = tmp.substr(0, found);
        
        std::size_t found_2 = tmp.find_first_of("_", found+1);
        std::string tmp_2 = tmp.substr(found+1, found_2-found-1);
        int sim_no = std::stoi(sim_no_str);

        initial_step = stoi(tmp_2);
        initial_step = int(double(config.n_z()) * double(stoi(tmp_2)) / double(config.original_n_z()));

        propagation_step = initial_step;// + 1;  // The way the main loop works is that it propagates the laser to step i to start with, so the
                                              // initial step being read in is step i-1
        initial_position = dz * initial_step;
        if (this_process == 0) {
            std::cout << "Simulation run (read in): " << sim_no << std::endl;
            std::cout << "Initial step: " << initial_step << std::endl;
            std::cout << "Next propagation step (read in from initial step): " << propagation_step << std::endl;
            std::cout << "Original position: " << (double(stoi(tmp_2)) / double(config.original_n_z())) * config.Z() << std::endl;
            std::cout << "New position: " << double(initial_step) / double(config.n_z()) * config.Z() << std::endl;
        }
    } else {
        propagation_step = 1;
    }
    initial_position = dz * propagation_step;

    // Physical
    UPPE::laser_pulse laser_driving(config.p_av(), config.rep(), config.fwhm(), config.l_0(), config.ceo(), config.waist(),
                              tw, rkr, ft, ht, maths,
                              config,
                              config.read_in_laser_pulse(), initial_position,
                              config.laser_rel_tol());
    capillary_fibre capillary_driving(config.Z(), rkr, tw, physics, maths);
    keldysh_gas gas(config.press(), tw, ft, maths, config.gas_pressure_profile());

    //--------------------------------------------------------------------------------------------//
    // 3. Propagation
    //--------------------------------------------------------------------------------------------//
    // Main loop

    if (this_process == 0) {
        config.print(config.path_config_log());

        std::cout << "-------------------------------------------------------------------------------\n";
        std::cout << "Main Program:\n";
        std::cout << "-------------------------------------------------------------------------------\n";

        std::cout << "Laser p_pk: " << laser_driving.p_pk << ", laser E_pk: " << laser_driving.E_pk << std::endl;
    }
    
    IO file_prop_step;

    ArrayXXd acceleration_HHG = ArrayXXd::Zero(config_XNLO.N_t(), config.n_r());
    ArrayXXd E;
    if (config_XNLO.output_electric_field() == 1) {
        ArrayXXd E = ArrayXXd::Zero(config_XNLO.N_t(), config.n_r());
    } else {
        ArrayXXd E = ArrayXXd::Zero(0, 0);
    }
    //XNLO::Result atomResponse;
    ArrayXd neutral_atoms = ArrayXd::Zero(config.n_r());
    //ArrayXXd neutral_atoms = ArrayXXd::Zero(config.n_t(), config.n_r());
    ArrayXd temp_linSpace_HHG_acceleration = ArrayXd::LinSpaced(config_XNLO.N_t(), -500.0e-15, 500.0e-15);  // I think this is wring and should be using T_min/T_max of XNLO???
    ArrayXd window_HHG_acceleration = (1 - ((0.5 * maths.pi * temp_linSpace_HHG_acceleration / 500e-15).sin()).pow(50));
    temp_linSpace_HHG_acceleration = 0;

    ArrayXXcd A_w_active;

    XNLO::grid_tw tw_XNLO(config_XNLO.N_t(), config_XNLO.t_min(), config_XNLO.t_max());

    XNLO_AtomResponse atomResponse;
    if (total_processes > 1) {
        atomResponse = XNLO_AtomResponse(&rkr, &tw_XNLO, &maths, &physics,
                                         this_process, total_processes,
                                         config_XNLO,
                                         "minimum");
    }
    MPI_Barrier(MPI_COMM_WORLD); 

    double lamda_min_HHG = 6.0e-9;
    double lamda_max_HHG = 130e-9;
    double w_active_min_HHG = 2.0 * maths.pi * physics.c / lamda_max_HHG;
    double w_active_max_HHG = 2.0 * maths.pi * physics.c / lamda_min_HHG;
    double E_min = 10.0;
    double E_max = 206.0;
    int n_active_HHG = 0;
    ArrayXd w_active_HHG;
 
    ArrayXd w_tmp = tw_XNLO.w;
    //if (this_process == 0) {
    //    std::cout << "HHG w_tmp(0): " << w_tmp(0) << ", HHG w_tmp(" << w_tmp.rows() - 1 << "): " << w_tmp(w_tmp.rows() - 1) << std::endl;
    //}
    int w_active_min_index_HHG = 0;
    while (w_tmp(w_active_min_index_HHG) < w_active_min_HHG)
        w_active_min_index_HHG++;
    int count = 0;
    while (w_tmp(count) < w_active_max_HHG) {
        count++;
    }
    n_active_HHG = count - w_active_min_index_HHG;
    w_active_HHG = w_tmp.segment(w_active_min_index_HHG, n_active_HHG);
    w_tmp = 0;

    //if (this_process == 0) {
    //    std::cout << "n_active_HHG: " << n_active_HHG << std::endl;
    //    std::cout << "HHG w_active_HHG(0): " << w_active_HHG(0) << ", HHG w_active_HHG(" << w_active_HHG.rows() - 1 << "): " << w_active_HHG(w_active_HHG.rows() - 1) << std::endl;
    //}
    propagation prop;
    HHGP hhgp;
prop = propagation(E_min, E_max, config.Z(), w_active_HHG,
                           gas, rkr,
                           physics, maths, ht);
prop.print = false;
    if (this_process == 0) {
//        prop = propagation(E_min, E_max, config.Z(), w_active_HHG,
//                           gas, rkr,
//                           physics, maths, ht);
        prop.print = true;
        hhgp = HHGP(prop,
                    config_HHGP,
                    rkr, gas,
                    maths, ht);
    }
    bool HH_prop_to_end_only = true;
    //double dz = config.Z() / config.n_z();
    //prop.z = double(config.inital_propagation_step()) / double(config.n_z()) * double(config.Z());;
    prop.z = double(initial_step) / double(config.n_z()) * double(config.Z());
    ////prop.z -= dz;
    //int inital_propagation_step = config.inital_propagation_step();
    double HHGP_starting_z = config.HHGP_starting_z();

    ArrayXXcd hhg;
    ArrayXXcd hhg_new = ArrayXXcd::Zero(prop.n_k, config.n_r());
    ArrayXXcd hhg_source;
    ArrayXXcd hhg_previous;

    ArrayXXcd HHG_tmp;// = ArrayXXcd::Zero(w_active_HHG.rows(), config.n_r());
    ArrayXXcd HHP = ArrayXXcd::Zero(prop.n_k, config.n_r());
    ArrayXXcd HHP_multiThread_tmp = ArrayXXcd::Zero(prop.n_k, config.n_r());

    ArrayXXcd hhg_old = ArrayXXcd::Zero(prop.n_k, config.n_r());//;// = ArrayXXcd::Zero(w_active_HHG.rows(), config.n_r());
    ArrayXXcd hhg_old_old = ArrayXXcd::Zero(prop.n_k, config.n_r());
    ArrayXXcd hhg_old_interp = ArrayXXcd::Zero(prop.n_k, config.n_r());
    ArrayXXcd dS_i = ArrayXXcd::Zero(prop.n_k, config.n_r());
    ArrayXXcd hhg_i = ArrayXXcd::Zero(prop.n_k, config.n_r());



    int wavelength_index_min;
    int wavelength_index_max;
    if (this_process == 0) {
        wavelength_index_min = 0;
        while(2.0 * maths.pi / prop.k(wavelength_index_min).real() *1e9 > 33.0) {
            wavelength_index_min++;
        }
        wavelength_index_max = 0;
        while(2.0 * maths.pi / prop.k(wavelength_index_max).real() *1e9 > 26.0) {
            wavelength_index_max++;
        }
        //std::cout << "Hwavelength_index_min: " << wavelength_index_min << ", wavelength_index_max: " << wavelength_index_max << std::endl;
    }



    if (this_process == 0) {
            // Output already known variables, in case crashes etc, so they are already saved early on
            IO file;
            file.write(tw.w_active, config.path_w_active());
            file.write(w_active_HHG, config.path_HHG_w());
    }

    MPI_Barrier(MPI_COMM_WORLD);

    bool HHGP_starting_z_bool = false;

        int ii;
        if (config.read_in_laser_pulse() == true) {
            ii = propagation_step;
        } else {
            ii = 0;
            propagation_step = 0;
        }
        if (dz*ii >= HHGP_starting_z) HHGP_starting_z_bool = true;
        if (this_process == 0) {
            std::cout << "Propagation step: " << ii << std::endl;
            // Driving pulse:
            // Always start at 1, so first step is always outputted, even when reading in
            if ((ii - propagation_step) % config.output_sampling_rate() == 0) {
                config.step_path(ii, "UPPE_A_w");
                file_prop_step.write(laser_driving.A_w_active.real(), config.path_A_w_R_step(), true);
                file_prop_step.write(laser_driving.A_w_active.imag(), config.path_A_w_I_step(), false);
            }
            // Only needed once!
            //file_prop_step.write(tw.w_active, config.path_w_active_step(), true);
            // Change to an if statement so can be outputted if needed
            //file_prop_step.write(laser_driving.electron_density, config.path_electron_density_step(), true);
            A_w_active = laser_driving.A_w_active;
            if ((total_processes > 1) && HHGP_starting_z_bool) {
                // Send
                for (int j = 1; j < total_processes; j++) {
                    MPI_Send(laser_driving.A_w_active.real().data(),
                             laser_driving.A_w_active.cols() * laser_driving.A_w_active.rows(),
                             MPI_DOUBLE, j, j, MPI_COMM_WORLD);
                }
            }
        } else if (HHGP_starting_z_bool) {
            // Receive
            A_w_active = ArrayXXd::Zero(laser_driving.A_w_active.cols(), laser_driving.A_w_active.rows());
            MPI_Recv(A_w_active.real().data(), laser_driving.A_w_active.cols() * laser_driving.A_w_active.rows(),
                     MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD, &status);
        }

        if ((total_processes > 1) && HHGP_starting_z_bool) {
            //atomResponse = XNLO::XNLO(A_w_active, tw.w_active, tw.w_active_min_index,
            //                          rkr, tw_XNLO,
            //                          maths, physics, "minimum");
            atomResponse.run(A_w_active, tw.w_active, tw.w_active_min_index);
        }

        if (this_process == 0 && total_processes > 1 && HHGP_starting_z_bool) {
            // Do we just take the electron density at the last time step or at all of them?
            for (int j = 0; j < rkr.n_r; j++) {
                //for (int i = 0; i < config.n_t(); i++) {
                //    //neutral_atoms.row(i).col(j) = (gas.atom_density(double(ii)*dz) - laser_driving.electron_density.row(i).col(j));
                    neutral_atoms(j) = gas.atom_density(double(ii)*dz) - laser_driving.electron_density(laser_driving.electron_density.rows() - 1, j);;//.row(laser_driving.electron_density.rows() - 1).col(j);
                //}
            }
            if (config_XNLO.output_electric_field() == 1) {
                E = atomResponse.E;
            }
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
                    //acceleration_HHG.row(i).col(j) *= neutral_atoms.row(neutral_atoms.rows() - 1).col(j);// * dz;
                    acceleration_HHG.row(i).col(j) *= neutral_atoms(j);
                    acceleration_HHG.row(i).col(j) *= window_HHG_acceleration.row(i);// / (w.row(i)).pow(2);

                    // How to do volume normalisation? Step increases to integrate over, or trapezoidal rule
                    // First is easier to implement, second is more accurate
                }
            }
            // Apply forward spectral transform
            ArrayXXcd accelerationToHHSource = acceleration_HHG.cast<std::complex<double> >();
            for (int i = 0; i < rkr.n_r; i++)
                DftiComputeForward(ft_HHG, accelerationToHHSource.col(i).data());
            hhg = accelerationToHHSource.block(w_active_min_index_HHG, 0, n_active_HHG, rkr.n_r);
            for (int j = 0; j < rkr.n_r; j++) {
                for (int i = 0; i < n_active_HHG; i++) {
                    hhg.row(i).col(j) /= (w_active_HHG.row(i)).pow(2);
                }
            }
            // Propagate high harmonics from current step to end of capillary
            //if (HH_prop_to_end_only == true){
            //HHG_tmp = prop.block(hhg) * dz;  // Normalisation to a dz volume
            HHG_tmp = prop.block(hhg) * (dz / double(config.interp_points() + 1));  // Normalisation to a dz volume
            // If at the last step then we're at teh end of the capillary and so aren't looking
            // to propagate the last HH source any further, but rather just use it's source as it's
            // already at the desired position
            if (ii < config.n_z()) {
                prop.nearFieldPropagationStep((config.Z() - dz*ii), HHG_tmp);
                HHP += prop.A_w_r;
            } else {
                HHP += HHG_tmp;
            }

            if ((ii - propagation_step) % config.output_sampling_rate() == 0) {
                config.step_path(ii, "HHG_A_w");
                file_prop_step.write(hhg.real(), config.path_HHG_R_step(), true);
                file_prop_step.write(hhg.imag(), config.path_HHG_I_step(), false);

                config.step_path(ii, "HHP_A_w");
                file_prop_step.write(HHP.real(), config.path_HHP_R_step(), true);
                file_prop_step.write(HHP.imag(), config.path_HHP_I_step(), false);
            }
            // Only needed once!
            //file_prop_step.write(w_active_HHG, config.path_HHG_w_step(), true);
            if (config_XNLO.output_electric_field() == 1) {
                config.step_path(ii, "HHG_electric_field");
                file_prop_step.write(E, config.path_HHG_E_step(), true);
            }

            //if (ii == 1) {
                hhg_old = prop.block(hhg) * (dz / double(config.interp_points() + 1));  // Normalisation to a dz volume
            //} else {
            //    std::cout << "Starting interpolation!" << std::endl;
            //    hhg_new = prop.block(hhg) * (dz / double(config.interp_points() + 1));  // Normalisation to a dz volume
            //    double interp_dz = dz / double(config.interp_points() + 2);
            //    dS_i = (hhg_new - hhg_old) / double(config.interp_points() + 2);
            //    for (int interp_i = 1; interp_i < config.interp_points() + 2; interp_i++) {
            //        prop.z += interp_dz;
            //        hhg_i = hhg_old + interp_i * dS_i;
            //
            //        prop.nearFieldPropagationStep((config.Z() - dz*ii)-(interp_i * interp_dz), hhg_i);
            //        HHP += prop.A_w_r;
            //   } 
            //    hhg_old = hhg_new;
            //    std::cout << "Interpolation complete!" << std::endl;
            //    if ((ii - propagation_step) % config.output_sampling_rate() == 0) {
            //        config.step_path(ii, "HHP_A_w");
            //        file_prop_step.write(HHP.real(), config.path_HHP_R_step(), true);
            //        file_prop_step.write(HHP.imag(), config.path_HHP_I_step(), false);
            //    }
            //}
        }
        propagation_step++;

    HHGP_starting_z_bool = false;

        for (int ii = propagation_step; ii < config.ending_n_z() + 1; ii++) {//config.n_z() + 1; ii++) {
            if (dz*ii >= HHGP_starting_z) HHGP_starting_z_bool = true;
            if (this_process == 0) {
                std::cout << "Propagation step: " << ii << std::endl;
                laser_driving.propagate(dz, capillary_driving, gas);
                prop.z += dz;
                // Driving pulse:
                // Always start at 1, so first step is always outputted, even when reading in
                if ((ii - initial_step) % config.output_sampling_rate() == 0) {
                    config.step_path(ii, "UPPE_A_w");
                    file_prop_step.write(laser_driving.A_w_active.real(), config.path_A_w_R_step(), true);
                    file_prop_step.write(laser_driving.A_w_active.imag(), config.path_A_w_I_step(), false);
                }
                // Only needed once!
                //file_prop_step.write(tw.w_active, config.path_w_active_step(), true);
                // Change to an if statement so can be outputted if needed
                //file_prop_step.write(laser_driving.electron_density, config.path_electron_density_step(), true);
                A_w_active = laser_driving.A_w_active;
                if ((total_processes > 1) && HHGP_starting_z_bool) {
                    // Send
                    for (int j = 1; j < total_processes; j++) {
                        MPI_Send(laser_driving.A_w_active.real().data(),
                                 laser_driving.A_w_active.cols() * laser_driving.A_w_active.rows(),
                                 MPI_DOUBLE, j, j, MPI_COMM_WORLD);
                    }
                }
            } else if (HHGP_starting_z_bool) {
                // Receive
                A_w_active = ArrayXXd::Zero(laser_driving.A_w_active.cols(), laser_driving.A_w_active.rows());
                MPI_Recv(A_w_active.real().data(), laser_driving.A_w_active.cols() * laser_driving.A_w_active.rows(),
                         MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD, &status);
            }

            //int response_rate = 1;//config.n_z() / 10;
            if ((total_processes > 1) && HHGP_starting_z_bool) {// && ((ii % response_rate == 0) || ii == 1)) {
                //atomResponse = XNLO::XNLO(A_w_active, tw.w_active, tw.w_active_min_index,
                //                          rkr, tw_XNLO,
                //                          maths, physics, "minimum");
                atomResponse.run(A_w_active, tw.w_active, tw.w_active_min_index);
            }

            if (this_process == 0 && total_processes > 1 && HHGP_starting_z_bool) {
                // Do we just take the electron density at the last time step or at all of them?
                for (int j = 0; j < rkr.n_r; j++) {
                //for (int i = 0; i < config.n_t(); i++) {
                //    //neutral_atoms.row(i).col(j) = (gas.atom_density(double(ii)*dz) - laser_driving.electron_density.row(i).col(j));
                    neutral_atoms(j) = gas.atom_density(double(ii)*dz) - laser_driving.electron_density(laser_driving.electron_density.rows() - 1, j);;//.row(laser_driving.electron_density.rows() - 1).col(j);
                //}
                }
                if (config_XNLO.output_electric_field() == 1) {
                    E = atomResponse.E;
                }
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
                        //acceleration_HHG.row(i).col(j) *= neutral_atoms.row(neutral_atoms.rows() - 1).col(j);// * dz;
                        acceleration_HHG.row(i).col(j) *= neutral_atoms(j);
                        acceleration_HHG.row(i).col(j) *= window_HHG_acceleration.row(i);// / (w.row(i)).pow(2);

                        // How to do volume normalisation? Step increases to integrate over, or trapezoidal rule
                        // First is easier to implement, second is more accurate
                    }
                }
                // Apply forward spectral transform
                ArrayXXcd accelerationToHHSource = acceleration_HHG.cast<std::complex<double> >();
                for (int i = 0; i < rkr.n_r; i++)
                    DftiComputeForward(ft_HHG, accelerationToHHSource.col(i).data());
// Uncomment the below when the debugging and testing longer wavelegnths is done
//                hhg = prop.block(accelerationToHHSource.block(0, 0, n_active_HHG, rkr.n_r));
                //hhg = prop.block(accelerationToHHSource.block(w_active_min_index_HHG, 0, n_active_HHG, rkr.n_r));
                hhg = accelerationToHHSource.block(w_active_min_index_HHG, 0, n_active_HHG, rkr.n_r);
                //hhg = accelerationToHHSource;
                for (int j = 0; j < rkr.n_r; j++) {
                    for (int i = 0; i < n_active_HHG/*prop.n_k*/; i++) {//n_active_HHG; i++) {
                        hhg.row(i).col(j) /= (w_active_HHG.row(i)).pow(2);
                    }
                }
                // Propagate high harmonics from current step to end of capillary
                //if (HH_prop_to_end_only == true){
                //HHG_tmp = prop.block(hhg) * dz;  // Normalisation to a dz volume
                HHG_tmp = prop.block(hhg) * (dz / double(config.interp_points() + 1));  // Normalisation to a dz volume
                // If at the last step then we're at the end of the capillary and so aren't looking
                // to propagate the last HH source any further, but rather just use it's source as it's
                // already at the desired position
                if (ii < config.n_z()) {
                    prop.nearFieldPropagationStep((config.Z() - dz*ii), HHG_tmp);
                    HHP += prop.A_w_r;
                } else {
                    HHP += HHG_tmp;
                }
                //} else {
                //    if (ii == inital_propagation_step) {
                //        // NOT SURE
                //        prop.z += dz;///2.0;
                //        // Get source at step, set current and previous values to it
                //        HHG_tmp = prop.block(hhg) * dz;  // Normalisation to a dz volume
                //        HHP = HHG_tmp;
                //    } else if (ii > inital_propagation_step) {
                //        prop.z += dz;
                //        // Propagate source at previous step to the current step and store the previous step as it
                //        prop.nearFieldPropagationStep(dz, HHP);
                //        HHP = prop.A_w_r;
                //        // Get the new source at the current step and add the propagted source from the previous step to this
                //        HHG_tmp = prop.block(hhg) * dz;  // Normalisation to a dz volume
                //        HHP += A_w_r;//prop.A_w_r;
                //    }
                //}
                if ((ii - initial_step) % config.output_sampling_rate() == 0) {
                    config.step_path(ii, "HHG_A_w");
                    file_prop_step.write(hhg.real(), config.path_HHG_R_step(), true);
                    file_prop_step.write(hhg.imag(), config.path_HHG_I_step(), false);

                    config.step_path(ii, "HHP_A_w");
                    file_prop_step.write(HHP.real(), config.path_HHP_R_step(), true);
                    file_prop_step.write(HHP.imag(), config.path_HHP_I_step(), false);
                }
                if (config_XNLO.output_electric_field() == 1) {
                    config.step_path(ii, "HHG_electric_field");
                    file_prop_step.write(E, config.path_HHG_E_step(), true);
                }

                //if (ii == 1) {
                //    hhg_old = prop.block(hhg) * (dz / double(config.interp_points() + 1));  // Normalisation to a dz volume
                //} else {
                    std::cout << "Starting interpolation!" << std::endl;

                    std::cout << "As a rough estimate," << std::endl;
                    std::cout << "   for n_z/2-n_z, there is a relative HHG source difference/tolerance of: ";
                    hhg_old_interp = hhg_old_old + 0.5*(hhg_new - hhg_old_old);
                    double hhg_tol_check = (hhg_old - hhg_old_interp).matrix().norm() / hhg_old.matrix().norm();
                    std::cout << hhg_tol_check << std::endl;

                    std::cout << "   And for wavelengths " << int(2.0*maths.pi/prop.k(wavelength_index_min).real()*1e9) << "-"<< int(2.0*maths.pi/prop.k(wavelength_index_max).real()*1e9) << "nm: " << std::endl;
                    std::cout << "   the nz/2-nz relattive HHG source difference/tolerance is: ";
                    hhg_tol_check = (hhg_old.block(wavelength_index_min, 0, (wavelength_index_max - wavelength_index_min), config.n_r()) - hhg_old_interp.block(wavelength_index_min, 0, (wavelength_index_max - wavelength_index_min), config.n_r())).matrix().norm() / hhg_old.block(wavelength_index_min, 0, (wavelength_index_max - wavelength_index_min), config.n_r()).matrix().norm();
                    std::cout << hhg_tol_check << std::endl;

                    std::cout << "   (hhg_new - hhg_old).matrix().norm() / hhg_new.matrix().norm(): " << (hhg_new - hhg_old).matrix().norm() / hhg_new.matrix().norm() << std::endl;

                    std::cout << "Interpolating on to " << config.interp_points() << " internal sites... " << std::endl;
                    hhg_new = prop.block(hhg) * (dz / double(config.interp_points() + 1));  // Normalisation to a dz volume
                    
                    double interp_dz = dz / double(config.interp_points() + 1);
                    dS_i = (hhg_new - hhg_old) / double(config.interp_points() + 1);
                    prop.print = false;
                    prop.z -= dz;

                    int HH_prop_start_end_step[2] = {-1, -1};
                    if (HHGP_starting_z_bool) {
                        // Send
                        double HH_z_data_tmp[2] = {prop.z, interp_dz};

                        int HH_prop_steps_per_thread;
                        int HH_prop_remainder_steps;
                        if (total_processes > 1) {
                            HH_prop_steps_per_thread = config.interp_points() / (total_processes - 1);
                            HH_prop_remainder_steps = config.interp_points() % (total_processes - 1);
                        } else {
                            HH_prop_steps_per_thread = 0;
                            HH_prop_remainder_steps = config.interp_points();
                        }
                        std::cout << "Performing HH interpolation and propagation on " << total_processes << " threads" << std::endl;
                        std::cout << "   with " << HH_prop_steps_per_thread << " step(s) per worker thread and " << HH_prop_remainder_steps << " step(s) on the master thread!" << std::endl;
                        for (int j = 1; j < total_processes; j++) {
                            HH_prop_start_end_step[0] = 1 + (j - 1)*HH_prop_steps_per_thread;
                            HH_prop_start_end_step[1] = HH_prop_start_end_step[0] + HH_prop_steps_per_thread;
                            MPI_Send(&HH_z_data_tmp, 2,
                                     MPI_DOUBLE, j, j, MPI_COMM_WORLD);
                            MPI_Send(&HH_prop_start_end_step, 2,
                                     MPI_INT, j, j, MPI_COMM_WORLD);

                            MPI_Send(hhg_old.real().data(),
                                     hhg_old.cols() * hhg_old.rows(),
                                     MPI_DOUBLE, j, j, MPI_COMM_WORLD);
                            MPI_Send(hhg_old.imag().data(),
                                     hhg_old.cols() * hhg_old.rows(),
                                     MPI_DOUBLE, j, j, MPI_COMM_WORLD);

                            MPI_Send(dS_i.real().data(),
                                     dS_i.cols() * dS_i.rows(),
                                     MPI_DOUBLE, j, j, MPI_COMM_WORLD);
                            MPI_Send(dS_i.imag().data(),
                                     dS_i.cols() * dS_i.rows(),
                                     MPI_DOUBLE, j, j, MPI_COMM_WORLD);
                        }
                        HH_prop_start_end_step[0] = 1 + (total_processes-1)*HH_prop_steps_per_thread;
                        HH_prop_start_end_step[1] = HH_prop_start_end_step[0] + HH_prop_remainder_steps;
                    }

                    prop.z += interp_dz*HH_prop_start_end_step[0];
                    for (int interp_i = HH_prop_start_end_step[0]; interp_i < HH_prop_start_end_step[1]; interp_i++) {
                        //prop.z += interp_dz;
                        hhg_i = hhg_old + interp_i * dS_i;
                        prop.nearFieldPropagationStep((config.Z() - prop.z), hhg_i);
                        HHP += prop.A_w_r;
                        prop.z += interp_dz;
                    }
                    prop.print = true;
                    hhg_old_old = hhg_old;
                    hhg_old = hhg_new;

                    for (int j = 1; j < total_processes; j++) {
                        MPI_Recv(HHP_multiThread_tmp.real().data(), HHP_multiThread_tmp.cols() * HHP_multiThread_tmp.rows(),
                                 MPI_DOUBLE, j, j, MPI_COMM_WORLD, &status);
                        MPI_Recv(HHP_multiThread_tmp.imag().data(), HHP_multiThread_tmp.cols() * HHP_multiThread_tmp.rows(),
                                 MPI_DOUBLE, j, j, MPI_COMM_WORLD, &status);
                        HHP += HHP_multiThread_tmp;
                    }


                    std::cout << "Interpolation complete!" << std::endl;
                    if ((ii - initial_step) % config.output_sampling_rate() == 0) {
                        config.step_path(ii, "HHP_A_w");
                        file_prop_step.write(HHP.real(), config.path_HHP_R_step(), true);
                        file_prop_step.write(HHP.imag(), config.path_HHP_I_step(), false);
                    }
                //}
            }// Recieve
            else if (total_processes > 1 && HHGP_starting_z_bool) {
                // Receive
                // For receiving prop.z and interp_dz
                double HH_z_data_tmp[2];
                MPI_Recv(&HH_z_data_tmp, 2, MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD, &status);
    // This one can be done before the main loop even starts as it's a const value
                double interp_dz = HH_z_data_tmp[1];

                int HH_prop_start_end_step[2];
                MPI_Recv(&HH_prop_start_end_step, 2, MPI_INT, 0, this_process, MPI_COMM_WORLD, &status);

                MPI_Recv(hhg_old.real().data(), hhg_old.cols() * hhg_old.rows(),
                         MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD, &status);
                MPI_Recv(hhg_old.imag().data(), hhg_old.cols() * hhg_old.rows(),
                         MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD, &status);

                MPI_Recv(dS_i.real().data(), dS_i.cols() * dS_i.rows(),
                         MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD, &status);
                MPI_Recv(dS_i.imag().data(), dS_i.cols() * dS_i.rows(),
                         MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD, &status);

                prop.z = HH_z_data_tmp[0] + interp_dz*(HH_prop_start_end_step[0]);

                HHP = ArrayXXcd::Zero(prop.n_k, config.n_r());

                for (int interp_i = HH_prop_start_end_step[0]; interp_i < HH_prop_start_end_step[1]; interp_i++) {
                    hhg_i = hhg_old + interp_i * dS_i;
                    prop.nearFieldPropagationStep((config.Z() - prop.z), hhg_i);
                    HHP += prop.A_w_r;
                    prop.z += interp_dz;
                }

                MPI_Send(HHP.real().data(),
                         HHP.cols() * HHP.rows(),
                         MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD);
                MPI_Send(HHP.imag().data(),
                         HHP.cols() * HHP.rows(),
                         MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD);
            }



        }
        if (this_process == 0) {
            std::cout << "-------------------------------------------------------------------------------\n";
        }

        if (this_process == 0) {
            // Output, as final block step, so don't overwrite and loose info from other blocks
            IO file;
            file.write(laser_driving.A_w_active.real(), config.path_A_w_R_step());
            file.write(laser_driving.A_w_active.imag(), config.path_A_w_I_step());
            file.write(tw.w_active, config.path_w_active());

            file.write(hhg.real(), config.path_HHG_R_step());
            file.write(hhg.imag(), config.path_HHG_I_step());
            file.write(w_active_HHG, config.path_HHG_w());

            //file.write(HHP.real(), config.path_HHP_R_step());
            //file.write(HHP.imag(), config.path_HHP_I_step());
            config.step_path(ii, "HHP_A_w");
            file.write(HHP.real(), config.path_HHP_R_step());
            file.write(HHP.imag(), config.path_HHP_I_step());
        }

        // Clean up
        DftiFreeDescriptor(&ft);
        MPI_Finalize();

        if (this_process == 0) {
            std::cout << "\n-------------------------------------------------------------------------------\n";
            std::cout << "UPPE successfully ran!\n";
            std::cout << "-------------------------------------------------------------------------------\n";
        }
}
