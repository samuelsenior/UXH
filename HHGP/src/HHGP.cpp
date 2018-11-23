#include <iostream>
#include <string>

#include <mpi.h>

#include "Eigen/Dense"

#include "HHGP.hpp"

#include "config_settings.hpp"
#include "maths_textbook.hpp"
#include "physics_textbook.hpp"
#include "keldysh_gas.hpp"
#include "DHT.hpp"
#include "grid_rkr.hpp"
#include "propagation.hpp"
#include "IO.hpp"

using namespace Eigen;

namespace HHGP {

	HHGP::HHGP() {

		// MPI
        int this_node;
        int total_nodes;
        MPI_Status status;

        //MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &total_nodes);
        MPI_Comm_rank(MPI_COMM_WORLD, &this_node);

        int world_rank, world_size;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        if (this_node == 0) {

	        config_file_path = "./config_HHGP_test.txt";
	        // Input Settings and Parameters
	        Config_Settings config;
	        if(config_file_path.empty()) {
	            std::cout << "Using default config file path " << config.path_config_file() << std::endl;
	        } else {
	            config.path_config_file_set(config_file_path);
	            config.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
	            std::cout << "Using config file path " << config.path_config_file() << std::endl;
	        }
	        config.read_in(config.path_config_file());
	        config.check_paths();
	        config.print();

	    //    // Am I expecting spectral amplitudes in terms of radial position or mode?
		//	int N_cols = source.cols();
		//    int N_rows = source.rows();
	    //
		//    int N_cols_w = w_active.cols();
		//    int N_rows_w = w_active.rows();

		    maths_textbook maths(config.path_input_j0());

		    // Set up Hankel transform
		//    DHT ht(N_cols, maths);
		//    int n_active = N_rows;

		    //--------------------------------------------------------------------------------------------//
		    // 2. Constructors
		    //--------------------------------------------------------------------------------------------//

		    // General
		    //Used above...
		    physics_textbook physics;

		    // Grids
		    grid_rkr rkr(config.n_r(), config.R(), config.n_m(), maths);
		    
		    MKL_LONG dimensions = 1;
		    MKL_LONG length = config.n_t();
		    double scale = 1.0 / config.n_t();
		    DFTI_DESCRIPTOR_HANDLE ft;
		    DftiCreateDescriptor(&ft, DFTI_DOUBLE, DFTI_COMPLEX, dimensions, length);
		    DftiSetValue(ft, DFTI_BACKWARD_SCALE, scale);
		    DftiCommitDescriptor(ft);

		    grid_tw tw(config.n_t(), config.T(), config.w_active_min(), config.w_active_max(), maths);
		    keldysh_gas gas(config.press(), tw, ft, maths);

		    // Change this to be read in from file eventually!
		    // E_min should really come from config or a data_config
		    E_min = 10.0;
		    // Propagation
		//    propagation prop(E_min, w_active, gas, rkr, ht);

		    config.print(config.path_config_log());
		}

	}

	ArrayXXcd HHGP::nearFieldStep(ArrayXXcd source, ArrayXXcd previous,
                                  ArrayXd w_active,
                                  double z, double dz) {
std::cout << "bar 1" << std::endl;
	//	std::string config_file_path;
    //    config_file_path = "./config_HHGP_test.txt";
    //    // Input Settings and Parameters
    //    Config_Settings config;
    //    if(config_file_path.empty()) {
    //        std::cout << "Using default config file path " << config.path_config_file() << std::endl;
    //    } else {
    //        config.path_config_file_set(config_file_path);
    //        config.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
    //        std::cout << "Using config file path " << config.path_config_file() << std::endl;
    //    }
    //    config.read_in(config.path_config_file());
    //    config.check_paths();
    //    config.print();

		// Am I expecting spectral amplitudes in terms of radial position or mode?
		N_cols = source.cols();
	    N_rows = source.rows();
std::cout << "bar 2" << std::endl;
	    N_cols_w = w_active.cols();
	    N_rows_w = w_active.rows();
std::cout << "bar 3" << std::endl;
	    maths_textbook maths(config.path_input_j0());
std::cout << "N_cols: " << N_cols << ", j0 zeros: " << maths.J0_zeros << std::endl;
	    // Set up Hankel transform
	    DHT ht(N_cols, maths);
std::cout << "bar 3.2" << std::endl;
	    n_active = N_rows;
std::cout << "bar 4" << std::endl;
	    //--------------------------------------------------------------------------------------------//
	    // 2. Constructors
	    //--------------------------------------------------------------------------------------------//

	//    // General
	//    //Used above...
	//    physics_textbook physics;
	//
	//    // Grids
	//    grid_rkr rkr(config.n_r(), config.R(), config.n_m(), maths);
	//    
	//    MKL_LONG dimensions = 1;
	//    MKL_LONG length = config.n_t();
	//    double scale = 1.0 / config.n_t();
	//    DFTI_DESCRIPTOR_HANDLE ft;
	//    DftiCreateDescriptor(&ft, DFTI_DOUBLE, DFTI_COMPLEX, dimensions, length);
	//    DftiSetValue(ft, DFTI_BACKWARD_SCALE, scale);
	//    DftiCommitDescriptor(ft);
    //
	//    grid_tw tw(config.n_t(), config.T(), config.w_active_min(), config.w_active_max(), maths);
	//    keldysh_gas gas(config.press(), tw, ft, maths);
    //
	//    // Change this to be read in from file eventually!
	//    // E_min should really come from config or a data_config
	//    double E_min = 10.0;
	//    // Propagation
std::cout << "E_min: " << E_min << ", w_active(0): " << w_active(0) << ", gas.atom_density_max: " << gas.atom_density_max << ", rkr.r(0): " << rkr.r(0) << ", ht.H(0): " << ht.H(0) << std::endl;
	    propagation prop(E_min, w_active, gas, rkr, ht);
    //
	//    config.print(config.path_config_log());
std::cout << "bar 4.1" << std::endl;
	    // ????
	    A_w_r = ArrayXXcd::Zero(n_active, N_cols);
std::cout << "bar 4.2" << std::endl;
	    A_w_r_tmp = ArrayXXcd::Zero(prop.n_k, N_cols);
std::cout << "bar 5" << std::endl;
	    // Want to propagate to the end fo the capillary and include the very final
	    // source terms but not propagate them outside of the capillary
	    // Don't need a step 1 for this when being called from UPPE
	    
        //prop.z += dz;
	    prop.z = z + dz;
std::cout << "bar 6" << std::endl;
        prop.nearFieldPropagationStep(dz, previous);
std::cout << "bar 7" << std::endl;
	    A_w_r_tmp = prop.A_w_r;
	    //
	    A_w_r = source;//hh_source.GetSource(i, config, maths);
std::cout << "bar 8" << std::endl;
	    A_w_r_tmp = prop.block(A_w_r);
std::cout << "bar 9" << std::endl;
	    A_w_r_tmp += prop.A_w_r;
std::cout << "BAR 10" << std::endl;

		return A_w_r_tmp;
	}
} // HHGP namespace