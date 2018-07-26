//
//  dipole_moment.hpp
//
//  Created by Samuel Senior on 10/03/2017.
//  "dipole_moment" wraps around the dipole moment calculations and output files.
//

#ifndef __DIPOLE_MOMENT_HPP_INCLUDED__
#define __DIPOLE_MOMENT_HPP_INCLUDED__

#include "Eigen/Dense"

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
/*! Created by Samuel Senior on 10/03/2017.
    "dipole_moment" wraps around the dipole moment calculations and output files.
*/
class Dipole_moment {
    // Data
    ArrayXXd dipole_data;

    // Functions

public:
    // Data

    // Functions
    Dipole_moment();
    void update_dipole_moment();
    ArrayXXd get_moment();
};

#endif
