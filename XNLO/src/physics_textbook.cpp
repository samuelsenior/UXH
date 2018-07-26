//
//  physics_textbook.cpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "physics_textbook" is a container for physical constants.
//

#include "physics_textbook.hpp"

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Constructor */

physics_textbook::physics_textbook() {
    
    // Atomic units
    E_at = 5.14e11;
    l_at = 5.29177e-11;
    m_at = 9.10938e-31;
    q_at = 1.60218e-19;
    t_at = 2.41888e-17;
    w_at = 4.13e16;
    
    // EM
    c = 2.99792e8;
    eps_0 = 8.85419e-12;
    mu_0 = 1.25664e-6;
    
    // QM
    h_bar = 1.05457e-34;
    
    // Thermodynamics
    k_B = 1.38065e-23;
    
}

}
