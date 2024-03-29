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

#include "../../Eigen/Dense"
#include "../maths/maths_textbook.hpp"

using namespace Eigen;

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

	maths_textbook maths;

public:
    // Data
    ArrayXd r;
    ArrayXd kr;
    int n_r;
    double R;
    int n_m;

    // Functions
    grid_rkr();
    grid_rkr(int n_r_, double R_, int n_m_, maths_textbook& maths_);
};

#endif
