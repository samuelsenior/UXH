#include <iostream>
#include <string>

#include "Eigen/Dense"

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

	ArrayXXcd nearFieldStep(ArrayXXcd source, ArrayXXcd previous,
                            ArrayXd w_active,
                            double z, double dz) {

		std::string config_file_path;
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

		// Am I expecting spectral amplitudes in terms of radial position or mode?
		int N_cols = source.cols();
	    int N_rows = source.rows();

	    int N_cols_w = w_active.cols();
	    int N_rows_w = w_active.rows();

	    maths_textbook maths(config.path_input_j0());

	    // Set up Hankel transform
	    DHT ht(N_cols, maths);
	    int n_active = N_rows;

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
	    double E_min = 10.0;
	    // Propagation
	    propagation prop(E_min, w_active, gas, rkr, ht);

	    config.print(config.path_config_log());

	    // ????
	    ArrayXXcd A_w_r = ArrayXXcd::Zero(n_active, N_cols);
	    ArrayXXcd A_w_r_tmp = ArrayXXcd::Zero(prop.n_k, N_cols);

	    // Want to propagate to the end fo the capillary and include the very final
	    // source terms but not propagate them outside of the capillary
	    // Don't need a step 1 for this when being called from UPPE
	    
        //prop.z += dz;
	    prop.z = z + dz;
        prop.nearFieldPropagationStep(dz, previous);
	    A_w_r_tmp = prop.A_w_r;
	    //
	    A_w_r = source;//hh_source.GetSource(i, config, maths);
	    A_w_r_tmp = prop.block(A_w_r);
	    A_w_r_tmp += prop.A_w_r;

		return A_w_r_tmp;
	}
} // HHGP namespace