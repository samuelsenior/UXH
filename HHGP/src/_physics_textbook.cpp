//
//  physics_textbook.cpp
//
//  Modified by Patrick Anderson on 03/09/2015.
//  "physics_textbook" is a container for physical constants.
//

#include "physics_textbook.hpp"

//namespace HHGP {

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Constructor */
physics_textbook::physics_textbook() {

    // J to eV conversion
    E_eV = 6.241509e18;   // Number of eV's in 1J

    // Classical
    r_0 = 2.8179e-15;  // Classical electron radius
    
    // Atomic units
    E_at = 5.14e11;
    l_at = 5.29177e-11;   // Bohr radius
    m_at = 9.10938e-31;
    q_at = 1.60218e-19;   // Electron charge
    t_at = 2.41888e-17;
    w_at = 4.13e16;
    
    // EM
    c = 2.99792e8;        // Speed of light in vacuum
    eps_0 = 8.85419e-12;  // Permitivity of free space
    mu_0 = 1.25664e-6;    // Permeability of free space
    
    // QM
    h = 6.63e-34;         // Planck constant
    h_bar = 1.05457e-34;  // Reduced Planck constant
    
    // Thermodynamics
    k_B = 1.38065e-23;    // Boltzmann Constant
}

//} // HHGP namespace