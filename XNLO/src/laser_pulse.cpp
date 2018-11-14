//
//  laser_pulse.cpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "laser_pulse" contains a time varying electric field. The intital conditions are passed to the
//  constructor and field can be updated as it propagates.
//

#include "laser_pulse.hpp"
#include "grid_rkr.hpp"
#include "grid_tw.hpp"
#include "physics_textbook.hpp"
#include "maths_textbook.hpp"
#include "../Eigen/Dense"
#include "IO.hpp"
#include "DHT.hpp"

#include "mkl.h"

#include <iostream>

using namespace Eigen;

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Constructor */
laser_pulse::laser_pulse(double P_av_, double RR_, double FWHM_, double l_0_, double CEO_,
                         double spot_radius_, double ROC_, grid_rkr rkr_, grid_tw tw_,
                         std::string path_A_w_R, std::string path_A_w_I, std::string path_w_active, int read_in_laser_pulse) {

    maths_textbook maths("../input/J0_zeros.bin");
    physics_textbook physics;

    if (read_in_laser_pulse == 0) {
        // If we aren't reading in a laser pulse from file then just make a Gaussian pulse
        // from the input and/or default parameters

        // Time
        double P_peak = 0.94 * ((P_av_ / RR_) / FWHM_);
        double E_peak = std::sqrt((4 * P_peak) /
                                  (physics.c * physics.eps_0 * maths.pi * (std::pow(spot_radius_, 2))));
        ArrayXd envelope_t = (-2.77 * (tw_.t / FWHM_).pow(2)).exp();
        
        // Space
        ArrayXd envelope_x = (-(rkr_.r / spot_radius_).pow(2)).exp();
        
        // Space-time
        E = ArrayXXd::Zero(tw_.N_t, rkr_.n_r);
        double f_0 = physics.c / l_0_;
        double w_0 = 2 * maths.pi * f_0;
        ArrayXd phi = (maths.pi * rkr_.r.pow(2) / (l_0_ * ROC_)) + CEO_;
        
        for (int ii = 0; ii < rkr_.n_r; ii++) {
            E.col(ii) = E_peak * envelope_x(ii) * envelope_t * ((w_0 * tw_.t) + phi(ii)).cos();
        }
    } else if (read_in_laser_pulse == 1) {
        // Read in spectral amplitudes from file and transfrom them into the electric field
        // at different radial positions

        IO laser_pulse_file;
        laser_pulse_file.read_header(path_A_w_R, false);
        ArrayXXd A_w_R = laser_pulse_file.read_double(path_A_w_R);
        laser_pulse_file.read_header(path_A_w_I, false);
        ArrayXXd A_w_I = laser_pulse_file.read_double(path_A_w_I);
        int N_cols = laser_pulse_file.N_col_;
        int N_rows = laser_pulse_file.N_row_;

        laser_pulse_file.read_header(path_w_active, false);
        ArrayXd w_active = laser_pulse_file.read_double(path_w_active);
        int N_cols_w = laser_pulse_file.N_col_;
        int N_rows_w = laser_pulse_file.N_row_;

        ArrayXd temp_linSpace = ArrayXd::LinSpaced(N_rows, -500.0e-15, 500.0e-15);
        ArrayXd boundary = (1 - ((0.5 * maths.pi * temp_linSpace / 500e-15).sin()).pow(50));
        //for (int i = 0; i < N_cols; i++) {
        //    A_w_R.col(i) *= boundary;
        //    A_w_I.col(i) *= boundary;
        //}

        // Combine the two real array that represent the real and complex parts and make a complex array from them
        ArrayXXcd A_w_m = (A_w_R.cast<std::complex<double> >() + (std::complex<double>(0.0, 1.0) * A_w_I));

        // Set up Hankel transform
        DHT ht(N_cols, maths);

        int n_active = N_rows;
        int w_active_min_index = 32;

        double dw_U = std::abs(w_active(0) - w_active(1));
        double dt_X = std::abs(tw_.t(0) - tw_.t(1));
        double T_u = 2.0*maths.pi / dw_U;

        int N_t = int(T_u / dt_X);

        // Backward spectral transform
        ArrayXXcd temp_2 = ArrayXXcd::Zero(n_active, N_cols);
        temp_2.block(0, 0, n_active, N_cols) = A_w_m;
        for (int ii = 0; ii < n_active; ii++)
            temp_2.row(ii) = ht.backward(A_w_m.row(ii));
        ArrayXXcd temp_3 = ArrayXXcd::Zero(N_t, N_cols);
        temp_3.block(w_active_min_index , 0, n_active, N_cols) = temp_2;
        temp_3.block(N_t - n_active - w_active_min_index + 1, 0,
                     n_active, N_cols) = temp_2.conjugate().colwise().reverse();

        // Set up transform, MKL
        MKL_LONG dimension = 1;
        MKL_LONG length = N_t;
        // Scale factor is 1/N_t, where N_t is the number of time steps in
        // the UPPE simulation being read in
        double scale_factor = 1.0 / 16384.0;
        
        DFTI_DESCRIPTOR_HANDLE transform;
        DftiCreateDescriptor(&transform, DFTI_DOUBLE, DFTI_COMPLEX, dimension, length);
        DftiSetValue(transform, DFTI_BACKWARD_SCALE, scale_factor);
        DftiCommitDescriptor(transform);
        // Transform to temporal amplitudes
        for (int ii = 0; ii < N_cols; ii++)
             DftiComputeBackward(transform, temp_3.col(ii).data());

        //Now need to take a slice...
        double T_U_min = -1.0*T_u / 2.0;
        double T_U_max = 1.0*T_u / 2.0;

        int N_t_before = int(std::abs(T_U_min - tw_.t_min) / dt_X);
        int N_t_after = int(std::abs(T_U_max - tw_.t_max) / dt_X);

        //std::cout << "T_U_min: " << T_U_min << ", T_U_max: " << T_U_max << ", N_t_before: " << N_t_before << ", N_t_after: " << N_t_after << std::endl;
        std::cout << "dw_U: " << dw_U << ", dt_X" << dt_X << std::endl;
        std::cout << "N_T_calc: " << N_t - (N_t_after + N_t_before) << std::endl;

        ArrayXXd E_ = ArrayXXd::Zero(tw_.N_t, N_cols);
        E_ = temp_3.real().block(N_t_before, 0, tw_.N_t, N_cols);

        E = ArrayXXd::Zero(tw_.N_t, rkr_.n_r);

        // Want to sample E_ at the different points of the rkr grid that's being used
        // If we assume that the rkr grid initialised in XNLO has the same diameter as that used
        // in UPPE then E(0) is sampled at the closest point to the axis
        // E(N) is sampled at the furthest point, with N being the number of atoms-1
        //
        //E.col(0) = E_.col(0);
        //E.col(1) = E_.col(6);
        //E.col(2) = E_.col(13);
        //E.col(3) = E_.col(19);

        E = E_;

    }  
}

/*! Constructor */
laser_pulse::laser_pulse(grid_rkr rkr_, grid_tw tw_, ArrayXXcd A_w_active, ArrayXd w_active) {

    maths_textbook maths("../input/J0_zeros.bin");
    physics_textbook physics;

    int N_cols = A_w_active.cols();
    int N_rows = A_w_active.rows();

    //laser_pulse_file.read_header(path_w_active, false);
    //ArrayXd w_active = laser_pulse_file.read_double(path_w_active);
    //int N_cols_w = laser_pulse_file.N_col_;
    //int N_rows_w = laser_pulse_file.N_row_;

    ArrayXd temp_linSpace = ArrayXd::LinSpaced(N_rows, -500.0e-15, 500.0e-15);
    ArrayXd boundary = (1 - ((0.5 * maths.pi * temp_linSpace / 500e-15).sin()).pow(50));
    //for (int i = 0; i < N_cols; i++) {
    //    A_w_R.col(i) *= boundary;
    //    A_w_I.col(i) *= boundary;
    //}

    // Combine the two real array that represent the real and complex parts and make a complex array from them
    ArrayXXcd A_w_m = A_w_active;

    // Set up Hankel transform
    DHT ht(N_cols, maths);

    int n_active = N_rows;
    int w_active_min_index = 32;

    double dw_U = std::abs(w_active(0) - w_active(1));
    double dt_X = std::abs(tw_.t(0) - tw_.t(1));
    double T_u = 2.0*maths.pi / dw_U;

    int N_t = int(T_u / dt_X);

    // Backward spectral transform
    ArrayXXcd temp_2 = ArrayXXcd::Zero(n_active, N_cols);
    temp_2.block(0, 0, n_active, N_cols) = A_w_m;
    for (int ii = 0; ii < n_active; ii++)
        temp_2.row(ii) = ht.backward(A_w_m.row(ii));
    ArrayXXcd temp_3 = ArrayXXcd::Zero(N_t, N_cols);
    temp_3.block(w_active_min_index , 0, n_active, N_cols) = temp_2;
    temp_3.block(N_t - n_active - w_active_min_index + 1, 0,
                 n_active, N_cols) = temp_2.conjugate().colwise().reverse();

    // Set up transform, MKL
    MKL_LONG dimension = 1;
    MKL_LONG length = N_t;
    // Scale factor is 1/N_t, where N_t is the number of time steps in
    // the UPPE simulation being read in
    double scale_factor = 1.0 / 16384.0;
    
    DFTI_DESCRIPTOR_HANDLE transform;
    DftiCreateDescriptor(&transform, DFTI_DOUBLE, DFTI_COMPLEX, dimension, length);
    DftiSetValue(transform, DFTI_BACKWARD_SCALE, scale_factor);
    DftiCommitDescriptor(transform);
    // Transform to temporal amplitudes
    for (int ii = 0; ii < N_cols; ii++)
         DftiComputeBackward(transform, temp_3.col(ii).data());

    //Now need to take a slice...
    double T_U_min = -1.0*T_u / 2.0;
    double T_U_max = 1.0*T_u / 2.0;

    int N_t_before = int(std::abs(T_U_min - tw_.t_min) / dt_X);
    int N_t_after = int(std::abs(T_U_max - tw_.t_max) / dt_X);

    //std::cout << "T_U_min: " << T_U_min << ", T_U_max: " << T_U_max << ", N_t_before: " << N_t_before << ", N_t_after: " << N_t_after << std::endl;
    std::cout << "dw_U: " << dw_U << ", dt_X" << dt_X << std::endl;
    std::cout << "N_T_calc: " << N_t - (N_t_after + N_t_before) << std::endl;

    ArrayXXd E_ = ArrayXXd::Zero(tw_.N_t, N_cols);
    E_ = temp_3.real().block(N_t_before, 0, tw_.N_t, N_cols);

    E = ArrayXXd::Zero(tw_.N_t, rkr_.n_r);

    E = E_;

}

}
