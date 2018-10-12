#include <iostream>
#include <string>

#include "Eigen/Dense"

#include "HH_source.hpp"
#include "IO.hpp"
#include "config_settings.hpp"
#include "maths_textbook.hpp"
#include "DHT.hpp"

using namespace Eigen;

HH_source::HH_source () {}

ArrayXXcd HH_source::GetSource(int fileNumber,
                           Config_Settings config, maths_textbook maths) {

    std::string dir = "../../../Results/UPPE-XNLO/Convergence/nz-100/data/";
    std::string prepend = "001_";
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
}