#include <iostream>
#include <string>

#include "../../Eigen/Dense"

#include "config_settings.hpp"
#include "../maths/maths_textbook.hpp"
#include "../physics/physics_textbook.hpp"
#include "HH_source.hpp"
#include "../gas/keldysh_gas.hpp"
#include "../DHT/DHT.hpp"
#include "../grid/grid_rkr.hpp"
#include "propagation.hpp"
#include "../IO/IO.hpp"

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
    HH::Config_Settings config;
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

    int inital_propagation_step = config.inital_propagation_step();

    maths_textbook maths(config.path_input_j0());
    HH_source hh_source;
    ArrayXXcd tmp_A_w_r = hh_source.GetSource(inital_propagation_step, config, maths);
    IO laser_pulse_file;

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

    // Grids
    //grid_rkr rkr(config.n_r(), config.R(), config.n_m(), maths);
    grid_rkr rkr(N_cols, config.R(), N_cols, maths);
    keldysh_gas gas(config.press(), config.gas_pressure_profile());

    // Change this to be read in from file eventually!
    // E_min should really come from config or a data_config
    double E_min = 10.0;
    double E_max = 206.0;
    // Propagation
    propagation prop(E_min, E_max, config.Z(), w_active, gas, rkr,
                     physics, maths, ht);//, config);

double dz = config.Z() / config.n_z();
   prop.z = double(config.inital_propagation_step()) / double(config.n_z()) * double(config.Z()) - dz;
//prop.z -= dz;

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

    // Want to propagate to the end fo the capillary and include the very final
    // source terms but not propagate them outside of the capillary
    bool to_end_only = true;
    for (int i = inital_propagation_step; i < config.n_z() + 1; i++) {
        std::cout << "Propagation Step: " << i << std::endl;
        if (to_end_only == true){
// Need to rethink if z += dz comes here or after propagation, have a feeling it should be after if
// propagating to end of capillary only...
            prop.z += dz;
            A_w_r = prop.block(hh_source.GetSource(i, config, maths)) * dz;  // Normalisation to a dz volume
            // If at the last step then we're at teh end of the capillary and so aren't looking
            // to propagate the last HH source any further, but rather just use it's source as it's
            // already at the desired position
            if (i < config.n_z()) {
                prop.nearFieldPropagationStep(dz, A_w_r);
                A_w_r_tmp += prop.A_w_r;
            } else {
                //prop.nearFieldPropagationStep(dz, A_w_r);
                A_w_r_tmp += A_w_r;//prop.A_w_r;
            }
        } else {
            if (i == inital_propagation_step) {
                // NOT SURE
                prop.z += dz;///2.0;
                // Get source at step, set current and previous values to it
                A_w_r = hh_source.GetSource(i, config, maths) * dz;  // Normalisation to a dz volume
                A_w_r_tmp = prop.block(A_w_r);
            } else {
                prop.z += dz;
                // Propagate source at previous step to the current step and store the previous step as it
                prop.nearFieldPropagationStep(dz, A_w_r_tmp);
                A_w_r_tmp = prop.A_w_r;
                // Get the new source at the current step and add the propagted source from the previous step to this
                A_w_r = hh_source.GetSource(i, config, maths) * dz;  // Normalisation to a dz volume
                A_w_r_tmp += prop.block(A_w_r);//prop.A_w_r;
            }
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
        file_prop_step.write_double(HHG_w, prop.w_active, prop.n_k, 1, false);
        // This would output the source and propagated HHG at the current position
    }

return 0;
}
