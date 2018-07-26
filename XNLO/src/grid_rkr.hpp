//
//  grid_rkr.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "grid_rkr" is a non-uniform radial grid. The spectral counterpart of this grid is evaluated and
//  accessible.
//

#ifndef __GRID_RKR_HPP_INCLUDED__
#define __GRID_RKR_HPP_INCLUDED__

#include "../Eigen/Dense"
#include "maths_textbook.hpp"

using namespace Eigen;

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
/*!
Originally created by Patrick Anderson.
Modified by Samuel Senior on 10/03/2017.
"grid_rkr" is a non-uniform radial grid. The spectral counterpart of this grid is evaluated and
accessible.
*/
class grid_rkr {

public:
    // Data
    ArrayXd r;
    ArrayXd kr;
    int n_r;
    double R;

    // Functions
    grid_rkr(int n_r_, double R_, maths_textbook& maths_);
};

}

#endif

