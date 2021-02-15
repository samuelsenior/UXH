//
//  Schrodinger_atom_1D.hpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "Schrodinger_atom_1D" encapsulates the interaction of an isolated atom with a strong laser
//  field. The interaction is restricted to a single active electron and spatial dimension.
//

#ifndef __SCHRODINGER_ATOM_1D_HPP_INCLUDED__
#define __SCHRODINGER_ATOM_1D_HPP_INCLUDED__

#include "../grid/grid_tw.hpp"
#include "../grid/grid_xkx.hpp"
#include "../maths/maths_textbook.hpp"
#include "../physics/physics_textbook.hpp"

#include "../../Eigen/Dense"
#include "mkl.h"

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class definition
/*!
    Modified by Patrick Anderson on 07/05/2015.
    "Schrodinger_atom_1D" encapsulates the interaction of an isolated atom with a strong laser
    field. The interaction is restricted to a single active electron and spatial dimension.
*/
class Schrodinger_atom_1D {

    maths_textbook maths;
    physics_textbook physics;
    
public:
    
    // Set up transform, MKL
    MKL_LONG mkl_dimension;
    MKL_LONG mkl_length;
    double mkl_scale_factor;
    DFTI_DESCRIPTOR_HANDLE transform;

    // Data
    XNLO::grid_tw tw;      /*!< The linear temporal grid.*/
    grid_xkx xkx;    /*!< The linear spatial grid.*/
    double alpha;    /*!< The parameter \f$\alpha\f$ is used to scale the Coulomb potential to match the ground state energy of the atom. */
    int SAR_N_x;
    double SAR_x_min;
    double SAR_x_max;
    ArrayXd V_model; /*!< The soft Coulomb potential, \f[V_{Coulomb} = \frac{-1}{\sqrt{\alpha + x^2}}.\f] */
    ArrayXcd wfn_GS; /*!< The ground state wavefunction of the electron at the initial time and as a function of position. */
    ArrayXcd wfn;    /*!< The wavefunction of the electron at a time \f$t\f$ and as a function of position. */
    ArrayXcd wfn_boundary;

    double energy;   /*!< The energy expectation value of the electron. */

    bool print;      /*!< Switch to print details to screen. */

    ArrayXcd T;
    ArrayXcd V;

    ArrayXcd x_psi;
    ArrayXcd T2_x_psi;
    ArrayXcd T_V_x_psi;
    ArrayXcd V_T_x_psi;
    ArrayXcd V2_x_psi;
    ArrayXcd x_T2_psi;
    ArrayXcd x_T_V_psi;
    ArrayXcd x_V_T_psi;
    ArrayXcd x_V2_psi;
    ArrayXcd T_x_T_psi;
    ArrayXcd T_x_V_psi;
    ArrayXcd V_x_T_psi;
    ArrayXcd V_x_V_psi;

    ArrayXcd wfn_temp;
    int output_wavefunction; /*!< A switch to toggle outputting and saving the electron wavefunction, for every position and time step.*/
    ArrayXXcd wfn_output;    /*!< The electron wavefunction at every position and time step. */

    ArrayXd acceleration;

    // Functions
    Schrodinger_atom_1D();
    Schrodinger_atom_1D(XNLO::grid_tw& tw_, double alpha_, int SAR_N_x_, double SAR_x_min_, double SAR_x_max_, int output_wavefunction_, bool print=true);
    void set_GS(int N_it_);
    ArrayXd get_acceleration(int N_it_, double dt_, ArrayXd E_);
    ArrayXd solve_TDSE_PS(int N_it_, std::complex<double> dt_, ArrayXd E_, int e_);
    
};

#endif
