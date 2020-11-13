
#include <mpi.h>
#include <mkl.h>
#include <iostream>
#include <string>

#include "UPPE_simulation.hpp"

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

#include "../HHGP/propagation.hpp"

using namespace Eigen;

namespace UPPE {

    UPPE_simulation::UPPE_simulation(int argc, char** argv){
        // MPI
        //int this_process;
        //int total_processes;
        //MPI_Status status;

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
        MPI_Comm_rank(MPI_COMM_WORLD, &this_process);
    }

    void UPPE_simulation::set_initial_configs(std::string config_UPPE_file_path_, std::string config_XNLO_file_path_, std::string config_HHGP_file_path_){
        config_UPPE_file_path = config_UPPE_file_path_;
        config_XNLO_file_path = config_XNLO_file_path_;
        config_HHGP_file_path = config_HHGP_file_path_;

        if (this_process == 0) { std::cout << "Setting initial config settings" << std::endl; }

        if(config_UPPE_file_path.empty()) {
          if (this_process == 0) { std::cout << "Using default config file path " << config.path_config_file() << std::endl; }
        } else {
            config.path_config_file_set(config_UPPE_file_path);
            config.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
            if (this_process == 0) { std::cout << "Using config file path " << config.path_config_file() << std::endl; }
        }
        config.read_in(config.path_config_file(), false);
        if (config.read_in_laser_pulse() == 0) { config.check_paths(false); }

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
     
            config.n_m_set(config_XNLO.atoms_per_worker()*(total_processes-1));
            config.n_m_description_set("(int) Number of modes, set through UPPE");
            config.n_r_set(config_XNLO.atoms_per_worker()*(total_processes-1));
            config.n_r_description_set("(int) Number of radial points, set through UPPE");
        }
        if (this_process == 0) { config.print(config.path_config_log()); }

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

            config_HHGP.n_m_set(config_XNLO.atoms_per_worker()*(total_processes-1));
            config_HHGP.n_m_description_set("(int) Number of modes, set through UPPE");
            config_HHGP.n_r_set(config_XNLO.atoms_per_worker()*(total_processes-1));
            config_HHGP.n_r_description_set("(int) Number of radial points, set through UPPE");
        }
    }

    void UPPE_simulation::set_initial_constructors(){
        if (this_process == 0) { std::cout << "Setting initial constrcutors" << std::endl; }

        maths_textbook maths(config.path_input_j0());
        grid_rkr rkr(config.n_r(), config.R(), config.n_m(), maths);

        //grid_tw tw;
        if (this_process == 0) {
            tw = grid_tw(config.n_t(), config.T(), config.w_active_min(), config.w_active_max(), maths, true, true);
        } else {
            tw = grid_tw(config.n_t(), config.T(), config.w_active_min(), config.w_active_max(), maths, true, false);
        }

        //physics_textbook physics;

        w_active_min_HHG = 2.0 * maths.pi * physics.c / config.HHG_lambda_max();
        w_active_max_HHG = 2.0 * maths.pi * physics.c / config.HHG_lambda_min();
        //XNLO::grid_tw tw_XNLO;
        if (this_process == 0) {
            tw_XNLO = XNLO::grid_tw(config_XNLO.N_t(), config_XNLO.t_min(), config_XNLO.t_max(), w_active_max_HHG, true);
        } else {
            tw_XNLO = XNLO::grid_tw(config_XNLO.N_t(), config_XNLO.t_min(), config_XNLO.t_max(), w_active_max_HHG, false);
        }
    }

    void UPPE_simulation::set_initial_variables(){
        if (this_process == 0) { std::cout << "Setting initial variables" << std::endl; }

        dz = config.Z() / double(config.n_z());
        initial_step = 0;

        if (config.read_in_laser_pulse() == 1) {
            std::size_t found = config.path_A_w_R_initial().find_last_of("/");
            std::string tmp = config.path_A_w_R_initial().substr(found+1);
            found = tmp.find_first_of("_");
            std::string sim_no_str = tmp.substr(0, found);
            
            std::size_t found_2 = tmp.find_first_of("_", found+1);
            std::string tmp_2 = tmp.substr(found+1, found_2-found-1);
            sim_no = std::stoi(sim_no_str);

            initial_step = stoi(tmp_2);
            initial_step = int(double(config.n_z()) * double(stoi(tmp_2)) / double(config.original_n_z()));

            propagation_step = initial_step;// + 1;  // The way the main loop works is that it propagates the laser to step i to start with, so the
                                                     // initial step being read in is step i-1
            initial_position = dz * initial_step;
        } else {
            propagation_step = 1;
        }
        initial_position = dz * propagation_step;
    }

    void UPPE_simulation::print_initial_variable_debug(){
        if (this_process == 0) std::cout << "w_active_min_index: " << tw.w_active_min_index << std::endl;

        if (config.read_in_laser_pulse() == 1 && this_process == 0) {
            std::cout << "Simulation run (read in): " << sim_no << std::endl;
            std::cout << "Initial step: " << initial_step << std::endl;
            std::cout << "Next propagation step (read in from initial step): " << propagation_step << std::endl;
            std::cout << "Original position: " << double(initial_step) / double(config.n_z()) * config.Z() << std::endl;
            std::cout << "New position: " << double(initial_step) / double(config.n_z()) * config.Z() << std::endl;
        }
    }

    void UPPE_simulation::update_configs(){
        if (this_process == 0) { std::cout << "Updating config settings" << std::endl; }

        if (config.n_t() != tw.n_t) {
            config.n_t_set(tw.n_t);
            config.n_t_description_set("(double) Number of points on t and w grids, set through UPPE n_t expansion");
        }

        if (config_XNLO.N_t() != tw_XNLO.N_t) {
            config_XNLO.N_t_set(tw_XNLO.N_t);
            config_XNLO.N_t_description_set("(double) Number of points on t and w grids, set through UPPE n_t expansion");
        }

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
    }

    void UPPE_simulation::set_Fourier_transforms(){
        if (this_process == 0) { std::cout << "Setting Fourier transforms" << std::endl; }

        dimensions = 1;
        length = config.n_t();
        scale = 1.0 / config.n_t();
        DftiCreateDescriptor(&ft, DFTI_DOUBLE, DFTI_COMPLEX, dimensions, length);
        DftiSetValue(ft, DFTI_BACKWARD_SCALE, scale);
        DftiCommitDescriptor(ft);

        dimensions_HHG = 1;
        length_HHG = config_XNLO.N_t();
        scale_HHG = 1.0 / config_XNLO.N_t();
        DftiCreateDescriptor(&ft_HHG, DFTI_DOUBLE, DFTI_COMPLEX, dimensions_HHG, length_HHG);
        DftiSetValue(ft_HHG, DFTI_BACKWARD_SCALE, scale_HHG);
        DftiCommitDescriptor(ft_HHG);
    }

    void UPPE_simulation::set_and_segment_HH_frequency_grid(){
        if (this_process == 0) { std::cout << "Setting and segmenting HH frequency grid" << std::endl; }

        n_active_HHG = 0;
        w_active_min_index_HHG = 0;
        ArrayXd w_tmp = tw_XNLO.w;
        while (w_tmp(w_active_min_index_HHG) < w_active_min_HHG)
            w_active_min_index_HHG++;
        int count = 0;
        while (w_tmp(count) < w_active_max_HHG) {
            count++;
        }
        n_active_HHG = count - w_active_min_index_HHG;
        if (this_process == 0) std::cout << "w_active_min_index_HHG: " << w_active_min_index_HHG << ", n_active_HHG: " << n_active_HHG << std::endl;
        w_active_HHG = w_tmp.segment(w_active_min_index_HHG, n_active_HHG);
    }

    void UPPE_simulation::set_remaining_constructors(){
        if (this_process == 0) { std::cout << "Setting remaining constructors" << std::endl; }

        ht = DHT(config.n_r(), maths);
        laser_driving = UPPE::laser_pulse(config.p_av(), config.rep(), config.fwhm(), config.l_0(), config.ceo(), config.waist(),
                                          tw, rkr, ft, ht, maths,
                                          config,
                                          config.read_in_laser_pulse(), initial_position,
                                          config.laser_rel_tol(),
                                          false);
        if (this_process == 0) laser_driving.print = true;
        
        capillary_driving = capillary_fibre(config.Z(), rkr, tw, physics, maths);
        gas = keldysh_gas(config.press(), tw, ft, maths, config.gas_pressure_profile());

        if (total_processes > 1) {
            atomResponse = XNLO_AtomResponse(&rkr, &tw_XNLO, &maths, &physics,
                                             this_process, total_processes,
                                             config_XNLO,
                                             "minimum");
        }

        prop = propagation(config.HHP_E_min(), config.HHP_E_max(), config.Z(), w_active_HHG,
                           gas, rkr,
                           physics, maths, ht);
        prop.print = false;
        if (this_process == 0) { prop.print = true; }
        prop.z = double(initial_step) / double(config.n_z()) * double(config.Z());
    }

    void UPPE_simulation::set_remaining_variables(){
        if (this_process == 0) { std::cout << "Setting remaining variables" << std::endl; }

        acceleration_HHG = ArrayXXd::Zero(config_XNLO.N_t(), config.n_r());
        ArrayXXd E;
        if (config_XNLO.output_electric_field() == 1) {
            E = ArrayXXd::Zero(config_XNLO.N_t(), config.n_r());
        } else {
            E = ArrayXXd::Zero(0, 0);
        }

        neutral_atoms = ArrayXd::Zero(config.n_r());
        temp_linSpace_HHG_acceleration = ArrayXd::LinSpaced(config_XNLO.N_t(), -500.0e-15, 500.0e-15);  // I think this is wring and should be using T_min/T_max of XNLO???
        window_HHG_acceleration = (1 - ((0.5 * maths.pi * temp_linSpace_HHG_acceleration / 500e-15).sin()).pow(50));
        temp_linSpace_HHG_acceleration = 0;

        //ArrayXXcd hhg;
        hhg_new = ArrayXXcd::Zero(prop.n_k, config.n_r());
        //ArrayXXcd hhg_source;
        //ArrayXXcd hhg_previous;

        //ArrayXXcd HHG_tmp;// = ArrayXXcd::Zero(w_active_HHG.rows(), config.n_r());
        HHP = ArrayXXcd::Zero(prop.n_k, config.n_r());
        HHP_multiThread_tmp = ArrayXXcd::Zero(prop.n_k, config.n_r());
        HHP_multiThread_tmp_2 = ArrayXXcd::Zero(prop.n_k, config.n_r());

        hhg_old = ArrayXXcd::Zero(prop.n_k, config.n_r());//;// = ArrayXXcd::Zero(w_active_HHG.rows(), config.n_r());
        hhg_old_old = ArrayXXcd::Zero(prop.n_k, config.n_r());
        hhg_old_interp = ArrayXXcd::Zero(prop.n_k, config.n_r());
        dS_i = ArrayXXcd::Zero(prop.n_k, config.n_r());
        hhg_i = ArrayXXcd::Zero(prop.n_k, config.n_r());

        if (this_process == 0) {
            wavelength_index_min = 0;
            while(2.0 * maths.pi / prop.k(wavelength_index_min).real() *1e9 > 33.0) {
                wavelength_index_min++;
            }
            wavelength_index_max = 0;
            while(2.0 * maths.pi / prop.k(wavelength_index_max).real() *1e9 > 26.0) {
                wavelength_index_max++;
            }
        }
    }

    void UPPE_simulation::initialise_UPPE_simulation(std::string config_file_path, std::string config_XNLO_file_path, std::string config_HHGP_file_path){
        if (this_process == 0) {
            std::cout << "-------------------------------------------------------------------------------\n";
            std::cout << "                                  UPPE\n";
            std::cout << "-------------------------------------------------------------------------------\n";
        }
        if (this_process == 0) { std::cout << "Initialising UPPE simulation" << std::endl; }

        set_initial_configs(config_UPPE_file_path, config_XNLO_file_path, config_HHGP_file_path);
        set_initial_constructors();
        set_initial_variables();
        print_initial_variable_debug();
        update_configs();
        set_Fourier_transforms();
        set_and_segment_HH_frequency_grid();
        set_remaining_constructors();
        set_remaining_variables();
    }

    void UPPE_simulation::simulation_step(int ii){
        if (this_process == 0) {
            std::cout << "Propagation step: " << ii << std::endl;
            // Driving pulse:
            // Always start at 1, so first step is always outputted, even when reading in
            if ((ii - propagation_step) % config.output_sampling_rate() == 0) {
                config.step_path(ii, "UPPE_A_w");
                file_prop_step.write(laser_driving.A_w_active.real(), config.path_A_w_R_step(), true);
                file_prop_step.write(laser_driving.A_w_active.imag(), config.path_A_w_I_step(), false);
                
                // If config set to output electron density then output it, if not then skip
                if (config.output_electron_density() == 1) {
                    config.step_path(ii, "UPPE_electron_density");
                    file_prop_step.write(laser_driving.electron_density, config.path_electron_density_step(), true);
                }
            }
            A_w_active = laser_driving.A_w_active;
            if ((total_processes > 1) && HHGP_starting_z_bool) {
                // Send
                ArrayXXd A_w_active_send = laser_driving.A_w_active.real();
                for (int j = 1; j < total_processes; j++) {
                    MPI_Send(A_w_active_send.data(),
                             laser_driving.A_w_active.cols() * laser_driving.A_w_active.rows(),
                             MPI_DOUBLE, j, j, MPI_COMM_WORLD);
                }
            }
        } else if (HHGP_starting_z_bool) {
            // Receive
            ArrayXXd A_w_active_recv = ArrayXXd::Zero(laser_driving.A_w_active.cols(), laser_driving.A_w_active.rows());
            MPI_Recv(A_w_active_recv.data(), laser_driving.A_w_active.cols() * laser_driving.A_w_active.rows(),
                     MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD, &status);
            A_w_active = A_w_active_recv;
        }

        if ((total_processes > 1) && HHGP_starting_z_bool) {
            atomResponse.run(A_w_active, tw.w_active, tw.w_active_min_index);
        }

        if (this_process == 0 && total_processes > 1 && HHGP_starting_z_bool) {
            // Use the electron density at the last time step to calculate the number of neutral atoms
            for (int j = 0; j < rkr.n_r; j++) {
                neutral_atoms(j) = gas.atom_density(double(ii)*dz) - laser_driving.electron_density(laser_driving.electron_density.rows() - 1, j);
            }
            if (config_XNLO.output_electric_field() == 1) {
                E = atomResponse.E;
            }
            // Delete atomResponse after use to save ram
            acceleration_HHG = atomResponse.acceleration;
            for (int j = 0; j < rkr.n_r; j++) {
                for (int i = 0; i < config_XNLO.N_t(); i++) {
                    acceleration_HHG.row(i).col(j) *= neutral_atoms(j);
                    acceleration_HHG.row(i).col(j) *= window_HHG_acceleration.row(i);// / (w.row(i)).pow(2);
                }
            }
            // Apply forward spectral transform
            ArrayXXcd accelerationToHHSource = acceleration_HHG.cast<std::complex<double> >();
            for (int i = 0; i < rkr.n_r; i++) {
                DftiComputeForward(ft_HHG, accelerationToHHSource.col(i).data());
            }
            hhg = accelerationToHHSource.block(w_active_min_index_HHG, 0, n_active_HHG, rkr.n_r);
            //for (int j = 0; j < rkr.n_r; j++) {
                //for (int i = 0; i < n_active_HHG; i++) {
                //    hhg.row(i).col(j) /= (w_active_HHG.row(i)).pow(2);
                //}
            //}
            // Propagate high harmonics from current step to end of capillary
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

                // Need to have a check to see if I want tooutput HHP at first step or not
                // ()
                config.step_path(ii, "HHP_A_w");
                file_prop_step.write(HHP.real(), config.path_HHP_R_step(), true);
                file_prop_step.write(HHP.imag(), config.path_HHP_I_step(), false);

                if (config_XNLO.output_electric_field() == 1) {
                    config.step_path(ii, "HHG_electric_field");
                    file_prop_step.write(E, config.path_HHG_E_step(), true);
                }
            }
        }
    }

    void UPPE_simulation::interpolation_step(int ii){
        if (this_process == 0 && total_processes > 1 && HHGP_starting_z_bool) {
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

                    MPI_Send(hhg_old.data(),
                             hhg_old.cols() * hhg_old.rows(),
                             MPI_DOUBLE_COMPLEX, j, j, MPI_COMM_WORLD);

                    MPI_Send(dS_i.data(),
                             dS_i.cols() * dS_i.rows(),
                             MPI_DOUBLE_COMPLEX, j, j, MPI_COMM_WORLD);
                }
                HH_prop_start_end_step[0] = 1 + (total_processes-1)*HH_prop_steps_per_thread;
                HH_prop_start_end_step[1] = HH_prop_start_end_step[0] + HH_prop_remainder_steps;
            }
            prop.z += interp_dz*HH_prop_start_end_step[0];
            for (int interp_i = HH_prop_start_end_step[0]; interp_i < HH_prop_start_end_step[1]; interp_i++) {
                hhg_i = hhg_old + interp_i * dS_i;
                prop.nearFieldPropagationStep((config.Z() - prop.z), hhg_i);
                HHP += prop.A_w_r;
                prop.z += interp_dz;
            }
            prop.print = true;
            hhg_old_old = hhg_old;
            hhg_old = hhg_new;

            HHP_multiThread_tmp_2 = ArrayXXcd::Zero(prop.n_k, config.n_r());
            for (int j = 1; j < total_processes; j++) {
                MPI_Recv(HHP_multiThread_tmp.data(), HHP_multiThread_tmp.cols() * HHP_multiThread_tmp.rows(),
                         MPI_DOUBLE_COMPLEX, j, j, MPI_COMM_WORLD, &status);
                HHP_multiThread_tmp_2 += HHP_multiThread_tmp;
            }

            HHP += HHP_multiThread_tmp_2;

            std::cout << "Interpolation complete!" << std::endl;
            if ((ii - initial_step) % config.output_sampling_rate() == 0) {
                config.step_path(ii, "HHP_A_w");
                file_prop_step.write(HHP.real(), config.path_HHP_R_step(), true);
                file_prop_step.write(HHP.imag(), config.path_HHP_I_step(), false);
            }
        } else if (total_processes > 1 && HHGP_starting_z_bool) {
            // Receive
            // For receiving prop.z and interp_dz
            double HH_z_data_tmp[2];
            MPI_Recv(&HH_z_data_tmp, 2, MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD, &status);
            // This one can be done before the main loop even starts as it's a const value
            double interp_dz = HH_z_data_tmp[1];

            int HH_prop_start_end_step[2];
            MPI_Recv(&HH_prop_start_end_step, 2, MPI_INT, 0, this_process, MPI_COMM_WORLD, &status);

            MPI_Recv(hhg_old.data(), hhg_old.cols() * hhg_old.rows(),
                     MPI_DOUBLE_COMPLEX, 0, this_process, MPI_COMM_WORLD, &status);

            MPI_Recv(dS_i.data(), dS_i.cols() * dS_i.rows(),
                     MPI_DOUBLE_COMPLEX, 0, this_process, MPI_COMM_WORLD, &status);

            prop.z = HH_z_data_tmp[0] + interp_dz*(HH_prop_start_end_step[0]);

            HHP = ArrayXXcd::Zero(prop.n_k, config.n_r());

            for (int interp_i = HH_prop_start_end_step[0]; interp_i < HH_prop_start_end_step[1]; interp_i++) {
                hhg_i = hhg_old + interp_i * dS_i;
                prop.nearFieldPropagationStep((config.Z() - prop.z), hhg_i);
                HHP += prop.A_w_r;
                prop.z += interp_dz;
            }

            MPI_Send(HHP.data(),
                     HHP.cols() * HHP.rows(),
                     MPI_DOUBLE_COMPLEX, 0, this_process, MPI_COMM_WORLD);
        }
    }

    void UPPE_simulation::run_UPPE_simulation(){
        if (this_process == 0) { std::cout << "Running UPPE simulation" << std::endl; }

        if (this_process == 0) {
            std::cout << "-------------------------------------------------------------------------------\n";
            std::cout << " Main Program:\n";
            std::cout << "-------------------------------------------------------------------------------\n";

            std::cout << "Laser p_pk: " << laser_driving.p_pk << ", laser E_pk: " << laser_driving.E_pk << std::endl;
        }

        if (this_process == 0) {
            // Output already known variables, in case crashes etc, so they are already saved early on
            file.write(tw.w_active, config.path_w_active());
            file.write(w_active_HHG, config.path_HHG_w());
            file.write(prop.w_active, config.path_HHP_w());

            // Diagnostic stats that may be useful one day:
            std::cout << "Basic diagnostic values from XNLO frequency grid:" << std::endl;
            std::cout << "   Min XNLO frequency: " << tw_XNLO.w(1) - tw_XNLO.w(0) << std::endl;
            std::cout << "   Max XNLO frequency: " << tw_XNLO.w.maxCoeff() << std::endl;
            std::cout << "   Min allowed HHG wavelength: " << 1.0e9 * physics.c / tw_XNLO.w.maxCoeff() << "nm" << std::endl;
            std::cout << "   Max allowed HHG wavelength: " << 1.0e9 * physics.c / (tw_XNLO.w(1) - tw_XNLO.w(0)) << "nm" << std::endl;
            std::cout << "   Min allowed HHG energy: " << 1.2398 / (1.0e6 * physics.c / (tw_XNLO.w(1) - tw_XNLO.w(0))) << "eV" << std::endl;
            std::cout << "   Max allowed HHG energy: " << 1.2398 / (1.0e6 * physics.c / tw_XNLO.w.maxCoeff()) << "eV" << std::endl;
        }

        MPI_Barrier(MPI_COMM_WORLD);
        
        int ii;
        if (config.read_in_laser_pulse() == true) {
            ii = propagation_step;
        } else {
            ii = 0;
            propagation_step = 0;
        }
        if (dz*ii >= config.HHGP_starting_z()) {
            HHGP_starting_z_bool = true;
        } else {
            HHGP_starting_z_bool = false;
        }

        // Doing first step outside main loop as the interpolation stage can't be done in it since there's only one step
        // and no previous step
        simulation_step(ii);
        if (this_process == 0 && total_processes > 1 && HHGP_starting_z_bool) {
            hhg_old = prop.block(hhg) * (dz / double(config.interp_points() + 1));  // Normalisation to a dz volume
        }
        propagation_step++;

        for (int ii = propagation_step; ii < config.ending_n_z() + 1; ii++) {
            simulation_step(ii);
            interpolation_step(ii);
        }

        if (this_process == 0) {
            std::cout << "-------------------------------------------------------------------------------\n";
        }

        if (this_process == 0) {
            // Output, as final block step, so don't overwrite and loose info from other blocks
            IO file;
            if (config.n_z() != config.ending_n_z()) {
                config.step_path(config.ending_n_z(), "UPPE_A_w");
                file.write(laser_driving.A_w_active.real(), config.path_A_w_R_step());
                file.write(laser_driving.A_w_active.imag(), config.path_A_w_I_step());
                file.write(tw.w_active, config.path_w_active());

                config.step_path(config.ending_n_z(), "HHG_A_w");
                file.write(hhg.real()* (dz / double(config.interp_points() + 1)), config.path_HHG_R_step());
                file.write(hhg.imag()* (dz / double(config.interp_points() + 1)), config.path_HHG_I_step());
                file.write(w_active_HHG, config.path_HHG_w());

                config.step_path(config.ending_n_z(), "HHP_A_w");
                file.write(HHP.real(), config.path_HHP_R_step());
                file.write(HHP.imag(), config.path_HHP_I_step());
                file.write(prop.w_active, config.path_HHP_w());
            } else {
                file.write(laser_driving.A_w_active.real(), config.path_A_w_R_step());
                file.write(laser_driving.A_w_active.imag(), config.path_A_w_I_step());
                file.write(tw.w_active, config.path_w_active());

                file.write(hhg.real()* (dz / double(config.interp_points() + 1)), config.path_HHG_R_step());
                file.write(hhg.imag()* (dz / double(config.interp_points() + 1)), config.path_HHG_I_step());
                file.write(w_active_HHG, config.path_HHG_w());

                file.write(HHP.real(), config.path_HHP_R_step());
                file.write(HHP.imag(), config.path_HHP_I_step());
                file.write(prop.w_active, config.path_HHP_w());
            }
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
} // End of UPPE namespace
