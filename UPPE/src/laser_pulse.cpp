//
//  laser_pulse.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "laser_pulse" contains the active spectral amplitudes and governs their propagation over
//  longitudinal step dz.
//

#include "laser_pulse.hpp"
#include "grid_tw.hpp"
#include "grid_rkr.hpp"
#include <mkl.h>
#include "DHT.hpp"
#include "physics_textbook.hpp"
#include "maths_textbook.hpp"
#include "Eigen/Dense"
#include "capillary_fibre.hpp"
#include "keldysh_gas.hpp"

#include <iostream>

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*!
    Constructor
*/
laser_pulse::laser_pulse(double p_av_, double rep_, double fwhm_, double l_0_, double ceo_,
                         double waist_, grid_tw& tw_, grid_rkr& rkr_, DFTI_DESCRIPTOR_HANDLE& ft_,
                         DHT& ht_, maths_textbook& maths_)
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
                         maths(maths_) {

    // Temporal
    p_pk = 0.94 * ((p_av / rep) / fwhm);
    E_pk = std::sqrt((4 * p_pk) / (physics.c * physics.eps_0 * maths.pi *
                                          (std::pow(waist, 2))));
    ArrayXd env_t = (-2.77 * (tw.t / fwhm).pow(2)).exp();
    ArrayXd carr_t = (((2 * maths.pi * physics.c / l_0) * tw.t) + ceo).cos();

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

    //P_NL_r_t = ArrayXXcd::Zero(tw.n_t, rkr.n_r);
    //P_NL_m_t = ArrayXXcd::Zero(tw.n_t, rkr.n_m);
    //P_NL_w = ArrayXXcd::Zero(tw.n_active, rkr.n_m);

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
    double tol = 1e-4;     // Relative error tolerance

    //static
    int i = 0;
    //static
    int j = 0;

    atom_density_max = gas_.atom_density_max;// = press_ * 1.0e5 / (physics.k_B * 300.0);  // [atoms/m^3]

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
