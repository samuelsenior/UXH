#ifndef __PROPAGATION_HPP_INCLUDED__
#define __PROPAGATION_HPP_INCLUDED__

#include "physics_textbook.hpp"
#include "maths_textbook.hpp"
#include "grid_rkr.hpp"
#include "grid_tw.hpp"
#include "DHT.hpp"
#include "Eigen/Dense"

//using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
class propagation{

    // Data
    physics_textbook physics;  /*!< Physical constants */
    maths_textbook maths;      /*!< Mathematical constants and functions */
    grid_rkr rkr;              /*!< Radial grid */
    grid_tw tw;
    DHT ht;                    /*!< Hankel transform */

    Eigen::ArrayXd k_r;

    Eigen::ArrayXd w_active;
    Eigen::ArrayXXcd A_r_w;

public:
    // Data

    // Functions
    propagation(grid_rkr rkr, grid_tw tw, DHT ht);

    double n(double k);

    void propagate();
};

#endif
