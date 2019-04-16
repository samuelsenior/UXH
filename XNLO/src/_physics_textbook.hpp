//
//  physics_textbook.hpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "physics_textbook" is a container for physical constants.
//

#ifndef __PHYSICS_TEXTBOOK_HPP_INCLUDED__
#define __PHYSICS_TEXTBOOK_HPP_INCLUDED__

namespace XNLO {
    
//------------------------------------------------------------------------------------------------//
//  Class definition
/*!
    Modified by Patrick Anderson on 07/05/2015.
    "physics_textbook" is a container for physical constants.
*/
class physics_textbook {
    
public:
    
    // Data
    double E_at;
    double l_at;
    double m_at;
    double q_at;
    double t_at;
    double w_at;
    double c;
    double eps_0;
    double mu_0;
    double h_bar;
    double k_B;
    
    // Functions
    physics_textbook();
    
};

}

#endif
