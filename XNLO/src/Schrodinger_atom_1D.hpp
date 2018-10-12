//
//  Schrodinger_atom_1D.hpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "Schrodinger_atom_1D" encapsulates the interaction of an isolated atom with a strong laser
//  field. The interaction is restricted to a single active electron and spatial dimension.
//

#ifndef __SCHRODINGER_ATOM_1D_HPP_INCLUDED__
#define __SCHRODINGER_ATOM_1D_HPP_INCLUDED__

#include "grid_tw.hpp"
#include "grid_xkx.hpp"
#include "../Eigen/Dense"

using namespace Eigen;

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class definition
/*!
    Modified by Patrick Anderson on 07/05/2015.
    "Schrodinger_atom_1D" encapsulates the interaction of an isolated atom with a strong laser
    field. The interaction is restricted to a single active electron and spatial dimension.
*/
class Schrodinger_atom_1D {
    
public:
    
    // Data
    grid_tw tw;      /*!< The linear temporal grid.*/
    grid_xkx xkx;    /*!< The linear spatial grid.*/
    double alpha;    /*!< The parameter \f$\alpha\f$ is used to scale the Coulomb potential to match the ground state energy of the atom. */
    ArrayXd V_model; /*!< The soft Coulomb potential, \f[V_{Coulomb} = \frac{-1}{\sqrt{\alpha + x^2}}.\f] */
    ArrayXcd wfn_GS; /*!< The ground state wavefunction of the electron at the initial time and as a function of position. */
    ArrayXcd wfn;    /*!< The wavefunction of the electron at a time \f$t\f$ and as a function of position. */
    double energy;   /*!< The energy expectation value of the elctron. */

    int output_wavefunction; /*!< A switch to toggle outputting and saving the electron wavefunction, for every position and time step.*/
    ArrayXXcd wfn_output;    /*!< The electron wavefunction at every position and time step. */
    
    // Functions
    Schrodinger_atom_1D(grid_tw& tw_, double alpha_, int output_wavefunction_);
    void set_GS(int N_it_);
    ArrayXd get_acceleration(int N_it_, double dt_, ArrayXd E_);
    ArrayXd solve_TDSE_PS(int N_it_, std::complex<double> dt_, ArrayXd E_, int e_);
    
};

}

#endif
