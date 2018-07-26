//
//  grid_rkr.cpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "grid_rkr" is a non-uniform radial grid. The spectral counterpart of this grid is evaluated and
//  accessible.
//

#include "grid_rkr.hpp"
#include "maths_textbook.hpp"
#include "../Eigen/Dense"

using namespace Eigen;

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Default constructor */
//grid_rkr::grid_rkr() {  }

//------------------------------------------------------------------------------------------------//
/*! Parameterized constructor */
grid_rkr::grid_rkr(int n_r_, double R_, maths_textbook& maths_) :
                   n_r(n_r_), R(R_){

    r = R * maths_.J0_zeros.head(n_r) / maths_.J0_zeros(n_r);
    kr = maths_.J0_zeros.head(n_r) / R;
}

}
