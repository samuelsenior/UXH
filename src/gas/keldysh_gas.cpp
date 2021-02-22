//
//  keldysh_gas.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "keldysh_gas" contains the medium response model.
//

#include "keldysh_gas.hpp"
#include "../physics/physics_textbook.hpp"
#include "../grid/grid_tw.hpp"
#include <mkl.h>
#include "../../Eigen/Dense"
#include "../maths/maths_textbook.hpp"
#include <cmath>

#include <iostream>

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Constructor */
keldysh_gas::keldysh_gas() {}
keldysh_gas::keldysh_gas(double press_,
                         std::string gas_pressure_profile_, bool print_) :
                         gas_pressure_profile(gas_pressure_profile_),
                         print(print_) {

    atom_density_max = press_ * 1.0e5 / (physics.k_B * 300.0);  // [atoms/m^3]
    z_max = 0.07;
    inlet_1 = 0.02;
    inlet_2 = z_max - 0.02;
    transitionLength = 0.001;

    // Ionization parameters, argon
    U = 15.76;  // [eV]
    C_kl = 0.95;
    n_star = 0.93;
    kappa = std::sqrt(U / 13.60);

    // Witchcraft to switch around which pressure profile function atom_density
    // points to.
    if (gas_pressure_profile == "capillary") {
        if (print) std::cout << "keldysh_gas: capillary gas pressure profile chosen!" << std::endl;
        set_atom_density_ptr(&keldysh_gas::capillary_pressure_profile);
    } else if (gas_pressure_profile == "constant") {
        if (print) std::cout << "keldysh_gas: constant gas pressure profile chosen!" << std::endl;
        set_atom_density_ptr(&keldysh_gas::constant_pressure_profile);
    } else {
        std::cout << "Error, gas pressure profile not set to recognised profile, defaulting to capillary!" << std::endl;
        set_atom_density_ptr(&keldysh_gas::capillary_pressure_profile);
    }
}
keldysh_gas::keldysh_gas(double press_, grid_tw& tw_, DFTI_DESCRIPTOR_HANDLE& ft_, maths_textbook& maths_,
                         std::string gas_pressure_profile_, bool print_) :
                         maths(maths_), tw(tw_), ft(ft_),
                         gas_pressure_profile(gas_pressure_profile_),
                         print(print_) {

    atom_density_max = press_ * 1.0e5 / (physics.k_B * 300.0);  // [atoms/m^3]
    z_max = 0.07;
    inlet_1 = 0.02;
    inlet_2 = z_max - 0.02;
    transitionLength = 0.001;

    // Ionization parameters, argon
    U = 15.76;  // [eV]
    C_kl = 0.95;
    n_star = 0.93;
    kappa = std::sqrt(U / 13.60);

    // Witchcraft to switch around which pressure profile function atom_density
    // points to.
    if (gas_pressure_profile == "capillary") {
        if (print) std::cout << "keldysh_gas: capillary gas pressure profile chosen!" << std::endl;
        set_atom_density_ptr(&keldysh_gas::capillary_pressure_profile);
    } else if (gas_pressure_profile == "constant") {
        if (print) std::cout << "keldysh_gas: constant gas pressure profile chosen!" << std::endl;
        set_atom_density_ptr(&keldysh_gas::constant_pressure_profile);
    }
}

// Witchcraft to switch around which pressure profile function atom_density
// points to.
void keldysh_gas::set_atom_density_ptr(atom_density_func_ptr ptr) {
    atom_density_func = ptr;
}
double keldysh_gas::atom_density(double z) {
    return (this->*atom_density_func)(z);
}

// Pressure profile functions
double keldysh_gas::capillary_pressure_profile(double z) {
    if (z >= inlet_1 && z <= inlet_2) {
        // Constant max value
        return atom_density_max;
    } else if (z < inlet_1 - transitionLength) {
        // Ramp function up to 80%
        return atom_density_max*(z/(inlet_1 - transitionLength)) * 0.8;
    } else if (z >= inlet_1 - transitionLength && z < inlet_1) {
        // Step up from 80% to 100%
        return atom_density_max * (0.8 + 0.2*(z - (inlet_1 - transitionLength)) / (transitionLength));
    } else if (z > inlet_2 && z <= inlet_2 + transitionLength) {
        // Step down from 100% to 80%
        return atom_density_max * (1 - ((z - inlet_2)/(transitionLength)) * 0.2);
    } else if (z > inlet_2 + transitionLength && z <= z_max) {
        // Ramp down from 80% to 0%
        return atom_density_max * (1 - (z - (inlet_2 + transitionLength)) / (z_max - (inlet_2 + transitionLength))) * 0.8;
    } else {
        return 0.0;
    }
}

double keldysh_gas::constant_pressure_profile(double z) {
        // Constant max value
        return atom_density_max;
}

//double keldysh_gas::atom_density(double z) {
//    if (z >= inlet_1 && z <= inlet_2) {
//        // Constant max value
//        return atom_density_max;
//    } else if (z < inlet_1 - transitionLength) {
//        // Ramp function up to 80%
//        return atom_density_max*(z/(inlet_1 - transitionLength)) * 0.8;
//    } else if (z >= inlet_1 - transitionLength && z < inlet_1) {
//        // Step up from 80% to 100%
//        return atom_density_max * (0.8 + 0.2*(z - (inlet_1 - transitionLength)) / (transitionLength));
//    } else if (z > inlet_2 && z <= inlet_2 + transitionLength) {
//        // Step down from 100% to 80%
//        return atom_density_max * (1 - ((z - inlet_2)/(transitionLength)) * 0.2);
//    } else if (z > inlet_2 + transitionLength && z <= z_max) {
//        // Ramp down from 80% to 0%
//        return atom_density_max * (1 - (z - (inlet_2 + transitionLength)) / (z_max - (inlet_2 + transitionLength))) * 0.8;
//    } else {
//        return 0.0;
//    }
//}

//------------------------------------------------------------------------------------------------//
/*! Evaluate nonlinear polarization for active frequencies */
ArrayXcd keldysh_gas::nl_polarization(ArrayXd E_t_) {

    // Weak, zeros
    ArrayXcd output_zeros = ArrayXcd::Zero(tw.n_t);  // Original, returns zeros so no nonlinear polarisation
    return((output_zeros).segment(tw.w_active_min_index, tw.n_active));
}

//------------------------------------------------------------------------------------------------//
/*! Calulate ionization rate (Popov, 2004) */
ArrayXd keldysh_gas::ionization_rate(ArrayXd E_t_) {

    ArrayXd F_t = (E_t_ / (std::pow(kappa, 3.0) * physics.E_at)).abs();
    ArrayXd output = physics.w_at * std::pow(kappa, 2.0) * std::sqrt(3.0 / maths.pi) *
                     std::pow(C_kl, 2.0) * std::pow(2.0, 2.0 * n_star) * F_t.pow(1.5 - (2.0 * n_star)) *
                     (-2.0 / (3.0 * F_t)).exp();
    for (int ii = 0; ii < tw.n_t; ii++)     // Remove NaN
        if (std::isnan(output(ii)) == true) output(ii) = 0;

    return(output);
}

//------------------------------------------------------------------------------------------------//
/*! Calculate free electron density (solve rate equations) */
ArrayXd keldysh_gas::electron_density(ArrayXd W_t_, double z) {
    ArrayXd output =  atom_density(z) * (1.0 - (-maths.cumtrapz(tw.t, W_t_)).exp());
    return(output);
}

//------------------------------------------------------------------------------------------------//
/*! Evaluate Current density for active frequencies */
ArrayXcd keldysh_gas::current_density(ArrayXd E_t_, double z) {

    // Prerequisites
    ArrayXd W_t = ionization_rate(E_t_);
    ArrayXd rho_t = electron_density(W_t, z);

    // Eliminate derivatives in governing equation by FT
    ArrayXcd B = (-std::pow(physics.q_at, 2.0) * rho_t * E_t_ / physics.m_at).
                 cast<std::complex<double> >();
    ArrayXd temp_1 = rho_t * W_t * physics.q_at * U * E_t_ / E_t_.abs2();
    for (int ii = 0; ii < tw.n_t; ii++)     // Remove NaN
        if (std::isnan(temp_1(ii)) == true) temp_1(ii) = 0;
    ArrayXcd C = temp_1.cast<std::complex<double> >();
    DftiComputeForward(ft, B.data()); DftiComputeForward(ft, C.data());

    // Retain active components
    ArrayXcd output = (B.segment(tw.w_active_min_index, tw.n_active) /
                       (std::complex<double>(0.0, -1.0) * tw.w_active)) -
                      (physics.eps_0 * physics.c *
                       C.segment(tw.w_active_min_index, tw.n_active));
    return(output);
}
