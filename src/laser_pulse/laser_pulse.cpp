//
//  laser_pulse.cpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "laser_pulse" contains a time varying electric field. The intital conditions are passed to the
//  constructor and field can be updated as it propagates.
//

#include "mkl.h"
#include <iostream>

#include "../../Eigen/Dense"

#include "laser_pulse.hpp"

#include "../capillary/capillary_fibre.hpp"
#include "../DHT/DHT.hpp"
#include "../gas/keldysh_gas.hpp"
#include "../grid/grid_rkr.hpp"
#include "../grid/grid_tw.hpp"
#include "../IO/IO.hpp"
#include "../physics/physics_textbook.hpp"
#include "../maths/maths_textbook.hpp"

#include "../UPPE/config_settings.hpp"

using namespace Eigen;

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Constructor */
laser_pulse::laser_pulse(double P_av_, double RR_, double FWHM_, double l_0_, double CEO_,
                         double spot_radius_, double ROC_, grid_rkr rkr_, grid_tw tw_,
                         std::string path_A_w_R, std::string path_A_w_I, std::string path_w_active, int read_in_laser_pulse) {

    maths_textbook maths("../../input/J0_zeros.bin");
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
        ArrayXXd A_w_R = laser_pulse_file.read_double(path_A_w_R, true, false);
        laser_pulse_file.read_header(path_A_w_I, false);
        ArrayXXd A_w_I = laser_pulse_file.read_double(path_A_w_I, true, false);
        int N_cols = laser_pulse_file.N_col_;
        int N_rows = laser_pulse_file.N_row_;

        laser_pulse_file.read_header(path_w_active, false);
        ArrayXd w_active = laser_pulse_file.read_double(path_w_active, true, false);
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
        int w_active_min_index = 0;//32;

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
laser_pulse::laser_pulse(grid_rkr rkr_, grid_tw tw_, ArrayXXcd A_w_active,
                         ArrayXd w_active, int w_active_min_index_UPPE,
                         maths_textbook& maths_, physics_textbook& physics_)
                         :
                         maths(maths_),
                         physics(physics_) {
    int N_cols = A_w_active.cols();
    int N_rows = A_w_active.rows();

    ArrayXd temp_linSpace = ArrayXd::LinSpaced(N_rows, -500.0e-15, 500.0e-15);
    ArrayXd boundary = (1 - ((0.5 * maths.pi * temp_linSpace / 500e-15).sin()).pow(50));

    // Combine the two real array that represent the real and complex parts and make a complex array from them
    ArrayXXcd A_w_m = A_w_active;

    // Set up Hankel transform
    DHT ht(N_cols, maths);

    int n_active = N_rows;

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
    temp_3.block(w_active_min_index_UPPE , 0, n_active, N_cols) = temp_2;
    temp_3.block(N_t - n_active - w_active_min_index_UPPE + 1, 0,
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

    //std::cout << "dw_U: " << dw_U << ", dt_X" << dt_X << std::endl;
    //std::cout << "N_T_calc: " << N_t - (N_t_after + N_t_before) << std::endl;

    ArrayXXd E_ = ArrayXXd::Zero(tw_.N_t, N_cols);
    E_ = temp_3.real().block(N_t_before, 0, tw_.N_t, N_cols);

    E = ArrayXXd::Zero(tw_.N_t, rkr_.n_r);

    E = E_;

}

} // XNLO namespace

namespace UPPE {
//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*!
    Constructor
*/
laser_pulse::laser_pulse(){}
laser_pulse::laser_pulse(double p_av_, double rep_, double fwhm_, double l_0_, double ceo_,
                         double waist_, grid_tw& tw_, grid_rkr& rkr_, DFTI_DESCRIPTOR_HANDLE& ft_,
                         DHT& ht_, maths_textbook& maths_,
                         Config_Settings config,
                         int read_in_laser_pulse, double initial_position,
                         double rel_tol_,
                         bool print_)
                        :
                         p_av(p_av_),
                         rep(rep_),
                         fwhm(fwhm_),
                         l_0(l_0_),
                         ceo(ceo_),
                         waist(waist_),
                         tw(tw_),
                         rkr(rkr_),
                         ft(ft_),
                         ht(ht_),
                         maths(maths_),
                         config(config),
                         read_in_laser_pulse(read_in_laser_pulse),
                         initial_position(initial_position),
                         rel_tol(rel_tol_),
                         print(print_) {

    if (read_in_laser_pulse == 0) {

        // Temporal
        p_pk = 0.94 * ((p_av / rep) / fwhm);
        E_pk = std::sqrt((4.0 * p_pk) / (physics.c * physics.eps_0 * maths.pi *
                                              (std::pow(waist, 2))));
        ArrayXd env_t = (-2.77 * (tw.t / fwhm).pow(2)).exp();
        ArrayXd carr_t = (((2.0 * maths.pi * physics.c / l_0) * tw.t) + ceo).cos();

        // Spatial
        ArrayXd env_r = (-(rkr.r / waist).pow(2)).exp();

        // Spatio-temporal
        ArrayXXd E_t_r = (E_pk * env_t * carr_t).matrix() * env_r.matrix().transpose();

        // Apply forward spectral transform
        ArrayXXcd temp_1 = E_t_r.cast<std::complex<double> >();
        for (int ii = 0; ii < rkr.n_r; ii++)
            DftiComputeForward(ft, temp_1.col(ii).data());
        ArrayXXcd temp_2 = temp_1.block(tw.w_active_min_index, 0, tw.n_active, rkr.n_r);
        
        for (int ii = 0; ii < tw.n_active; ii++)
            temp_2.row(ii) = ht.forward(temp_2.row(ii));
        A_w_active = temp_2.block(0, 0, tw.n_active, rkr.n_m);

        electron_density = ArrayXXd::Zero(tw.n_t, rkr.n_r);//tw.n_active, rkr.n_r);

        z_position = 0.0;
    } else if (read_in_laser_pulse == 1) {

        if (print == true) std::cout << "Reading in initial laser pulse from file..." << std::endl;
        // Read in spectral amplitudes from file
        IO laser_pulse_file;
        laser_pulse_file.read_header(config.path_A_w_R_initial(), false);
        ArrayXXd A_w_R = laser_pulse_file.read_double(config.path_A_w_R_initial(), true, false);
        laser_pulse_file.read_header(config.path_A_w_I_initial(), false);
        ArrayXXd A_w_I = laser_pulse_file.read_double(config.path_A_w_I_initial(), true, false);
        int N_cols = laser_pulse_file.N_col_;
        int N_rows = laser_pulse_file.N_row_;
        if (print == true)  std::cout << "N_cols: " << N_cols << ", N_rows: " << N_rows << std::endl;
        // Combine the two real array that represent the real and complex parts and make a complex array from them
        A_w_active = (A_w_R.cast<std::complex<double> >() + (std::complex<double>(0.0, 1.0) * A_w_I));

        electron_density = ArrayXXd::Zero(tw.n_t, rkr.n_r);//tw.n_active, rkr.n_r);
        z_position = initial_position;
        if (print == true) std::cout << "A_w_active.rows(): " << A_w_active.rows() << ", A_w_active.cols(): " << A_w_active.cols() << std::endl;
    }
 }


//------------------------------------------------------------------------------------------------//
/*!
    Propagate sprectral amplitudes A_w_active over the longitudinal step dz_, based on A. Couairon,
    et al., Eur. Phys. J. Special Topics, 199, 5 (2011)
*/
void laser_pulse::propagate(double dz_, capillary_fibre& capillary_, keldysh_gas& gas_) {

    // Call ODE solver
    // (custom, adaptive)
    //static
    double h = dz_ / 5;    // Initial guess
    //static
    double tol = rel_tol;//1e-10;     // Relative error tolerance

    //static
    int i = 0;
    //static
    int j = 0;
    //static
    double z = 0;
    //static
    bool last_step = false;
    while (last_step == false) {
        if (z + (1.1 * h) > dz_) {
            h = dz_ - z;
            last_step = true;
        }
        RK_F_45(h, capillary_, gas_);
        if (e <= tol * Y_5.matrix().norm()) {
            // Tolerence met
            A_w_active = Y_4;
            h *= 1.25;
            z += h;

        } else {
            // Tolerence not met
            h *= 0.5;
            last_step = false;
        }
    }
    // Backward spectral transform
    //static
    ArrayXXcd temp_2 = ArrayXXcd::Zero(tw.n_active, rkr.n_r);
    temp_2.block(0, 0, tw.n_active, rkr.n_m) = A_w_active;
    for (int ii = 0; ii < tw.n_active; ii++)
        temp_2.row(ii) = ht.backward(temp_2.row(ii));
    //static
    ArrayXXcd temp_3 = ArrayXXcd::Zero(tw.n_t, rkr.n_r);
    temp_3.block(tw.w_active_min_index , 0, tw.n_active, rkr.n_r) = temp_2;
    temp_3.block(tw.n_t - tw.n_active - tw.w_active_min_index + 1, 0,
                 tw.n_active, rkr.n_r) = temp_2.conjugate().colwise().reverse();
    //static
    ArrayXd temp_4 = ArrayXd::Zero(tw.n_t);
    for (int ii = 0; ii < rkr.n_r; ii++)
        DftiComputeBackward(ft, temp_3.col(ii).data());
    for (int ii = 0; ii < rkr.n_r; ii++) {
        temp_4 = gas_.electron_density(gas_.ionization_rate(temp_3.col(ii).real()), z_position);
        electron_density.col(ii) = temp_4;//.segment(tw.w_active_min_index, tw.n_active);
    }
    z_position += z;

    // Apply additional shift
    for (int ii = 0; ii < rkr.n_m; ii++) {
        A_w_active.col(ii) *= (std::complex<double>(0, -dz_) * (capillary_.gamma.col(ii) -
                               (tw.w_active / physics.c).cast<std::complex<double> >())).exp();
    }
}

//------------------------------------------------------------------------------------------------//
/*!
    Runge-Kutta-Fehlberg 45 ODE solver
*/
void laser_pulse::RK_F_45(double h_, capillary_fibre& capillary_, keldysh_gas& gas_) {

    // Need to check the statics here are correct and the k_n's are still
    // being assigned correctly etc
    // But if not then declare them static
    // and then intialise them on a seperate line
    //static
    ArrayXXcd k_1 = h_ * RHS_UPPE(0, A_w_active, capillary_, gas_);
    //static
    ArrayXXcd k_2 = h_ * RHS_UPPE(0 + (h_/4), A_w_active + (k_1/4), capillary_, gas_);
    //static
    ArrayXXcd k_3 = h_ * RHS_UPPE(0 + (3*h_/8), A_w_active + (3*k_1/32) + (9*k_2/32), capillary_, gas_);
    //static
    ArrayXXcd k_4 = h_ * RHS_UPPE(0 + (12*h_/13), A_w_active + (1932*k_1/2197) - (7200*k_2/2197) + (7296*k_3/2197), capillary_, gas_);
    //static
    ArrayXXcd k_5 = h_ * RHS_UPPE(0 + h_, A_w_active + (439*k_1/216) - (8*k_2) + (3680*k_3/513) - (845*k_4/4104), capillary_, gas_);
    //static
    ArrayXXcd k_6 = h_ * RHS_UPPE(0 + (h_/2), A_w_active - (8*k_1/27) + (2*k_2) - (3544*k_3/2565) + (1859*k_4/4104) - (11*k_5/40), capillary_, gas_);

    Y_4 = A_w_active + (25*k_1/216) + (1408*k_3/2565) + (2197*k_4/4104) - (k_5/5);
    Y_5 = A_w_active + (16*k_1/135) + (6656*k_3/12825) + (28561*k_4/56430) - (9*k_5/50) + (2*k_6/55);

    e = (Y_5 - Y_4).matrix().norm();
}

//------------------------------------------------------------------------------------------------//
/*!
    Evaluate RHS of UPPE
*/
ArrayXXcd laser_pulse::RHS_UPPE(double dz_, ArrayXXcd temp_1, capillary_fibre& capillary_,
                                keldysh_gas& gas_) {

    // Apply linear propagator
    for (int ii = 0; ii < rkr.n_m; ii++)
        temp_1.col(ii) *= (std::complex<double>(0, -dz_) * (capillary_.gamma.col(ii) -
                           (tw.w_active / physics.c).cast<std::complex<double> >())).exp();

    // Backward spectral transform
    //static
    ArrayXXcd temp_2 = ArrayXXcd::Zero(tw.n_active, rkr.n_r);
    temp_2.block(0, 0, tw.n_active, rkr.n_m) = temp_1;
    for (int ii = 0; ii < tw.n_active; ii++)
        temp_2.row(ii) = ht.backward(temp_2.row(ii));
    //static
    ArrayXXcd temp_3 = ArrayXXcd::Zero(tw.n_t, rkr.n_r);
    temp_3.block(tw.w_active_min_index , 0, tw.n_active, rkr.n_r) = temp_2;
    temp_3.block(tw.n_t - tw.n_active - tw.w_active_min_index + 1, 0,
                 tw.n_active, rkr.n_r) = temp_2.conjugate().colwise().reverse();
    for (int ii = 0; ii < rkr.n_r; ii++)
        DftiComputeBackward(ft, temp_3.col(ii).data());

    // Calculate medium response
    //static
    ArrayXXcd P_w_active_r = ArrayXXcd::Zero(tw.n_active, rkr.n_r);//tw.n_active, rkr.n_r);
    //static
    ArrayXXcd J_w_active_r = ArrayXXcd::Zero(tw.n_active, rkr.n_r);

    for (int ii = 0; ii < rkr.n_r; ii++) {
        P_w_active_r.col(ii) = gas_.nl_polarization(temp_3.col(ii).real());
        //P_NL_t.col(ii) = gas_.nl_polarization(temp_3.col(ii).real());
        J_w_active_r.col(ii) = gas_.current_density(temp_3.col(ii).real(), z_position);
    }

    for (int ii = 0; ii < tw.n_active; ii++) {
        P_w_active_r.row(ii) = ht.forward(P_w_active_r.row(ii));
        J_w_active_r.row(ii) = ht.forward(J_w_active_r.row(ii));
    }
    //static
    ArrayXXcd P_w_active = P_w_active_r.block(0, 0, tw.n_active, rkr.n_m);
    //static
    ArrayXXcd J_w_active = J_w_active_r.block(0, 0, tw.n_active, rkr.n_m);

    // Apply linear propagator
    for (int ii = 0; ii < rkr.n_m; ii++) {
        P_w_active.col(ii) *= (std::complex<double>(0, dz_) * (capillary_.gamma.col(ii) -
                               (tw.w_active / physics.c).cast<std::complex<double> >())).exp();
        J_w_active.col(ii) *= (std::complex<double>(0, dz_) * (capillary_.gamma.col(ii) -
                               (tw.w_active / physics.c).cast<std::complex<double> >())).exp();
    }

    // Return RHS
    //static
    ArrayXXcd output = std::sqrt(physics.mu_0 / physics.eps_0) * ((P_w_active.colwise() *
                       (std::complex<double>(0, 1) * tw.w_active)) - J_w_active);  // Should J_w_active be minus or plus?
    //ArrayXXcd output = std::sqrt(physics.mu_0 / physics.eps_0) * ( - J_w_active);
    //P_NL_w = P_w_active;

    return(output);
}
} // UPPE namespace