//
//  capillary_fibre.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "capillary_fibre" describes the dimensions and dispersion properties of a dielectric capillary
//  type fibre.
//

#ifndef __CAPILLARY_FIBRE_HPP_INCLUDED__
#define __CAPILLARY_FIBRE_HPP_INCLUDED__

#include "Eigen/Dense"
#include "grid_tw.hpp"
#include "grid_rkr.hpp"
#include "physics_textbook.hpp"
#include "maths_textbook.hpp"

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
/*!
Originally created by Patrick Anderson.
Modified by Samuel Senior on 10/03/2017.
"capillary_fibre" describes the dimensions and dispersion properties of a dielectric capillary
type fibre.
*/
class capillary_fibre {

public:
    // Data
    ArrayXXcd gamma;
    double Z;
    double R;
    double n_glass;

    // Functions
    capillary_fibre(double Z_, grid_rkr& rkr_, grid_tw& tw_, physics_textbook& physics_,
                    maths_textbook& maths_);
};

#endif
