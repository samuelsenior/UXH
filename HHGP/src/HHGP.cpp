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
							double z, double dz, int step,
							Config_Settings config) {

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
	    prop.z += dz;
	    prop.nearFieldPropagationStep(dz, previous);
	    A_w_r_tmp = prop.A_w_r;
	    //
	    A_w_r = source;//hh_source.GetSource(i, config, maths);
	    A_w_r_tmp = prop.block(A_w_r);
	    A_w_r_tmp += prop.A_w_r;

		return A_w_r_tmp;
	}
}






/*

int main(int argc, char** argv) {

	//// Get config file path passed in from command line with "-cf" flag
    //std::string args[argc];
    //std::string config_file_path;
    //for (int i = 0; i < argc; i++) {
    //    args[i] = argv[i];
    //}
    //for (int i = 0; i < argc; i++) {
    //    if (args[i] == "-cf") {
    //        config_file_path = argv[i+1];
    //  }
    //}
	//
    //std::cout << "-------------------------------------------------------------------------------\n";
    //std::cout << "                                  HHGP\n";
    //std::cout << "-------------------------------------------------------------------------------\n";
	//
	//
    ////--------------------------------------------------------------------------------------------//
    //// 1. Program input
    ////--------------------------------------------------------------------------------------------//
	//
    //// Input Settings and Parameters
    //Config_Settings config;
    //if(config_file_path.empty()) {
    //    std::cout << "Using default config file path " << config.path_config_file() << std::endl;
    //} else {
    //    config.path_config_file_set(config_file_path);
    //    config.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
    //    std::cout << "Using config file path " << config.path_config_file() << std::endl;
    //}
    //config.read_in(config.path_config_file(), false);
    //// Will need to add this back in eventaully, but currently it adds '000_' to
    //// the HHG paths, which  we don't want
    ////config.check_paths(false);
	//
    //config.print();

    //--------------------------------------------------------------------------------------------//
    // Read in spectral amplitudes
    // Need to incorporate this nicely!
    //--------------------------------------------------------------------------------------------//
    // Read in spectral amplitudes from file and transfrom them into the spectral amplitudes
    // at different radial positions

    int N_cols = source.cols();//laser_pulse_file.N_col_;
    int N_rows = source.rows();//laser_pulse_file.N_row_;

    //laser_pulse_file.read_header(config.path_w_active(), false);
    int N_cols_w = w_active.cols();//laser_pulse_file.N_col_;
    int N_rows_w = w_active.rows();//laser_pulse_file.N_row_;

    maths_textbook maths(config.path_input_j0());

    // Set up Hankel transform
    DHT ht(N_cols, maths);
    int n_active = N_rows;

    //--------------------------------------------------------------------------------------------//
    // 2. Constructors
    //--------------------------------------------------------------------------------------------//

    // General
    //Used above...
    //maths_textbook maths(config.path_input_j0());
    physics_textbook physics;

    //Set up above
    //DHT ht(config.n_r(), maths);

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
    //if (step == 1) {
    //    prop.z += dz;
    //    A_w_r = hh_source.GetSource(i, config, maths);
    //    A_w_r_tmp = prop.block(A_w_r);
    //    //prop.z += dz;
    //} else {
        //A_w_r = GetSource(i, config, maths);
        //A_w_r_tmp = prop.block(A_w_r);
    prop.z += dz;
    prop.nearFieldPropagationStep(dz, previous);
    A_w_r_tmp = prop.A_w_r;
    //
    A_w_r = source;//hh_source.GetSource(i, config, maths);
    A_w_r_tmp = prop.block(A_w_r);
    A_w_r_tmp += prop.A_w_r;
        //prop.z += dz;
        //A_w_r_tmp += prop.block(GetSource(i, config, maths));
    //}

    //// Forward spectral transform
    //ArrayXXcd A_w_m = ArrayXXcd::Zero(prop.n_k, rkr.n_r);
    //for (int ii = 0; ii < prop.n_k; ii++)
    //    A_w_m.row(ii) = ht.forward(A_w_r_tmp.row(ii));


	return A_w_r_tmp;
}

*/
