//
//  physics_textbook.hpp
//
//  Modified by Patrick Anderson on 03/09/2015.
//  "physics_textbook" is a container for physical constants.
//

#ifndef __PHYSICS_TEXTBOOK_HPP_INCLUDED__
#define __PHYSICS_TEXTBOOK_HPP_INCLUDED__

//namespace HHGP {

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
/*!
Modified by Patrick Anderson on 03/09/2015.
"physics_textbook" is a container for physical constants.
*/
class physics_textbook {

public:
    // Data
    double E_eV;   /*!< Number of eV's in one J */
    double r_0;
    double E_at;
    double l_at;   /*!< Bohr radius */
    double m_at;
    double q_at;   /*!< Electron charge */
    double t_at;
    double w_at;
    double c;      /*!< Speed of light in vacuum */
    double eps_0;  /*!< Permitivity of free space */
    double mu_0;   /*!< Permeability of free space */
    double h;      /*!< Planck constant */
    double h_bar;  /*!< Reduced Planck constant */
    double k_B;    /*!< Boltzmann Constant */

    // Functions
    physics_textbook();
};

//} // HHGP namespace

#endif
