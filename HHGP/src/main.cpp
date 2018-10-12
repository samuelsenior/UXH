#include <iostream>
#include <string>

#include "Eigen/Dense"

#include "config_settings.hpp"
#include "maths_textbook.hpp"
#include "physics_textbook.hpp"
#include "HH_source.hpp"
#include "keldysh_gas.hpp"
#include "DHT.hpp"
#include "grid_rkr.hpp"
#include "propagation.hpp"
#include "IO.hpp"

using namespace Eigen;

/*Eigen::ArrayXXcd GetSource(int fileNumber,
                           Config_Settings config, maths_textbook maths) {

    std::string dir = "../../../Results/UPPE-XNLO/Convergence/nz-150/data/";
    std::string prepend = "002_";
    std::string propStep = std::to_string(static_cast<unsigned long long>(fileNumber));
    std::string restOfName_R = "_HHG_R.bin";
    std::string restOfName_I = "_HHG_I.bin";

    std::string HHG_R = dir + prepend + propStep + restOfName_R;
    std::string HHG_I = dir + prepend + propStep + restOfName_I;

    IO laser_pulse_file;
    laser_pulse_file.read_header(HHG_R, false);
    ArrayXXd A_w_R = laser_pulse_file.read_double(HHG_R, true, false);

    laser_pulse_file.read_header(HHG_I, false);
    ArrayXXd A_w_I = laser_pulse_file.read_double(HHG_I, true, false);

    int N_cols = laser_pulse_file.N_col_;
    int N_rows = laser_pulse_file.N_row_;
    // NEED N_ROWS AND N_COLS TO BE READ IN FROM BIN FILES RATHER THAN CONFIG FILE!!!

    // Combine the two real array that represent the real and complex parts and make a complex array from them
    ArrayXXcd A_w_m = (A_w_R.cast<std::complex<double> >() + (std::complex<double>(0.0, 1.0) * A_w_I));

    //maths_textbook maths(config.path_input_j0());
    // Set up Hankel transform
    DHT ht(N_cols, maths);
    int n_active = N_rows;
    // Backward spectral transform
    ArrayXXcd A_w_r = ArrayXXcd::Zero(n_active, N_cols);
    A_w_r.block(0, 0, n_active, N_cols) = A_w_m;
    for (int ii = 0; ii < n_active; ii++)
        A_w_r.row(ii) = ht.backward(A_w_m.row(ii));

    return A_w_r;
}*/

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

    IO laser_pulse_file;
    laser_pulse_file.read_header(config.path_A_w_R(), false);
    ArrayXXd A_w_R = laser_pulse_file.read_double(config.path_A_w_R(), true, false);
    laser_pulse_file.read_header(config.path_A_w_I(), false);
    ArrayXXd A_w_I = laser_pulse_file.read_double(config.path_A_w_I(), true, false);
    int N_cols = laser_pulse_file.N_col_;
    int N_rows = laser_pulse_file.N_row_;

    laser_pulse_file.read_header(config.path_w_active(), false);
    ArrayXd w_active = laser_pulse_file.read_double(config.path_w_active(), true, false);
    int N_cols_w = laser_pulse_file.N_col_;
    int N_rows_w = laser_pulse_file.N_row_;

    // Combine the two real array that represent the real and complex parts and make a complex array from them
    ArrayXXcd A_w_m = (A_w_R.cast<std::complex<double> >() + (std::complex<double>(0.0, 1.0) * A_w_I));

    maths_textbook maths(config.path_input_j0());
    // Set up Hankel transform
    DHT ht(N_cols, maths);
    int n_active = N_rows;
    // Backward spectral transform
   // ArrayXXcd A_w_r = ArrayXXcd::Zero(n_active, N_cols);
   // A_w_r.block(0, 0, n_active, N_cols) = A_w_m;
   // for (int ii = 0; ii < n_active; ii++)
   //     A_w_r.row(ii) = ht.backward(A_w_m.row(ii));

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

    HH_source hh_source;


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
    for (int i = 1; i < config.n_z() + 1; i++) {
        std::cout << "Propagation Step: " << i << std::endl;
        if (i == 1) {
            prop.z += dz;
            A_w_r = hh_source.GetSource(i, config, maths);
            A_w_r_tmp = prop.block(A_w_r);
            //prop.z += dz;
        } else {
            //A_w_r = GetSource(i, config, maths);
            //A_w_r_tmp = prop.block(A_w_r);
            prop.z += dz;
            prop.nearFieldPropagationStep(dz, A_w_r_tmp);
            A_w_r_tmp = prop.A_w_r;
            //
            A_w_r = hh_source.GetSource(i, config, maths);
            A_w_r_tmp = prop.block(A_w_r);
            A_w_r_tmp += prop.A_w_r;
            //prop.z += dz;
            //A_w_r_tmp += prop.block(GetSource(i, config, maths));
        }

        // Forward spectral transform
        ArrayXXcd A_w_m = ArrayXXcd::Zero(prop.n_k, rkr.n_r);
        for (int ii = 0; ii < prop.n_k; ii++)
            A_w_m.row(ii) = ht.forward(A_w_r_tmp.row(ii));

        // Output step
        std::string propStep = std::to_string(static_cast<unsigned long long>(i));
        std::string HHG_R = dir + prepend + propStep + restOfName_R;
        std::string HHG_I = dir + prepend + propStep + restOfName_I;
        std::string HHG_w = dir + prepend + propStep + restOfName_w;

        //config.step_path(i);
        file_prop_step.overwrite(HHG_R, false);
        file_prop_step.write_header(HHG_R, prop.n_k, rkr.n_r, false);
        file_prop_step.write_double(HHG_R, A_w_m.real(), prop.n_k, rkr.n_r, false);

        file_prop_step.overwrite(HHG_I, false);
        file_prop_step.write_header(HHG_I, prop.n_k, rkr.n_r, false);
        file_prop_step.write_double(HHG_I, A_w_m.imag(), prop.n_k, rkr.n_r, false);

        file_prop_step.overwrite(HHG_w, false);
        file_prop_step.write_header(HHG_w, prop.n_k, 1, false);
        file_prop_step.write_double(HHG_w, prop.segment(w_active), prop.n_k, 1, false);
        // This would output the source and propagated HHG at the current position
    }

return 0;
}
