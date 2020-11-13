
#ifndef __UPPE_SIM_HPP_INCLUDED__
#define __UPPE_SIM_HPP_INCLUDED__


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

#include "../HHGP/propagation.hpp"

using namespace Eigen;

namespace UPPE {

    class UPPE_simulation {
        public:
            // Functions
            UPPE_simulation(int argc, char** argv);
            void initialise_UPPE_simulation(std::string config_file_path, std::string config_XNLO_file_path, std::string config_HHGP_file_path);
            void run_UPPE_simulation();

        private:
            int this_process;
            int total_processes;
            MPI_Status status;

            std::string config_UPPE_file_path;
            std::string config_XNLO_file_path;
            std::string config_HHGP_file_path;

            Config_Settings config;
            XNLO::Config_Settings config_XNLO;
            HH::Config_Settings config_HHGP;

            maths_textbook maths;
            grid_rkr rkr;
            grid_tw tw;
            physics_textbook physics;

            double dz;
            int initial_step;
            int propagation_step;
            double initial_position;
            int sim_no;

            double w_active_min_HHG;
            double w_active_max_HHG;
            XNLO::grid_tw tw_XNLO;

            DHT ht;
            UPPE::laser_pulse laser_driving;
            capillary_fibre capillary_driving;
            keldysh_gas gas;

            MKL_LONG dimensions;
            MKL_LONG length;
            double scale;
            DFTI_DESCRIPTOR_HANDLE ft;

            MKL_LONG dimensions_HHG;
            MKL_LONG length_HHG;
            double scale_HHG;
            DFTI_DESCRIPTOR_HANDLE ft_HHG;

            IO file_prop_step;
            IO file;

            XNLO_AtomResponse atomResponse;

            ArrayXd neutral_atoms;
            ArrayXd temp_linSpace_HHG_acceleration;
            ArrayXd window_HHG_acceleration;

            ArrayXXcd A_w_active;

            int n_active_HHG;
            ArrayXd w_active_HHG;
            int w_active_min_index_HHG;

            propagation prop;

            ArrayXXcd hhg;
            ArrayXXcd hhg_new;
            ArrayXXcd hhg_source;
            ArrayXXcd hhg_previous;

            ArrayXXcd HHG_tmp;
            ArrayXXcd HHP;
            ArrayXXcd HHP_multiThread_tmp;
            ArrayXXcd HHP_multiThread_tmp_2;

            ArrayXXcd hhg_old;
            ArrayXXcd hhg_old_old;
            ArrayXXcd hhg_old_interp;
            ArrayXXcd dS_i;
            ArrayXXcd hhg_i;

            int wavelength_index_min;
            int wavelength_index_max;

            bool HHGP_starting_z_bool;

            // Functions
            void set_initial_configs(std::string config_UPPE_file_path_, std::string config_XNLO_file_path_, std::string config_HHGP_file_path_);
            void update_configs();

            void set_initial_constructors();
            void set_remaining_constructors();

            void set_Fourier_transforms();

            void set_initial_variables();
            void print_initial_variable_debug();
            void set_remaining_variables();

            void set_and_segment_HH_frequency_grid();

            void simulation_step(int ii);
            void interpolation_step(int ii);
    };
} // UPPE namespace

#endif
