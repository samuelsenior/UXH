#include <iostream>
#include <string>

#include "../../Eigen/Dense"

#include "config_settings.hpp"
#include "../../src/maths_textbook.hpp"
#include "../../src/physics_textbook.hpp"
#include "HH_source.hpp"
#include "../../src/keldysh_gas.hpp"
#include "../../src/DHT.hpp"
#include "../../src/grid_rkr.hpp"
#include "propagation.hpp"
#include "../../src/IO.hpp"

using namespace Eigen;

int main(int argc, char** argv){

	// Get config file path passed in from command line with "-cf" flag
    std::string args[argc];
    std::string config_file_path;
    for (int i = 0; i < argc; i++) {
        args[i] = argv[i];
    }
    for (int i = 0; i < argc; i++) {
        if (args[i] == "-cf") {
            config_file_path = argv[i+1];
      }
    }

    std::cout << "-------------------------------------------------------------------------------\n";
    std::cout << "                                  HHGP\n";
    std::cout << "-------------------------------------------------------------------------------\n";


    //--------------------------------------------------------------------------------------------//
    // 1. Program input
    //--------------------------------------------------------------------------------------------//

    // Input Settings and Parameters
    Config_Settings config;
    if(config_file_path.empty()) {
        std::cout << "Using default config file path " << config.path_config_file() << std::endl;
    } else {
        config.path_config_file_set(config_file_path);
        config.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
        std::cout << "Using config file path " << config.path_config_file() << std::endl;
    }
    config.read_in(config.path_config_file(), false);
    // Will need to add this back in eventaully, but currently it adds '000_' to
    // the HHG paths, which  we don't want
    //config.check_paths(false);

    config.print();

    //--------------------------------------------------------------------------------------------//
    // Read in spectral amplitudes
    // Need to incorporate this nicely!
    //--------------------------------------------------------------------------------------------//
    // Read in spectral amplitudes from file and transfrom them into the spectral amplitudes
    // at different radial positions


    maths_textbook maths(config.path_input_j0());
    HH_source hh_source;
    ArrayXXcd tmp_A_w_r = hh_source.GetSource(1, config, maths);
    IO laser_pulse_file;

    //laser_pulse_file.read_header(config.path_A_w_R(), false);
    //ArrayXXd A_w_R = laser_pulse_file.read_double(config.path_A_w_R(), true, false);
    //laser_pulse_file.read_header(config.path_A_w_I(), false);
    //ArrayXXd A_w_I = laser_pulse_file.read_double(config.path_A_w_I(), true, false);
    int N_cols = tmp_A_w_r.cols();
    int N_rows = tmp_A_w_r.rows();

    laser_pulse_file.read_header(config.path_w_active(), false);
    ArrayXd w_active = laser_pulse_file.read_double(config.path_w_active(), true, false);
    int N_cols_w = laser_pulse_file.N_col_;
    int N_rows_w = laser_pulse_file.N_row_;

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
    //grid_rkr rkr(config.n_r(), config.R(), config.n_m(), maths);
    grid_rkr rkr(N_cols, config.R(), N_cols, maths);
    
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
    propagation prop(E_min, w_active, gas, rkr,
                     physics, maths, ht);


    config.print(config.path_config_log());

    std::cout << "-------------------------------------------------------------------------------\n";
    std::cout << "Main Program:\n";
    std::cout << "-------------------------------------------------------------------------------\n";

    // ????
    ArrayXXcd A_w_r = ArrayXXcd::Zero(n_active, N_cols);
    ArrayXXcd A_w_r_tmp = ArrayXXcd::Zero(prop.n_k, N_cols);

    IO file_prop_step;
    std::string dir = "../output/";
    std::string prepend = "000_";
    std::string restOfName_R = "_HHG_R.bin";
    std::string restOfName_I = "_HHG_I.bin";
    std::string restOfName_w = "_HHG_w.bin";
    double dz = config.Z() / config.n_z();
    // Want to propagate to the end fo the capillary and include the very final
    // source terms but not propagate them outside of the capillary
    //ArrayXXcd A_w_m_out = ArrayXXcd::Zero(prop.n_k, rkr.n_r);
    for (int i = 1; i < config.n_z() + 1; i++) {
        std::cout << "Propagation Step: " << i << std::endl;
        if (i == 1) {
            prop.z += dz;
            A_w_r = hh_source.GetSource(i, config, maths);
            A_w_r_tmp = prop.block(A_w_r);
        } else {
            prop.z += dz;
            prop.nearFieldPropagationStep(dz, A_w_r_tmp);
            A_w_r_tmp = prop.A_w_r;
            A_w_r = hh_source.GetSource(i, config, maths);
            A_w_r_tmp = prop.block(A_w_r);
            A_w_r_tmp += prop.A_w_r;
        }

        // Output step
        std::string propStep = std::to_string(static_cast<unsigned long long>(i));
        std::string HHG_R = dir + prepend + propStep + restOfName_R;
        std::string HHG_I = dir + prepend + propStep + restOfName_I;
        std::string HHG_w = dir + prepend + propStep + restOfName_w;

        //config.step_path(i);
        file_prop_step.overwrite(HHG_R, false);
        file_prop_step.write_header(HHG_R, prop.n_k, rkr.n_r, false);
        file_prop_step.write_double(HHG_R, A_w_r_tmp.real(), prop.n_k, rkr.n_r, false);

        file_prop_step.overwrite(HHG_I, false);
        file_prop_step.write_header(HHG_I, prop.n_k, rkr.n_r, false);
        file_prop_step.write_double(HHG_I, A_w_r_tmp.imag(), prop.n_k, rkr.n_r, false);

        file_prop_step.overwrite(HHG_w, false);
        file_prop_step.write_header(HHG_w, prop.n_k, 1, false);
        file_prop_step.write_double(HHG_w, prop.segment(w_active), prop.n_k, 1, false);
        // This would output the source and propagated HHG at the current position
    }

return 0;
}
