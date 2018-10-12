//
//  grid_xkx.cpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "grid_xkx" is a linear 1D spatial grid. The spectral counterpart of this grid is evaluated and
//  made accessible.
//

#include "grid_xkx.hpp"
#include "maths_textbook.hpp"
#include "../Eigen/Dense"

using namespace Eigen;

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
//  Constructors

/*! Default */
grid_xkx::grid_xkx() { }

/*! Parameterised */
grid_xkx::grid_xkx(int N_x_, double x_min_, double x_max_) {
    
    maths_textbook maths;
    
    // Spatial
    N_x = N_x_;
    x_min = x_min_;
    x_max = x_max_;
    x = ArrayXd::Zero(N_x);
    x.setLinSpaced(x_min, x_max);
    dx = x(1) - x(0);
    
    // Spectral
    kx = ArrayXd::Zero(N_x);
    kx.setLinSpaced(-N_x / 2, (N_x / 2) - 1);
    kx *=  (2 * maths.pi / (N_x * (x(1) - x(0))));
    kx.tail(N_x / 2).swap(kx.head(N_x / 2));                                                        // Equivalent to fftshift(kx)
    
}

}
