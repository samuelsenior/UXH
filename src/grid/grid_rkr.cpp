//
//  grid_rkr.cpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "grid_rkr" is a non-uniform radial grid. The spectral counterpart of this grid is evaluated and
//  accessible.
//

#include "grid_rkr.hpp"
#include "../maths/maths_textbook.hpp"
#include "../../Eigen/Dense"

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Default constructor */
grid_rkr::grid_rkr() {  }

//------------------------------------------------------------------------------------------------//
/*! Parameterized constructor */
grid_rkr::grid_rkr(int n_r_, double R_, int n_m_, maths_textbook& maths_) :
                   n_r(n_r_), R(R_), n_m(n_m_), maths(maths_){

    r = R * maths.J0_zeros.head(n_r) / maths.J0_zeros(n_r);
    kr = maths.J0_zeros.head(n_r) / R;
}

