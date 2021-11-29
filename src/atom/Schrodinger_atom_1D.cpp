//
//  Schrodinger_atom_1D.hpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "Schrodinger_atom_1D" encapsulates the interaction of an isolated atom with a strong laser
//  field. The interaction is restricted to a single active electron and spatial dimension. Atomic
//  units.
//

#include "Schrodinger_atom_1D.hpp"
#include "../maths/maths_textbook.hpp"
#include "../physics/physics_textbook.hpp"
#include "../grid/grid_tw.hpp"
#include "../grid/grid_xkx.hpp"

#include "../../Eigen/Dense"
#include "mkl.h"

#include <iostream>

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Class constructor. */
Schrodinger_atom_1D::Schrodinger_atom_1D() {}

/*! Class constructor. */
Schrodinger_atom_1D::Schrodinger_atom_1D(XNLO::grid_tw& tw_, double alpha_,
                                         int SAR_N_x_, double SAR_x_min_, double SAR_x_max_,
                                         int output_wavefunction_, bool print_)
                                         :
                                         tw(tw_), 
                                         SAR_N_x(SAR_N_x_), SAR_x_min(SAR_x_min_), SAR_x_max(SAR_x_max_),
                                         print(print_) {

    maths_textbook maths;
    physics_textbook physics;

    energy = 9999.9;

    xkx = grid_xkx(SAR_N_x, SAR_x_min, SAR_x_max);

    // Set up transform, MKL
    mkl_dimension = 1;
    mkl_length = xkx.N_x;
    mkl_scale_factor = 1.0 / xkx.N_x;
    DftiCreateDescriptor(&transform, DFTI_DOUBLE, DFTI_COMPLEX, mkl_dimension, mkl_length);
    DftiSetValue(transform, DFTI_BACKWARD_SCALE, mkl_scale_factor);
    DftiCommitDescriptor(transform);

    // Model potential (Soft Coulomb)
    alpha = alpha_;
    V_model = -1 / (alpha + xkx.x.pow(2)).sqrt();

    output_wavefunction = output_wavefunction_;

    if (output_wavefunction == 1) { wfn_output = ArrayXXcd::Zero(tw.N_t, SAR_N_x); }
    else { wfn_output = ArrayXXcd::Zero(0, 0); }

    T = (0.5 * xkx.kx.pow(2)).cast<std::complex<double> >();
    V = ArrayXcd::Zero(xkx.N_x);

    x_psi = ArrayXcd::Zero(xkx.N_x);
    T2_x_psi = ArrayXcd::Zero(xkx.N_x);
    T_V_x_psi = ArrayXcd::Zero(xkx.N_x);
    V_T_x_psi = ArrayXcd::Zero(xkx.N_x);
    V2_x_psi = ArrayXcd::Zero(xkx.N_x);
    x_T2_psi = ArrayXcd::Zero(xkx.N_x);
    x_T_V_psi = ArrayXcd::Zero(xkx.N_x);
    x_V_T_psi = ArrayXcd::Zero(xkx.N_x);
    x_V2_psi = ArrayXcd::Zero(xkx.N_x);
    T_x_T_psi = ArrayXcd::Zero(xkx.N_x);
    T_x_V_psi = ArrayXcd::Zero(xkx.N_x);
    V_x_T_psi = ArrayXcd::Zero(xkx.N_x);
    V_x_V_psi = ArrayXcd::Zero(xkx.N_x);

    // Ground state wavefunction
    wfn_GS = ArrayXcd::Zero(xkx.N_x);
    wfn = ArrayXcd::Zero(xkx.N_x);
    wfn_temp = ArrayXcd::Zero(xkx.N_x);
    // Initiallise wavefunction boundary
    wfn_boundary = (1 - ((0.5 * maths.pi * xkx.x /
                    xkx.x_max).sin()).pow(50)).cast<std::complex<double> >();
    set_GS(20000);

}

//------------------------------------------------------------------------------------------------//
/*! Find the GS wavefunction by imaginary time propagation, also display the energy. The time-dependent
    Schrodinger equation (TDSE) in atomic units is given as
    \f[
        i\frac{\partial \psi}{\partial t} = \hat{H}\psi.
    \f]
    Transforming to imaginary time causes the TDSE to become a diffusion equation,
    \f[
        t = -i\tau.
    \f]
    As time increases, the wavefunction then converges to the ground state,
    \f[
        \psi(x, t) \rightarrow e^{-\tau E_0}c_0\phi_0.
    \f]
    P. Bader et al. Solving the Schrodinger eigenvalue problem by imaginary time
    propagation techniques using splitting methods with complex coefficients, J. Chem. Phys., 139, 2013


    The energy expectation value is given by
    \f[
        \langle E \rangle = \int \psi^*\hat{K}\psi dx + \int \psi^*\hat{V}\psi dx.
    \f]
*/
void Schrodinger_atom_1D::set_GS(int N_it_) {

    //maths_textbook maths;

    // Create an imaginary time step and zero field
    std::complex<double> dt(0.0, -0.01);
    ArrayXd E_field_zeros = ArrayXd::Zero(N_it_);

    // Seed
    double FWHM = 30.0;
    double k = 1.0 / (2.0 * std::pow(FWHM / 2.35, 2));
    wfn_GS = (-k * xkx.x.pow(2)).exp().cast<std::complex<double> >();
    wfn_GS *= std::pow(maths.trapz(xkx.x, wfn_GS.abs2()), -0.5);

    // Solve TDSE
    ArrayXd acceleration_temp = solve_TDSE_PS(N_it_, dt, E_field_zeros, 0);
    wfn_GS = wfn;
    if (print == true) {
        std::cout << "Energy: "<< energy * 27.211 << " eV" << std::endl;
    }
}

//------------------------------------------------------------------------------------------------//
/*! Find the electron acceleration generated by an arbitary field. This is achieved by solving the TDSE using the pseudo-spectral method and then using the wavefunction
    in conjunction with the Ehrefest theorem to obtain the electron acceleration by
    \f[
        a(t) = \langle\left[\hat{H}, \left[\hat{H}, \hat{x}\right]\right]\rangle.
    \f]
    M. B. Gaarde and K. J. Schafer. Theory of attosecond pulse generation. Springer series in Optical Sciences, 177:11-31, 2013.
*/
ArrayXd Schrodinger_atom_1D::get_acceleration(int N_it_, double dt_, ArrayXd E_) {

    // Solve TDSE
    acceleration = solve_TDSE_PS(N_it_,
                        std::complex<double> (dt_ / physics.t_at, 0.0), E_ / physics.E_at, 1);

    return(acceleration);
}


//------------------------------------------------------------------------------------------------//
/*! TDSE solver (pseudo-spectral method). The time propagator of the pseudo-spectral method is given as
    \f[
        \psi(x, t+\Delta t) = e^{\frac{-i\hat{k}\Delta t}{2}}e^{-i\hat{V}\Delta t}e^{\frac{-i\hat{K}\Delta t}{2}}\psi(x, t).
    \f]
    P. L. DeVries and J. Hasbun. A first course in computational physics. Jones and Bartlett, 2nd edition, 2010.
*/
ArrayXd Schrodinger_atom_1D::solve_TDSE_PS(int N_it_, std::complex<double> dt_,
                                           ArrayXd E_, int e_) {

    // Initialise wavefunction, output
    wfn = wfn_GS;
    ArrayXd output = ArrayXd::Zero(N_it_);

    // Main loop
    int j = 0;
    int step_print = 10000;

    for (int ii = 0; ii < N_it_; ii++) {
        if (j*step_print == ii && print == true) {
            std::cout << "step: " << ii << std::endl;
            j++;
        }

        // Update operator
        V_model = -1 / (alpha + xkx.x.pow(2)).sqrt();
        V = (V_model + (xkx.x * E_(ii))).cast<std::complex<double> >();

        // Propagate
        DftiComputeForward(transform, wfn.data());
        wfn *= (std::complex<double>(0, -0.5) * T * dt_).exp();
        DftiComputeBackward(transform, wfn.data());

        wfn *= (std::complex<double>(0, -1) * V * dt_).exp();

        DftiComputeForward(transform, wfn.data());
        wfn *= (std::complex<double>(0, -0.5) * T * dt_).exp();
        DftiComputeBackward(transform, wfn.data());

        wfn *= wfn_boundary;

        // Calculate expectation values
        if (e_ == 0) {

            // Energy
            wfn_temp = wfn;
            DftiComputeForward(transform, wfn_temp.data());
            wfn_temp *= T;
            DftiComputeBackward(transform, wfn_temp.data());
            energy = maths.trapz(xkx.x, (wfn.conjugate() * wfn_temp).real()) +
                     maths.trapz(xkx.x, (wfn.conjugate() * V * wfn).real());
            output(ii) = energy;

            // Re-normalize
            wfn *= std::pow(maths.trapz(xkx.x, wfn.abs2()), -0.5);

        } else if (e_ == 1) {

            x_psi = xkx.x * wfn;

            T2_x_psi = x_psi;
            DftiComputeForward(transform, T2_x_psi.data());
            T2_x_psi *= T;
            T2_x_psi *= T;
            DftiComputeBackward(transform, T2_x_psi.data());

            T_V_x_psi = V * x_psi;
            DftiComputeForward(transform, T_V_x_psi.data());
            T_V_x_psi *= T;
            DftiComputeBackward(transform, T_V_x_psi.data());

            V_T_x_psi = x_psi;
            DftiComputeForward(transform, V_T_x_psi.data());
            V_T_x_psi *= T;
            DftiComputeBackward(transform, V_T_x_psi.data());
            V_T_x_psi *= V;

            V2_x_psi = V * V * x_psi;

            x_T2_psi = wfn;
            DftiComputeForward(transform, x_T2_psi.data());
            x_T2_psi *= T;
            x_T2_psi *= T;
            DftiComputeBackward(transform, x_T2_psi.data());
            x_T2_psi *= xkx.x;

            x_T_V_psi = V * wfn;
            DftiComputeForward(transform, x_T_V_psi.data());
            x_T_V_psi *= T;
            DftiComputeBackward(transform, x_T_V_psi.data());
            x_T_V_psi *= xkx.x;

            x_V_T_psi = wfn;
            DftiComputeForward(transform, x_V_T_psi.data());
            x_V_T_psi *= T;
            DftiComputeBackward(transform, x_V_T_psi.data());
            x_V_T_psi *= xkx.x * V;

            x_V2_psi = xkx.x * V * V *wfn;

            T_x_T_psi = wfn;
            DftiComputeForward(transform, T_x_T_psi.data());
            T_x_T_psi *= T;
            DftiComputeBackward(transform, T_x_T_psi.data());
            T_x_T_psi *= xkx.x;
            DftiComputeForward(transform, T_x_T_psi.data());
            T_x_T_psi *= T;
            DftiComputeBackward(transform, T_x_T_psi.data());

            T_x_V_psi = xkx.x * V * wfn;
            DftiComputeForward(transform, T_x_V_psi.data());
            T_x_V_psi *= T;
            DftiComputeBackward(transform, T_x_V_psi.data());

            V_x_T_psi = wfn;
            DftiComputeForward(transform, V_x_T_psi.data());
            V_x_T_psi *= T;
            DftiComputeBackward(transform, V_x_T_psi.data());
            V_x_T_psi *= V * xkx.x;

            V_x_V_psi = V * xkx.x * V * wfn;

            wfn_temp = T2_x_psi + T_V_x_psi + V_T_x_psi + V2_x_psi + x_T2_psi + x_T_V_psi
                       + x_V_T_psi + x_V2_psi
                       -2.0*T_x_T_psi - 2.0*T_x_V_psi - 2.0*V_x_T_psi - 2.0*V_x_V_psi;

            output(ii) = -1.0*maths.trapz(xkx.x, (wfn.conjugate() * wfn_temp).real());

            if (output_wavefunction == 1) { wfn_output.row(ii) = wfn; }

        } else {
            // Spare
            std::cout << "Invalid expectation value!" << std::endl;
        }
    }
    return(output);
}




