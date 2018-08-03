#ifndef __PROPAGATION_HPP_INCLUDED__
#define __PROPAGATION_HPP_INCLUDED__

#include "physics_textbook.hpp"
#include "maths_textbook.hpp"
#include "grid_rkr.hpp"
#include "DHT.hpp"
#include "Eigen/Dense"

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
class propagation{

    // Data
    physics_textbook physics;  /*!< Physical constants */
    maths_textbook maths;      /*!< Mathematical constants and functions */
    grid_rkr rkr;              /*!< Radial grid */
    DHT ht;                    /*!< Hankel transform */

    ArrayXd w_active;
    ArrayXXcd A_w_active;

public:
    // Data

    // Functions
    propagation();

    void propagate();
};

#endif
